#ifndef NOTESTAB_H
#define NOTESTAB_H

#include <QWidget>

class QListWidget;
class QListWidgetItem;
class QTextEdit;
class QPushButton;
class Database;

class NotesTab : public QWidget
{
    Q_OBJECT
public:
    explicit NotesTab(Database *db, QWidget *parent = nullptr);

private slots:
    void onNoteSelected(int row);
    void onNewNote();
    void onDeleteNote();
    void onContentChanged();

private:
    void refreshList();

    Database *m_db;
    QListWidget *m_list;
    QTextEdit *m_editor;
    QPushButton *m_newBtn;
    QPushButton *m_deleteBtn;
    QString m_currentNoteId;
    bool m_updatingEditor = false;
};

#endif
