import Foundation
import AppKit

struct ClipboardItem: Identifiable, Codable, Equatable {
    let id: UUID
    let content: String
    let timestamp: Date
    
    init(content: String) {
        self.id = UUID()
        self.content = content
        self.timestamp = Date()
    }
}

class ClipboardManager: ObservableObject {
    @Published var history: [ClipboardItem] = []
    private let pasteboard = NSPasteboard.general
    private var changeCount = NSPasteboard.general.changeCount
    private var timer: Timer?
    private let maxHistory = 100
    
    init() {
        startMonitoring()
    }
    
    func startMonitoring() {
        timer = Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true) { [weak self] _ in
            self?.checkClipboard()
        }
    }
    
    private func checkClipboard() {
        guard pasteboard.changeCount != changeCount else { return }
        changeCount = pasteboard.changeCount
        
        if let newString = pasteboard.string(forType: .string), !newString.isEmpty {
            DispatchQueue.main.async {
                // Remove duplicates and add new item to top
                self.history.removeAll { $0.content == newString }
                let newItem = ClipboardItem(content: newString)
                self.history.insert(newItem, at: 0)
                
                if self.history.count > self.maxHistory {
                    self.history = Array(self.history.prefix(self.maxHistory))
                }
            }
        }
    }
    
    func copyToClipboard(_ text: String) {
        pasteboard.clearContents()
        pasteboard.setString(text, forType: .string)
        self.changeCount = pasteboard.changeCount
    }
    
    func clearHistory() {
        history = []
    }
}
