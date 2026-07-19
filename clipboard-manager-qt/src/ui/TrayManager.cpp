#include "TrayManager.h"
#include "clipboard/ClipboardMonitor.h"
#include "database/Database.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QIcon>

TrayManager::TrayManager(Database *db, QObject *parent)
    : QObject(parent)
{
    m_monitor = new ClipboardMonitor(db, this);

    auto icon = QIcon(":/app_icon_64.png");
    if (icon.isNull()) {
        icon = QIcon(QApplication::applicationDirPath() + "/../assets/app_icon_64.png");
        if (icon.isNull())
            icon = QIcon::fromTheme("edit-paste");
    }

    m_trayIcon = new QSystemTrayIcon(icon, this);
    m_trayIcon->setToolTip("Clipboard Manager");
    auto *trayMenu = new QMenu();
    trayMenu->addAction("Show/Hide", this, &TrayManager::toggleRequested);
    trayMenu->addSeparator();
    trayMenu->addAction("Quit", qApp, &QApplication::quit);
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            emit toggleRequested();
    });
}
