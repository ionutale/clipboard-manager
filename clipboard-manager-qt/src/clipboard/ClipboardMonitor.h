#ifndef CLIPBOARDMONITOR_H
#define CLIPBOARDMONITOR_H

#include <QObject>
#include <QTimer>

class QClipboard;
class Database;

class ClipboardMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardMonitor(Database *db, QObject *parent = nullptr);

signals:
    void newItem();

private slots:
    void checkClipboard();

private:
    QClipboard *m_clipboard;
    Database *m_db;
    QTimer m_timer;
    QString m_lastContent;
};

#endif
