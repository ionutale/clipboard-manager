#include <QTest>
#include <QTemporaryDir>
#include <QDateTime>
#include <QUuid>
#include "database/Database.h"
#include "clipboard/ClipboardItem.h"

class TestDatabase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void saveAndRetrieveItem();
    void searchItems();
    void deleteItem();
    void clearHistory();
    void limitRecentItems();
    void emptyDatabase();

private:
    QTemporaryDir *m_tempDir = nullptr;
    Database *m_db = nullptr;
};

void TestDatabase::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    m_db = new Database(m_tempDir->filePath("test.db"), this);
}

void TestDatabase::cleanupTestCase()
{
    delete m_tempDir;
}

void TestDatabase::saveAndRetrieveItem()
{
    auto now = QDateTime::currentSecsSinceEpoch();
    ClipboardItem item = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "test content",
        "TestApp",
        now
    };
    m_db->saveItem(item);

    auto items = m_db->recentItems(10);
    QCOMPARE(items.size(), 1);
    QCOMPARE(items[0].content, "test content");
    QCOMPARE(items[0].sourceApp, "TestApp");
    QCOMPARE(items[0].createdAt, now);
}

void TestDatabase::searchItems()
{
    ClipboardItem item1 = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "hello world",
        "App1",
        QDateTime::currentSecsSinceEpoch()
    };
    ClipboardItem item2 = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "goodbye world",
        "App2",
        QDateTime::currentSecsSinceEpoch()
    };
    m_db->saveItem(item1);
    m_db->saveItem(item2);

    auto results = m_db->searchItems("hello");
    QVERIFY(results.size() >= 1);
    QCOMPARE(results[0].content, "hello world");

    auto noResults = m_db->searchItems("zzzzz");
    QCOMPARE(noResults.size(), 0);
}

void TestDatabase::deleteItem()
{
    auto now = QDateTime::currentSecsSinceEpoch();
    ClipboardItem item = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "delete me",
        "Test",
        now
    };
    m_db->saveItem(item);

    auto before = m_db->recentItems(100);
    int countBefore = before.size();

    m_db->deleteItem(item.id);

    auto after = m_db->recentItems(100);
    QCOMPARE(after.size(), countBefore - 1);
    for (const auto &i : after)
        QVERIFY(i.id != item.id);
}

void TestDatabase::clearHistory()
{
    ClipboardItem item = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        "should be cleared",
        "Test",
        QDateTime::currentSecsSinceEpoch()
    };
    m_db->saveItem(item);

    m_db->clearHistory();

    auto items = m_db->recentItems(100);
    QCOMPARE(items.size(), 0);
}

void TestDatabase::limitRecentItems()
{
    auto now = QDateTime::currentSecsSinceEpoch();
    for (int i = 0; i < 5; ++i) {
        ClipboardItem item = {
            QUuid::createUuid().toString(QUuid::WithoutBraces),
            QString("item %1").arg(i),
            "limit test",
            now + i
        };
        m_db->saveItem(item);
    }

    auto items = m_db->recentItems(3);
    QCOMPARE(items.size(), 3);
}

void TestDatabase::emptyDatabase()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    Database db(tempDir.filePath("empty.db"));

    auto items = db.recentItems(100);
    QCOMPARE(items.size(), 0);

    auto search = db.searchItems("anything");
    QCOMPARE(search.size(), 0);
}

QTEST_MAIN(TestDatabase)
#include "test_database.moc"
