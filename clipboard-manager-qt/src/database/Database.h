#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>

struct ClipboardItem;
struct Note;

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    void saveItem(const ClipboardItem &item);
    QVector<ClipboardItem> recentItems(int limit = 100);
    QVector<ClipboardItem> searchItems(const QString &query, int limit = 100);
    void deleteItem(const QString &id);
    void clearHistory();

    void saveNote(const Note &note);
    QVector<Note> allNotes();
    void deleteNote(const QString &id);

private:
    void migrate();
    QSqlDatabase m_db;
};

#endif
