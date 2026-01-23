import Foundation
import GRDB

enum ClipboardItemType: String, Codable {
    case text
    case image
    case file
}

struct ClipboardItem: Identifiable, Codable, Equatable, FetchableRecord, PersistableRecord {
    var id: UUID
    var content: String // Text content or local file path
    var type: ClipboardItemType
    var timestamp: Date
    
    init(id: UUID = UUID(), content: String, type: ClipboardItemType, timestamp: Date = Date()) {
        self.id = id
        self.content = content
        self.type = type
        self.timestamp = timestamp
    }
}
