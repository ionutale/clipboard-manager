import Foundation
import AppKit

import Foundation
import AppKit

class ClipboardManager: ObservableObject {
    @Published var history: [ClipboardItem] = []
    
    // Dependencies
    private let dbManager = DatabaseManager.shared
    private let storageManager = StorageManager.shared
    
    private let pasteboard = NSPasteboard.general
    private var changeCount = NSPasteboard.general.changeCount
    private var timer: Timer?
    private let maxHistory = 100
    
    init() {
        // Load initial history from DB
        loadHistory()
        startMonitoring()
    }
    
    private func loadHistory() {
        self.history = dbManager.fetchRecent(limit: maxHistory)
    }
    
    func startMonitoring() {
        timer = Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true) { [weak self] _ in
            self?.checkClipboard()
        }
    }
    
    private func checkClipboard() {
        guard pasteboard.changeCount != changeCount else { return }
        changeCount = pasteboard.changeCount
        
        var newItem: ClipboardItem?
        
        // 1. Check for File URL (Finder copy)
        if let urls = pasteboard.readObjects(forClasses: [NSURL.self], options: nil) as? [URL],
           let url = urls.first, url.isFileURL {
            if let savedPath = storageManager.saveFile(from: url) {
                newItem = ClipboardItem(content: savedPath, type: .file)
            }
        }
        // 2. Check for Image (Screenshot/Preview copy)
        else if let images = pasteboard.readObjects(forClasses: [NSImage.self], options: nil) as? [NSImage],
                let image = images.first {
            if let savedPath = storageManager.saveImage(image) {
                newItem = ClipboardItem(content: savedPath, type: .image)
            }
        }
        // 3. Check for String (Text copy)
        else if let newString = pasteboard.string(forType: .string), !newString.isEmpty {
            newItem = ClipboardItem(content: newString, type: .text)
        }
        
        if let item = newItem {
            saveItem(item)
        }
    }
    
    private func saveItem(_ item: ClipboardItem) {
        // 1. Save to DB
        dbManager.save(item)
        
        // 2. Update In-Memory UI State
        DispatchQueue.main.async {
            // Deduplication logic: Remove existing item if same content
            self.history.removeAll { $0.content == item.content && $0.type == item.type }
            self.history.insert(item, at: 0)
            
            if self.history.count > self.maxHistory {
                self.history = Array(self.history.prefix(self.maxHistory))
            }
        }
    }
    
    func copyToClipboard(_ item: ClipboardItem) {
        pasteboard.clearContents()
        
        switch item.type {
        case .text:
            pasteboard.setString(item.content, forType: .string)
        case .image:
            if let image = NSImage(contentsOfFile: item.content) {
                pasteboard.writeObjects([image])
            }
        case .file:
            let url = URL(fileURLWithPath: item.content)
            pasteboard.writeObjects([url as NSPasteboardWriting])
        }
        
        self.changeCount = pasteboard.changeCount
    }
    
    func clearHistory() {
        // TODO: Clear DB and local files too?
        history = []
    }
}

