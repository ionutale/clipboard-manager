#include <QTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QClipboard>
#include <QGuiApplication>
#include <QLineEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QSignalSpy>
#include <QPixmap>
#include <QScreen>
#include <QDir>
#include <QDateTime>
#include <QUuid>
#include "database/Database.h"
#include "clipboard/ClipboardMonitor.h"
#include "clipboard/ClipboardItem.h"
#include "ui/HistoryTab.h"
#include "ui/PopupWidget.h"

class TestE2E : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void fullClipboardFlow();
    void tabNavigation();
    void statePersistenceAcrossRefresh();
    void captureScreenshots();

private:
    QTemporaryDir *m_tempDir = nullptr;
    Database *m_db = nullptr;
    const QString m_screenshotDir = QStringLiteral("screenshots");
};

void TestE2E::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    m_db = new Database(m_tempDir->filePath("test_e2e.db"), this);

    QDir().mkpath(m_screenshotDir);
}

void TestE2E::cleanupTestCase()
{
    delete m_tempDir;
}

void TestE2E::fullClipboardFlow()
{
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

    bool found = false;
    for (int i = 0; i < list->count(); ++i) {
        QString itemText = list->item(i)->data(Qt::UserRole + 1).toString();
        if (itemText == "e2e clipboard content") {
            found = true;
            break;
        }
    }
    QVERIFY(found);

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

void TestE2E::tabNavigation()
{
    PopupWidget popup;
    popup.show();
    QTest::qWait(50);

    auto *tabs = popup.findChild<QTabWidget *>();
    QVERIFY(tabs != nullptr);
    QCOMPARE(tabs->count(), 1);
    QCOMPARE(tabs->tabText(0), "History");

    popup.close();
}

void TestE2E::statePersistenceAcrossRefresh()
{
    HistoryTab tab(m_db);
    tab.show();
    QTest::qWait(50);

    auto *search = tab.findChild<QLineEdit *>();
    QVERIFY(search != nullptr);

    search->setText("persistence");
    tab.refresh();
    QTest::qWait(50);

    QCOMPARE(search->text(), "persistence");

    tab.close();
}

void TestE2E::captureScreenshots()
{
    auto now = QDateTime::currentSecsSinceEpoch();

    // Seed sample clipboard data
    QStringList samples = {
        "git commit -m \"fix: resolve memory leak in clipboard monitor\"",
        "npx ctx7@latest library Qt6 \"Qt6 vs GTK4 comparison\"",
        "bre install qt@6 && cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)",
        "https://github.com/ionutale/clipboard-manager",
        "const auto step{ m_gridSize / scale };",
    };
    for (int i = 0; i < samples.size(); ++i) {
        ClipboardItem item = {
            QUuid::createUuid().toString(QUuid::WithoutBraces),
            samples[i], "terminal", now - i * 60
        };
        m_db->saveItem(item);
    }

    // Screenshot 1: Popup with clipboard history and active search
    {
        PopupWidget popup;
        popup.show();
        popup.activateWindow();
        QTest::qWait(200);

        // Set search text
        auto *tabs = popup.findChild<QTabWidget *>();
        QVERIFY(tabs != nullptr);

        auto *search = tabs->findChild<QLineEdit *>();
        if (search) {
            search->setText("cmake");
            QTest::qWait(100);
        }

        // Grab and save
        QPixmap pixmap = popup.grab();
        QVERIFY(!pixmap.isNull());
        bool saved = pixmap.save(m_screenshotDir + "/popup-search.png");
        QVERIFY(saved);
        qDebug() << "Saved screenshot:" << m_screenshotDir + "/popup-search.png"
                 << pixmap.width() << "x" << pixmap.height();
        popup.close();
    }

    // Screenshot 2: Full popup with all items (no search)
    {
        PopupWidget popup;
        popup.show();
        popup.activateWindow();
        QTest::qWait(200);

        QPixmap pixmap = popup.grab();
        QVERIFY(!pixmap.isNull());
        bool saved = pixmap.save(m_screenshotDir + "/popup-full.png");
        QVERIFY(saved);
        qDebug() << "Saved screenshot:" << m_screenshotDir + "/popup-full.png"
                 << pixmap.width() << "x" << pixmap.height();
        popup.close();
    }
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
