#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class Database;
class ClipboardMonitor;

class TrayManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayManager(Database *db, QObject *parent = nullptr);

signals:
    void toggleRequested();

private:
    QSystemTrayIcon *m_trayIcon;
    ClipboardMonitor *m_monitor;
};

#endif
