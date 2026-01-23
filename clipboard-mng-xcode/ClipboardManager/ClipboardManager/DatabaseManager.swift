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
}
