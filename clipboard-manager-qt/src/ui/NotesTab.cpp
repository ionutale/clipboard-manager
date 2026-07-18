#include "NotesTab.h"
#include "database/Note.h"
#include "database/Database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QDateTime>
#include <QUuid>

NotesTab::NotesTab(Database *db, QWidget *parent)
    : QWidget(parent)
    , m_db(db)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    auto *splitter = new QSplitter(Qt::Horizontal, this);

    auto *leftWidget = new QWidget(this);
    auto *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto *btnLayout = new QHBoxLayout();
    m_newBtn = new QPushButton("+ New", this);
    m_deleteBtn = new QPushButton("Delete", this);
    m_deleteBtn->setEnabled(false);
    btnLayout->addWidget(m_newBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addStretch();
    leftLayout->addLayout(btnLayout);

    m_list = new QListWidget(this);
    leftLayout->addWidget(m_list, 1);
    splitter->addWidget(leftWidget);

    m_editor = new QTextEdit(this);
    m_editor->setPlaceholderText("Select or create a note...");
    m_editor->setEnabled(false);
    splitter->addWidget(m_editor);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    mainLayout->addWidget(splitter, 1);

    connect(m_list, &QListWidget::currentRowChanged, this, &NotesTab::onNoteSelected);
    connect(m_newBtn, &QPushButton::clicked, this, &NotesTab::onNewNote);
    connect(m_deleteBtn, &QPushButton::clicked, this, &NotesTab::onDeleteNote);
    connect(m_editor, &QTextEdit::textChanged, this, &NotesTab::onContentChanged);

    refreshList();
}

void NotesTab::refresh()
{
    refreshList();
}

void NotesTab::refreshList()
{
    int prevRow = m_list->currentRow();
    QString prevId;
    if (prevRow >= 0)
        prevId = m_list->item(prevRow)->data(Qt::UserRole).toString();

    m_list->blockSignals(true);
    m_list->clear();

    auto notes = m_db->allNotes();
    for (const auto &note : notes) {
        QString display = note.title.isEmpty()
            ? note.content.left(40).replace('\n', ' ')
            : note.title;
        auto *item = new QListWidgetItem(display);
        item->setData(Qt::UserRole, note.id);
        item->setData(Qt::UserRole + 1, note.content);
        item->setData(Qt::UserRole + 2, note.title);
        item->setData(Qt::UserRole + 3, note.createdAt);
        m_list->addItem(item);
    }

    m_list->blockSignals(false);

    if (!prevId.isEmpty()) {
        for (int i = 0; i < m_list->count(); ++i) {
            if (m_list->item(i)->data(Qt::UserRole).toString() == prevId) {
                m_list->setCurrentRow(i);
                break;
            }
        }
    }
}

void NotesTab::onNoteSelected(int row)
{
    if (row < 0 || row >= m_list->count()) {
        m_editor->setEnabled(false);
        m_editor->clear();
        m_deleteBtn->setEnabled(false);
        m_currentNoteId.clear();
        return;
    }

    auto *item = m_list->item(row);
    m_currentNoteId = item->data(Qt::UserRole).toString();
    QString content = item->data(Qt::UserRole + 1).toString();

    m_updatingEditor = true;
    m_editor->setEnabled(true);
    m_editor->setPlainText(content);
    m_deleteBtn->setEnabled(true);
    m_updatingEditor = false;
}

void NotesTab::onNewNote()
{
    auto now = QDateTime::currentSecsSinceEpoch();
    Note note = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "Untitled", "", now, now
    };
    m_db->saveNote(note);
    refreshList();

    for (int i = 0; i < m_list->count(); ++i) {
        if (m_list->item(i)->data(Qt::UserRole).toString() == note.id) {
            m_list->setCurrentRow(i);
            break;
        }
    }
}

void NotesTab::onDeleteNote()
{
    if (m_currentNoteId.isEmpty()) return;
    m_db->deleteNote(m_currentNoteId);
    m_currentNoteId.clear();
    refreshList();
}

void NotesTab::onContentChanged()
{
    if (m_updatingEditor || m_currentNoteId.isEmpty()) return;

    for (int i = 0; i < m_list->count(); ++i) {
        auto *item = m_list->item(i);
        if (item->data(Qt::UserRole).toString() != m_currentNoteId)
            continue;

        QString content = m_editor->toPlainText();
        QString title = content.left(60).replace('\n', ' ');
        if (title.isEmpty()) title = "Untitled";

        item->setText(title);
        item->setData(Qt::UserRole + 1, content);
        item->setData(Qt::UserRole + 2, title);

        auto now = QDateTime::currentSecsSinceEpoch();
        Note note = {
            m_currentNoteId, title, content,
            item->data(Qt::UserRole + 3).toLongLong(), now
        };
        m_db->saveNote(note);
        return;
    }
}
