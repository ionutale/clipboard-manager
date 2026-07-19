#ifndef CLIPBOARDLISTMODEL_H
#define CLIPBOARDLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "clipboard/ClipboardItem.h"

class Database;

class ClipboardListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        ContentRole,
        SourceAppRole,
        CreatedAtRole,
        DisplayPreviewRole,
        TimeAgoRole
    };

    explicit ClipboardListModel(Database *db, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void search(const QString &query);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void deleteItem(const QString &id);
    Q_INVOKABLE void copyItem(const QString &id);

signals:
    void countChanged();
    void itemCopied(const QString &content);

private:
    void reload();

    Database *m_db;
    QVector<ClipboardItem> m_items;
    QString m_query;
};

#endif
