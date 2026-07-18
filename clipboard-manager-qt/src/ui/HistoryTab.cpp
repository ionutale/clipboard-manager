#include "HistoryTab.h"
#include "database/Database.h"
#include "clipboard/ClipboardItem.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QGuiApplication>
#include <QClipboard>
#include <QMenu>
#include <QDateTime>

HistoryTab::HistoryTab(Database *db, QWidget *parent)
    : QWidget(parent)
    , m_db(db)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    m_search = new QLineEdit(this);
    m_search->setPlaceholderText("Search clipboard history...");
    layout->addWidget(m_search);

    m_list = new QListWidget(this);
    layout->addWidget(m_list, 1);

    connect(m_search, &QLineEdit::textChanged, this, &HistoryTab::onSearch);
    connect(m_list, &QListWidget::itemClicked, this, &HistoryTab::onItemClicked);
    connect(m_list, &QWidget::customContextMenuRequested, this, &HistoryTab::onContextMenu);
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);

    refresh();
}

void HistoryTab::refresh()
{
    m_list->clear();
    auto items = m_search->text().isEmpty()
        ? m_db->recentItems(100)
        : m_db->searchItems(m_search->text(), 100);

    for (const auto &item : items) {
        auto *listItem = new QListWidgetItem();
        QString preview = item.content.left(80);
        if (item.content.length() > 80)
            preview += "...";
        preview.replace('\n', ' ');

        QString timeStr = QDateTime::fromSecsSinceEpoch(item.createdAt)
                              .toString("hh:mm AP");

        listItem->setText(preview + "\n" + timeStr);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setData(Qt::UserRole + 1, item.content);
        listItem->setToolTip(item.content);
        m_list->addItem(listItem);
    }
}

void HistoryTab::onSearch(const QString &text)
{
    Q_UNUSED(text);
    refresh();
}

void HistoryTab::onItemClicked(QListWidgetItem *item)
{
    QString content = item->data(Qt::UserRole + 1).toString();
    QGuiApplication::clipboard()->setText(content);
}

void HistoryTab::onContextMenu(const QPoint &pos)
{
    auto *item = m_list->itemAt(pos);
    if (!item) return;

    QMenu menu(this);
    menu.addAction("Copy", [item]() {
        QGuiApplication::clipboard()->setText(item->data(Qt::UserRole + 1).toString());
    });
    menu.addAction("Delete", [this, item]() {
        m_db->deleteItem(item->data(Qt::UserRole).toString());
        refresh();
    });
    menu.exec(m_list->mapToGlobal(pos));
}
