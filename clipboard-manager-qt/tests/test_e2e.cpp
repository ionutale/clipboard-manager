#include <QTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QClipboard>
#include <QGuiApplication>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QSignalSpy>
#include <QDateTime>
#include <QUuid>
#include "database/Database.h"
#include "clipboard/ClipboardMonitor.h"
#include "clipboard/ClipboardItem.h"
#include "ui/HistoryTab.h"
#include "ui/NotesTab.h"
#include "ui/PopupWidget.h"

class TestE2E : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void fullClipboardFlow();
    void fullNotesFlow();
    void crossTabNavigation();
    void statePersistenceAcrossRefresh();

private:
    QTemporaryDir *m_tempDir = nullptr;
    Database *m_db = nullptr;
};

void TestE2E::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    m_db = new Database(m_tempDir->filePath("test_e2e.db"), this);
}

void TestE2E::cleanupTestCase()
{
    delete m_tempDir;
}

void TestE2E::fullClipboardFlow()
{
    // Simulate: copy text → appears in history → click to copy again
    auto now = QDateTime::currentSecsSinceEpoch();
    ClipboardItem item = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "e2e clipboard content",
        "E2ETest",
        now
    };
    m_db->saveItem(item);

    HistoryTab tab(m_db);
    tab.show();
    QTest::qWait(50);
    tab.refresh();
    QTest::qWait(50);

    auto *list = tab.findChild<QListWidget *>();
    QVERIFY(list != nullptr);
    QVERIFY(list->count() >= 1);

    // Verify our item is in the list
    bool found = false;
    for (int i = 0; i < list->count(); ++i) {
        QString itemText = list->item(i)->data(Qt::UserRole + 1).toString();
        if (itemText == "e2e clipboard content") {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    // Test search
    auto *search = tab.findChild<QLineEdit *>();
    QVERIFY(search != nullptr);
    search->setText("e2e clip");
    QTest::qWait(50);
    QVERIFY(list->count() >= 1);

    search->setText("nonexistent");
    QTest::qWait(50);
    QCOMPARE(list->count(), 0);

    search->clear();
    QTest::qWait(50);
    QVERIFY(list->count() >= 1);

    tab.close();
}

void TestE2E::fullNotesFlow()
{
    NotesTab tab(m_db);
    tab.show();
    QTest::qWait(50);
    tab.refresh();
    QTest::qWait(50);

    auto *list = tab.findChild<QListWidget *>();
    QVERIFY(list != nullptr);
    int initialCount = list->count();

    // Create a note
    auto *newBtn = tab.findChild<QPushButton *>();
    QVERIFY(newBtn != nullptr);
    QCOMPARE(newBtn->text(), "+ New");

    QTest::mouseClick(newBtn, Qt::LeftButton);
    QTest::qWait(50);
    QCOMPARE(list->count(), initialCount + 1);

    // Select the new note and verify editor is enabled
    list->setCurrentRow(0);
    QTest::qWait(50);

    auto *editor = tab.findChild<QTextEdit *>();
    QVERIFY(editor != nullptr);
    QVERIFY(editor->isEnabled());

    // Edit content
    editor->setPlainText("Edited content from E2E test");
    QTest::qWait(50);

    // Verify persistence by reloading tab
    NotesTab tab2(m_db);
    tab2.show();
    QTest::qWait(50);
    tab2.refresh();
    QTest::qWait(50);

    auto *list2 = tab2.findChild<QListWidget *>();
    QVERIFY(list2 != nullptr);
    QVERIFY(list2->count() >= 1);

    // Verify note still exists with edited content
    list2->setCurrentRow(0);
    QTest::qWait(50);

    auto *editor2 = tab2.findChild<QTextEdit *>();
    QVERIFY(editor2 != nullptr);
    QCOMPARE(editor2->toPlainText(), "Edited content from E2E test");

    tab.close();
    tab2.close();
}

void TestE2E::crossTabNavigation()
{
    PopupWidget popup;
    popup.show();
    QTest::qWait(50);

    // Popup should have a QTabWidget with at least 2 tabs
    auto *tabs = popup.findChild<QTabWidget *>();
    QVERIFY(tabs != nullptr);
    QVERIFY(tabs->count() >= 2);
    QCOMPARE(tabs->tabText(0), "History");
    QCOMPARE(tabs->tabText(1), "Notes");

    // Click on each tab and verify they have content
    tabs->setCurrentIndex(0);
    QTest::qWait(50);
    QVERIFY(tabs->currentWidget() != nullptr);

    tabs->setCurrentIndex(1);
    QTest::qWait(50);
    QVERIFY(tabs->currentWidget() != nullptr);

    popup.close();
}

void TestE2E::statePersistenceAcrossRefresh()
{
    HistoryTab tab(m_db);
    tab.show();
    QTest::qWait(50);

    auto *search = tab.findChild<QLineEdit *>();
    QVERIFY(search != nullptr);

    // Set search text and refresh
    search->setText("persistence");
    tab.refresh();
    QTest::qWait(50);

    // Search text should be preserved after refresh
    QCOMPARE(search->text(), "persistence");

    tab.close();
}

static QApplication *app = nullptr;

int main(int argc, char *argv[])
{
    app = new QApplication(argc, argv);
    int result = QTest::qExec(new TestE2E, argc, argv);
    delete app;
    return result;
}
#include "test_e2e.moc"
