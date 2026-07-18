#include <QTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QClipboard>
#include <QGuiApplication>
#include <QSignalSpy>
#include <QDateTime>
#include <QUuid>
#include "database/Database.h"
#include "clipboard/ClipboardMonitor.h"
#include "clipboard/ClipboardItem.h"

class TestClipboard : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void monitorCapturesText();
    void monitorIgnoresDuplicate();
    void monitorEmitsSignal();
    void monitorSkipsEmpty();

private:
    QTemporaryDir *m_tempDir = nullptr;
    Database *m_db = nullptr;
    ClipboardMonitor *m_monitor = nullptr;
};

void TestClipboard::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    m_db = new Database(m_tempDir->filePath("test_clipboard.db"), this);
    m_monitor = new ClipboardMonitor(m_db, this);
}

void TestClipboard::cleanupTestCase()
{
    delete m_tempDir;
}

void TestClipboard::monitorCapturesText()
{
    QGuiApplication::clipboard()->setText("monitored text");
    QTest::qWait(100);

    QMetaObject::invokeMethod(m_monitor, "checkClipboard", Qt::DirectConnection);

    auto items = m_db->recentItems(10);
    QVERIFY(items.size() >= 1);
    QCOMPARE(items[0].content, "monitored text");
}

void TestClipboard::monitorIgnoresDuplicate()
{
    QGuiApplication::clipboard()->setText("duplicate test");
    QTest::qWait(100);

    QMetaObject::invokeMethod(m_monitor, "checkClipboard", Qt::DirectConnection);
    int count1 = m_db->recentItems(100).size();

    QMetaObject::invokeMethod(m_monitor, "checkClipboard", Qt::DirectConnection);
    int count2 = m_db->recentItems(100).size();

    QCOMPARE(count1, count2);
}

void TestClipboard::monitorEmitsSignal()
{
    QSignalSpy spy(m_monitor, &ClipboardMonitor::newItem);

    QGuiApplication::clipboard()->setText("signal test text");
    QTest::qWait(100);
    QMetaObject::invokeMethod(m_monitor, "checkClipboard", Qt::DirectConnection);

    QCOMPARE(spy.count(), 1);
}

void TestClipboard::monitorSkipsEmpty()
{
    int before = m_db->recentItems(100).size();

    QGuiApplication::clipboard()->clear();
    QTest::qWait(100);
    QMetaObject::invokeMethod(m_monitor, "checkClipboard", Qt::DirectConnection);

    int after = m_db->recentItems(100).size();
    QCOMPARE(before, after);
}

static QApplication *app = nullptr;

int main(int argc, char *argv[])
{
    app = new QApplication(argc, argv);
    int result = QTest::qExec(new TestClipboard, argc, argv);
    delete app;
    return result;
}
#include "test_clipboard.moc"
