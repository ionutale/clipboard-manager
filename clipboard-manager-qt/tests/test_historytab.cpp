#include <QTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QSignalSpy>
#include <QLineEdit>
#include <QListWidget>
#include <QClipboard>
#include <QGuiApplication>
#include <QDateTime>
#include <QUuid>
#include "database/Database.h"
#include "clipboard/ClipboardItem.h"
#include "ui/HistoryTab.h"

class TestHistoryTab : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void tabDisplaysItems();
    void searchFiltersItems();
    void clickCopiesToClipboard();

private:
    QTemporaryDir *m_tempDir = nullptr;
    Database *m_db = nullptr;
    HistoryTab *m_tab = nullptr;
};

void TestHistoryTab::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    m_db = new Database(m_tempDir->filePath("test_history.db"), this);
    m_tab = new HistoryTab(m_db);

    // Seed test data
    auto now = QDateTime::currentSecsSinceEpoch();
    for (int i = 0; i < 3; ++i) {
        ClipboardItem item = {
            QUuid::createUuid().toString(QUuid::WithoutBraces),
            QString("history item %1").arg(i),
            "test",
            now + i
        };
        m_db->saveItem(item);
    }
    m_tab->resize(400, 300);
    m_tab->show();
    QTest::qWait(50);
}

void TestHistoryTab::cleanupTestCase()
{
    delete m_tempDir;
}

void TestHistoryTab::tabDisplaysItems()
{
    m_tab->refresh();
    QTest::qWait(50);

    auto *list = m_tab->findChild<QListWidget *>();
    QVERIFY(list != nullptr);
    QVERIFY(list->count() >= 3);
}

void TestHistoryTab::searchFiltersItems()
{
    m_tab->refresh();
    QTest::qWait(50);

    auto *search = m_tab->findChild<QLineEdit *>();
    QVERIFY(search != nullptr);

    // Search should narrow results
    search->setText("item 1");
    QTest::qWait(50);

    auto *list = m_tab->findChild<QListWidget *>();
    QVERIFY(list->count() >= 1);

    // Search for non-existent text
    search->setText("zzzzz");
    QTest::qWait(50);
    QCOMPARE(list->count(), 0);

    // Clear search restores items
    search->clear();
    QTest::qWait(50);
    QVERIFY(list->count() >= 3);
}

void TestHistoryTab::clickCopiesToClipboard()
{
    m_tab->refresh();
    QTest::qWait(50);

    QGuiApplication::clipboard()->clear();

    auto *list = m_tab->findChild<QListWidget *>();
    QVERIFY(list != nullptr);
    QVERIFY(list->count() > 0);

    auto *firstItem = list->item(0);
    QVERIFY(firstItem != nullptr);

    list->setCurrentItem(firstItem);
    QTest::mouseClick(list->viewport(), Qt::LeftButton, {}, list->visualItemRect(firstItem).center());
    QTest::qWait(50);

    QString copied = QGuiApplication::clipboard()->text();
    QVERIFY(!copied.isEmpty());
}

// Need QApplication for clipboard + widgets
static QApplication *app = nullptr;

int main(int argc, char *argv[])
{
    app = new QApplication(argc, argv);
    int result = QTest::qExec(new TestHistoryTab, argc, argv);
    delete app;
    return result;
}
#include "test_historytab.moc"
