import Foundation
import AppKit

class StorageManager {
    static let shared = StorageManager()
    private let fileManager = FileManager.default
    
    private var storageURL: URL {
        let homeURL = fileManager.homeDirectoryForCurrentUser
        return homeURL.appendingPathComponent(".clipboard-manager/files")
    }
    
    private init() {
        try? fileManager.createDirectory(at: storageURL, withIntermediateDirectories: true)
    }
    
    func saveImage(_ image: NSImage) -> String? {
        guard let tiffData = image.tiffRepresentation,
              let bitmapImage = NSBitmapImageRep(data: tiffData),
              let pngData = bitmapImage.representation(using: .png, properties: [:]) else {
            return nil
        }
        
        let filename = UUID().uuidString + ".png"
        let fileURL = storageURL.appendingPathComponent(filename)
        
        do {
            try pngData.write(to: fileURL)
            return fileURL.path
        } catch {
            print("Failed to save image: \(error)")
            return nil
        }
    }
    
    func saveFile(from url: URL) -> String? {
        // Only save if file exists and is reachable
        guard (try? url.checkResourceIsReachable()) == true else { return nil }
        
        let filename = UUID().uuidString + "_" + url.lastPathComponent
        let destinationURL = storageURL.appendingPathComponent(filename)
        
        do {
            try fileManager.copyItem(at: url, to: destinationURL)
            return destinationURL.path
        } catch {
            print("Failed to save file: \(error)")
            return nil
        }
    }
}
