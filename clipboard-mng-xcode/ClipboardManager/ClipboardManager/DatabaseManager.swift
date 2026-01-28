import Foundation
import GRDB

class DatabaseManager {
    static let shared = DatabaseManager()
    
    var dbQueue: DatabaseQueue!
    
    private init() {
        setupDatabase()
    }
    
    private func setupDatabase() {
        do {
            let fileManager = FileManager.default
            let homeURL = fileManager.homeDirectoryForCurrentUser
            let appFolderURL = homeURL.appendingPathComponent(".clipboard-manager")
            
            try fileManager.createDirectory(at: appFolderURL, withIntermediateDirectories: true)
            
            let dbURL = appFolderURL.appendingPathComponent("db.sqlite")
            
            var configuration = Configuration()
            configuration.prepareDatabase { db in
                // db.trace { print($0) } // Unwrap to debug SQL queries
            }
            
            dbQueue = try DatabaseQueue(path: dbURL.path, configuration: configuration)
            
            try migrator.migrate(dbQueue)
            
        } catch {
            print("Database setup error: \(error)")
        }
    }
    
    private var migrator: DatabaseMigrator {
        var migrator = DatabaseMigrator()
        
        migrator.registerMigration("v1") { db in
            try db.create(table: "clipboardItem") { t in
                t.column("id", .text).primaryKey()
                t.column("content", .text).notNull()
                t.column("type", .text).notNull()
                t.column("timestamp", .datetime).notNull()
            }
        }
        
        migrator.registerMigration("v2") { db in
            try db.create(table: "note") { t in
                t.column("id", .text).primaryKey()
                t.column("title", .text).notNull()
                t.column("content", .text).notNull()
                t.column("timestamp", .datetime).notNull()
            }
        }
        
        migrator.registerMigration("v3") { db in
            try db.create(table: "credential") { t in
                t.column("id", .text).primaryKey()
                t.column("title", .text).notNull()
                t.column("username", .text).notNull()
                t.column("password", .text).notNull()
                t.column("url", .text).notNull()
                t.column("timestamp", .datetime).notNull()
            }
        }
        
        return migrator
    }
    
    func save(_ item: ClipboardItem) {
        do {
            try dbQueue.write { db in
                try item.save(db)
            }
        } catch {
            print("Failed to save item: \(error)")
        }
    }
    
    func fetchRecent(limit: Int = 100) -> [ClipboardItem] {
        do {
            return try dbQueue.read { db in
                try ClipboardItem
                    .order(Column("timestamp").desc)
                    .limit(limit)
                    .fetchAll(db)
            }
        } catch {
            print("Fetch error: \(error)")
            return []
        }
    }
    
    // MARK: - Notes
    
    func saveNote(_ note: Note) {
        do {
            try dbQueue.write { db in
                try note.save(db)
            }
        } catch {
            print("Failed to save note: \(error)")
        }
    }
    
    func updateNote(_ note: Note) {
         do {
            try dbQueue.write { db in
                try note.update(db)
            }
        } catch {
            print("Failed to update note: \(error)")
        }
    }
    
    func deleteNote(_ note: Note) {
        do {
            try dbQueue.write { db in
                try note.delete(db)
            }
        } catch {
            print("Failed to delete note: \(error)")
        }
    }
    
    func fetchNotes() -> [Note] {
        do {
            return try dbQueue.read { db in
                try Note
                    .order(Column("timestamp").desc)
                    .fetchAll(db)
            }
        } catch {
            print("Fetch notes error: \(error)")
            return []
        }
    }
    
    // MARK: - Credentials
    
    func saveCredential(_ credential: Credential) {
        do {
            try dbQueue.write { db in
                try credential.save(db)
            }
        } catch {
             print("Failed to save credential: \(error)")
        }
    }
    
    func updateCredential(_ credential: Credential) {
         do {
            try dbQueue.write { db in
                try credential.update(db)
            }
        } catch {
            print("Failed to update credential: \(error)")
        }
    }
    
    func deleteCredential(_ credential: Credential) {
        do {
            try dbQueue.write { db in
                try credential.delete(db)
            }
        } catch {
            print("Failed to delete credential: \(error)")
        }
    }
    
    func fetchCredentials() -> [Credential] {
        do {
            return try dbQueue.read { db in
                try Credential
                    .order(Column("timestamp").desc)
                    .fetchAll(db)
            }
        } catch {
            print("Fetch credentials error: \(error)")
            return []
        }
    }
}
