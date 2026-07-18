#include "PopupWidget.h"
#include "ui/HistoryTab.h"
#include "ui/NotesTab.h"
#include "clipboard/ClipboardMonitor.h"
#include "database/Database.h"
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QApplication>
#include <QCloseEvent>
#include <QScreen>
#include <QWindow>
#include <QGuiApplication>

PopupWidget::PopupWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Clipboard Manager");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_ShowWithoutActivating, false);
    resize(420, 500);

    auto icon = QIcon(":/app_icon_64.png");
    if (icon.isNull()) {
        icon = QIcon(QApplication::applicationDirPath() + "/../assets/app_icon_64.png");
        if (icon.isNull())
            icon = QIcon::fromTheme("edit-paste");
    }

    m_db = new Database(this);
    m_monitor = new ClipboardMonitor(m_db, this);

    m_tabWidget = new QTabWidget(this);
    m_historyTab = new HistoryTab(m_db, this);
    m_tabWidget->addTab(m_historyTab, "History");
    m_notesTab = new NotesTab(m_db, this);
    m_tabWidget->addTab(m_notesTab, "Notes");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_tabWidget);

    connect(m_monitor, &ClipboardMonitor::newItem, m_historyTab, &HistoryTab::refresh);

    m_trayIcon = new QSystemTrayIcon(icon, this);
    m_trayIcon->setToolTip("Clipboard Manager");
    auto *trayMenu = new QMenu(this);
    trayMenu->addAction("Show/Hide", this, &PopupWidget::toggleVisibility);
    trayMenu->addSeparator();
    trayMenu->addAction("Quit", qApp, &QApplication::quit);
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            toggleVisibility();
    });

    qApp->installEventFilter(this);
}

PopupWidget::~PopupWidget() {}

void PopupWidget::toggleVisibility()
{
    if (m_visible) {
        hidePopup();
    } else {
        m_historyTab->refresh();
        positionPopup();
        m_visible = true;
        show();
        raise();
        activateWindow();
    }
}

void PopupWidget::hidePopup()
{
    m_visible = false;
    hide();
}

void PopupWidget::positionPopup()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QRect screenGeom = screen->availableGeometry();
    int x = screenGeom.right() - width() - 20;
    int y = screenGeom.top() + 40;
    move(x, y);
}

void PopupWidget::closeEvent(QCloseEvent *event)
{
    hidePopup();
    event->ignore();
}

void PopupWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate && m_visible) {
        hidePopup();
    }
    QWidget::changeEvent(event);
}

bool PopupWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationDeactivate && m_visible) {
        hidePopup();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
