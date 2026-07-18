#ifndef HISTORYTAB_H
#define HISTORYTAB_H

#include <QWidget>

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class Database;

class HistoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryTab(Database *db, QWidget *parent = nullptr);

public slots:
    void refresh();

private slots:
    void onSearch(const QString &text);
    void onItemClicked(QListWidgetItem *item);
    void onContextMenu(const QPoint &pos);

private:
    Database *m_db;
    QLineEdit *m_search;
    QListWidget *m_list;
};

#endif
