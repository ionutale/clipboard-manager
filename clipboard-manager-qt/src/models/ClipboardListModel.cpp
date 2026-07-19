#include "ClipboardListModel.h"
#include "database/Database.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QDateTime>

ClipboardListModel::ClipboardListModel(Database *db, QObject *parent)
    : QAbstractListModel(parent)
    , m_db(db)
{
    reload();
}

int ClipboardListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant ClipboardListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};

    const auto &item = m_items[index.row()];

    switch (role) {
    case IdRole:
        return item.id;
    case ContentRole:
        return item.content;
    case SourceAppRole:
        return item.sourceApp;
    case CreatedAtRole:
        return item.createdAt;
    case DisplayPreviewRole: {
        QString preview = item.content.left(120).simplified();
        if (item.content.length() > 120)
            preview += "…";
        return preview;
    }
    case TimeAgoRole: {
        qint64 secs = QDateTime::currentSecsSinceEpoch() - item.createdAt;
        if (secs < 60) return "just now";
        if (secs < 3600) return QString("%1m ago").arg(secs / 60);
        if (secs < 86400) return QString("%1h ago").arg(secs / 3600);
        return QString("%1d ago").arg(secs / 86400);
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> ClipboardListModel::roleNames() const
{
    return {
        { IdRole, "itemId" },
        { ContentRole, "content" },
        { SourceAppRole, "sourceApp" },
        { CreatedAtRole, "createdAt" },
        { DisplayPreviewRole, "preview" },
        { TimeAgoRole, "timeAgo" }
    };
}

void ClipboardListModel::search(const QString &query)
{
    m_query = query;
    reload();
}

void ClipboardListModel::refresh()
{
    reload();
}

void ClipboardListModel::deleteItem(const QString &id)
{
    m_db->deleteItem(id);
    reload();
}

void ClipboardListModel::copyItem(const QString &id)
{
    for (const auto &item : m_items) {
        if (item.id == id) {
            QGuiApplication::clipboard()->setText(item.content);
            emit itemCopied(item.content);
            return;
        }
    }
}

void ClipboardListModel::reload()
{
    beginResetModel();
    m_items = m_query.isEmpty()
        ? m_db->recentItems(100)
        : m_db->searchItems(m_query, 100);
    endResetModel();
    emit countChanged();
}
