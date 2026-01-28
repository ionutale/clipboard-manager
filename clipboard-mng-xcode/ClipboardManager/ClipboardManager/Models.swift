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

struct Note: Identifiable, Codable, Equatable, FetchableRecord, PersistableRecord {
    var id: UUID
    var title: String
    var content: String
    var timestamp: Date
    
    init(id: UUID = UUID(), title: String, content: String, timestamp: Date = Date()) {
        self.id = id
        self.title = title
        self.content = content
        self.timestamp = timestamp
    }
}

struct Credential: Identifiable, Codable, Equatable, FetchableRecord, PersistableRecord {
    var id: UUID
    var title: String
    var username: String
    var password: String // In a real app, encrypt this!
    var url: String
    var timestamp: Date
    
    init(id: UUID = UUID(), title: String, username: String, password: String, url: String, timestamp: Date = Date()) {
        self.id = id
        self.title = title
        self.username = username
        self.password = password
        self.url = url
        self.timestamp = timestamp
    }
}
