#include <QTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QSignalSpy>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QDateTime>
#include <QUuid>
#include "database/Database.h"
#include "database/Note.h"
#include "ui/NotesTab.h"

class TestNotesTab : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void tabDisplaysNotes();
    void createNewNote();
    void selectNoteShowsEditor();
    void deleteNoteRemovesFromList();

private:
    QTemporaryDir *m_tempDir = nullptr;
    Database *m_db = nullptr;
    NotesTab *m_tab = nullptr;
};

void TestNotesTab::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    m_db = new Database(m_tempDir->filePath("test_notes.db"), this);
    m_tab = new NotesTab(m_db);
    m_tab->resize(600, 400);
    m_tab->show();
    QTest::qWait(50);

    // Seed a note
    auto now = QDateTime::currentSecsSinceEpoch();
    Note note = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "Existing Note",
        "Existing content",
        now, now
    };
    m_db->saveNote(note);
    m_tab->refresh();
    QTest::qWait(50);
}

void TestNotesTab::cleanupTestCase()
{
    delete m_tempDir;
}

void TestNotesTab::tabDisplaysNotes()
{
    auto *list = m_tab->findChild<QListWidget *>();
    QVERIFY(list != nullptr);
    QVERIFY(list->count() >= 1);
}

void TestNotesTab::createNewNote()
{
    auto *list = m_tab->findChild<QListWidget *>();
    int before = list->count();

    auto *newBtn = m_tab->findChild<QPushButton *>();
    QVERIFY(newBtn != nullptr);
    QTest::mouseClick(newBtn, Qt::LeftButton);
    QTest::qWait(50);

    QVERIFY(list->count() > before);
}

void TestNotesTab::selectNoteShowsEditor()
{
    auto *list = m_tab->findChild<QListWidget *>();
    QVERIFY(list->count() > 0);

    // Clear selection first
    list->setCurrentRow(-1);
    QTest::qWait(50);

    auto *editor = m_tab->findChild<QTextEdit *>();
    QVERIFY(editor != nullptr);
    QVERIFY(!editor->isEnabled());

    // Select a note — editor should become enabled
    list->setCurrentRow(0);
    QTest::qWait(50);

    QVERIFY(editor->isEnabled());
    QVERIFY(!editor->toPlainText().isEmpty());
}

void TestNotesTab::deleteNoteRemovesFromList()
{
    auto *list = m_tab->findChild<QListWidget *>();
    QVERIFY(list->count() > 0);

    list->setCurrentRow(0);
    QTest::qWait(50);

    // Find the Delete button (second button, after "+ New")
    auto buttons = m_tab->findChildren<QPushButton *>();
    QPushButton *deleteBtn = nullptr;
    for (auto *btn : buttons) {
        if (btn->text() == "Delete") {
            deleteBtn = btn;
            break;
        }
    }
    QVERIFY(deleteBtn != nullptr);

    int before = list->count();
    QTest::mouseClick(deleteBtn, Qt::LeftButton);
    QTest::qWait(50);

    QCOMPARE(list->count(), before - 1);
}

static QApplication *app = nullptr;

int main(int argc, char *argv[])
{
    app = new QApplication(argc, argv);
    int result = QTest::qExec(new TestNotesTab, argc, argv);
    delete app;
    return result;
}
#include "test_notestab.moc"
