#include "ClipboardMonitor.h"
#include "clipboard/ClipboardItem.h"
#include "database/Database.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QDateTime>
#include <QUuid>

ClipboardMonitor::ClipboardMonitor(Database *db, QObject *parent)
    : QObject(parent)
    , m_clipboard(QGuiApplication::clipboard())
    , m_db(db)
{
    connect(&m_timer, &QTimer::timeout, this, &ClipboardMonitor::checkClipboard);
    m_timer.start(500);
}

void ClipboardMonitor::checkClipboard()
{
    QString text = m_clipboard->text();
    if (text.isEmpty() || text == m_lastContent)
        return;

    m_lastContent = text;

    auto now = QDateTime::currentSecsSinceEpoch();
    ClipboardItem item = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        text,
        QString(),
        now
    };

    m_db->saveItem(item);
    emit newItem();
}
