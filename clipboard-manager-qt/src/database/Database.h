#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>

struct ClipboardItem;

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    explicit Database(const QString &dbPath, QObject *parent = nullptr);
    ~Database();

    void saveItem(const ClipboardItem &item);
    QVector<ClipboardItem> recentItems(int limit = 100);
    QVector<ClipboardItem> searchItems(const QString &query, int limit = 100);
    void deleteItem(const QString &id);
    void clearHistory();

private:
    static int s_connectionCounter;
    void migrate();
    QSqlDatabase m_db;
    QString m_connectionName;
};

#endif
