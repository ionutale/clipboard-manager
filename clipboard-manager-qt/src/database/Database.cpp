#include "Database.h"
#include "clipboard/ClipboardItem.h"
#include "database/Note.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Database::Database(QObject *parent)
    : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                      + "/.clipboard-manager";
    QDir().mkpath(dataDir);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dataDir + "/db.sqlite");

    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return;
    }

    migrate();
}

Database::~Database()
{
    if (m_db.isOpen())
        m_db.close();
}

void Database::migrate()
{
    QSqlQuery q(m_db);

    q.exec("CREATE TABLE IF NOT EXISTS clipboard_items ("
           "id TEXT PRIMARY KEY,"
           "content TEXT NOT NULL,"
           "source_app TEXT,"
           "created_at INTEGER NOT NULL"
           ")");

    q.exec("CREATE TABLE IF NOT EXISTS notes ("
           "id TEXT PRIMARY KEY,"
           "title TEXT NOT NULL DEFAULT '',"
           "content TEXT NOT NULL DEFAULT '',"
           "created_at INTEGER NOT NULL,"
           "updated_at INTEGER NOT NULL"
           ")");
}

void Database::saveItem(const ClipboardItem &item)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO clipboard_items (id, content, source_app, created_at) "
              "VALUES (?, ?, ?, ?)");
    q.addBindValue(item.id);
    q.addBindValue(item.content);
    q.addBindValue(item.sourceApp);
    q.addBindValue(item.createdAt);
    if (!q.exec())
        qWarning() << "saveItem failed:" << q.lastError().text();
}

QVector<ClipboardItem> Database::recentItems(int limit)
{
    QVector<ClipboardItem> items;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, content, source_app, created_at FROM clipboard_items "
              "ORDER BY created_at DESC LIMIT ?");
    q.addBindValue(limit);
    if (!q.exec()) {
        qWarning() << "recentItems failed:" << q.lastError().text();
        return items;
    }
    while (q.next()) {
        items.append({q.value(0).toString(),
                      q.value(1).toString(),
                      q.value(2).toString(),
                      q.value(3).toLongLong()});
    }
    return items;
}

QVector<ClipboardItem> Database::searchItems(const QString &query, int limit)
{
    QVector<ClipboardItem> items;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, content, source_app, created_at FROM clipboard_items "
              "WHERE content LIKE ? ORDER BY created_at DESC LIMIT ?");
    q.addBindValue("%" + query + "%");
    q.addBindValue(limit);
    if (!q.exec()) {
        qWarning() << "searchItems failed:" << q.lastError().text();
        return items;
    }
    while (q.next()) {
        items.append({q.value(0).toString(),
                      q.value(1).toString(),
                      q.value(2).toString(),
                      q.value(3).toLongLong()});
    }
    return items;
}

void Database::deleteItem(const QString &id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM clipboard_items WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec())
        qWarning() << "deleteItem failed:" << q.lastError().text();
}

void Database::clearHistory()
{
    QSqlQuery q(m_db);
    q.exec("DELETE FROM clipboard_items");
}

void Database::saveNote(const Note &note)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO notes (id, title, content, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(note.id);
    q.addBindValue(note.title);
    q.addBindValue(note.content);
    q.addBindValue(note.createdAt);
    q.addBindValue(note.updatedAt);
    if (!q.exec())
        qWarning() << "saveNote failed:" << q.lastError().text();
}

QVector<Note> Database::allNotes()
{
    QVector<Note> notes;
    QSqlQuery q(m_db);
    q.exec("SELECT id, title, content, created_at, updated_at FROM notes ORDER BY updated_at DESC");
    while (q.next()) {
        notes.append({q.value(0).toString(),
                      q.value(1).toString(),
                      q.value(2).toString(),
                      q.value(3).toLongLong(),
                      q.value(4).toLongLong()});
    }
    return notes;
}

void Database::deleteNote(const QString &id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM notes WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec())
        qWarning() << "deleteNote failed:" << q.lastError().text();
}
