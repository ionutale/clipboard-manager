# Qt6 Clipboard Manager — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a cross-platform Qt6 (C++17, QtWidgets) clipboard manager with text history, search, notes CRUD, and system tray.

**Architecture:** Single-process Qt app. `ClipboardMonitor` polls `QClipboard` on a 500ms timer, saves to SQLite via `QSqlDatabase`. `PopupWidget` with `QTabWidget` shows history + notes. `QSystemTrayIcon` hosts the popup.

**Tech Stack:** Qt6.10 (Widgets, Sql), CMake 3.16+, C++17, SQLite3

---

### File Structure

```
clipboard-manager-qt/
├── CMakeLists.txt
├── assets/
│   └── app_icon_64.png              # copy of existing assets/icon_web_Large_64x64.png
└── src/
    ├── main.cpp
    ├── clipboard/
    │   ├── ClipboardItem.h
    │   └── ClipboardMonitor.h
    │   └── ClipboardMonitor.cpp
    ├── database/
    │   ├── Note.h
    │   ├── Database.h
    │   └── Database.cpp
    └── ui/
        ├── PopupWidget.h
        ├── PopupWidget.cpp
        ├── HistoryTab.h
        ├── HistoryTab.cpp
        ├── NotesTab.h
        └── NotesTab.cpp
```

### Task 1: CMake project + skeleton main.cpp

**Files:**
- Create: `clipboard-manager-qt/CMakeLists.txt`
- Create: `clipboard-manager-qt/src/main.cpp`
- Create: `clipboard-manager-qt/assets/` (symlink or copy icon)

- [ ] **Step 1: Create directory structure**

```bash
cd /Users/ionutale/developer/clipboard-manager
mkdir -p clipboard-manager-qt/src/clipboard clipboard-manager-qt/src/database clipboard-manager-qt/src/ui clipboard-manager-qt/assets
cp assets/icon_web_Large_64x64.png clipboard-manager-qt/assets/app_icon_64.png
```

- [ ] **Step 2: Write CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.16)
project(ClipboardManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Sql)

add_executable(clipboard-manager-qt
    src/main.cpp
    src/clipboard/ClipboardMonitor.cpp
    src/database/Database.cpp
    src/ui/PopupWidget.cpp
    src/ui/HistoryTab.cpp
    src/ui/NotesTab.cpp
)

target_link_libraries(clipboard-manager-qt PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

target_include_directories(clipboard-manager-qt PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
```

- [ ] **Step 3: Write main.cpp skeleton**

```cpp
#include <QApplication>
#include "ui/PopupWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Clipboard Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("clipboard-manager");

    PopupWidget popup;
    popup.show();

    return app.exec();
}
```

- [ ] **Step 4: Create stub PopupWidget.h**

```cpp
#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QWidget>

class QSystemTrayIcon;
class QTabWidget;
class HistoryTab;
class NotesTab;
class ClipboardMonitor;
class Database;

class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr);
    ~PopupWidget();

private:
    QSystemTrayIcon *m_trayIcon;
    QTabWidget *m_tabWidget;
    HistoryTab *m_historyTab;
    NotesTab *m_notesTab;
    ClipboardMonitor *m_monitor;
    Database *m_db;
};

#endif
```

- [ ] **Step 5: Create stub PopupWidget.cpp**

```cpp
#include "PopupWidget.h"
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenu>

PopupWidget::PopupWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(420, 500);
    m_tabWidget = new QTabWidget(this);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_tabWidget);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/app_icon_64.png"), this);
    m_trayIcon->setToolTip("Clipboard Manager");
    auto *trayMenu = new QMenu(this);
    trayMenu->addAction("Show", this, &QWidget::show);
    trayMenu->addAction("Quit", qApp, &QApplication::quit);
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();
}

PopupWidget::~PopupWidget() {}
```

- [ ] **Step 6: Build and verify**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt6) 2>&1
cmake --build build 2>&1
```

Expected: Build succeeds, produces `build/clipboard-manager-qt`.

### Task 2: Database — models + schema + CRUD

**Files:**
- Create: `src/database/Note.h`
- Create: `src/database/Database.h`
- Create: `src/database/Database.cpp`
- Modify: `src/main.cpp`
- Modify: `src/ui/PopupWidget.h`
- Modify: `src/ui/PopupWidget.cpp`

- [ ] **Step 1: Write Note.h**

```cpp
#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QDateTime>

struct Note {
    QString id;
    QString title;
    QString content;
    qint64 createdAt;
    qint64 updatedAt;

    static Note create(const QString &title, const QString &content)
    {
        auto now = QDateTime::currentSecsSinceEpoch();
        return {
            QUuid::createUuid().toString(QUuid::WithoutBraces),
            title, content, now, now
        };
    }
};

#endif
```

- [ ] **Step 2: Write ClipboardItem.h**

```cpp
#ifndef CLIPBOARDITEM_H
#define CLIPBOARDITEM_H

#include <QString>
#include <QDateTime>

struct ClipboardItem {
    QString id;
    QString content;
    QString sourceApp;
    qint64 createdAt;
};

#endif
```

- [ ] **Step 3: Write Database.h**

```cpp
#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include "database/Note.h"
#include "clipboard/ClipboardItem.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    // Clipboard items
    void saveItem(const ClipboardItem &item);
    QVector<ClipboardItem> recentItems(int limit = 100);
    QVector<ClipboardItem> searchItems(const QString &query, int limit = 100);
    void deleteItem(const QString &id);
    void clearHistory();

    // Notes
    void saveNote(const Note &note);
    QVector<Note> allNotes();
    void deleteNote(const QString &id);

private:
    void migrate();
    QSqlDatabase m_db;
};

#endif
```

- [ ] **Step 4: Write Database.cpp**

```cpp
#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>

Database::Database(QObject *parent)
    : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                      + "/.clipboard-manager";
    QDir().mkpath(dataDir);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dataDir + "/db.sqlite");

    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return;
    }

    migrate();
}

Database::~Database()
{
    if (m_db.isOpen())
        m_db.close();
}

void Database::migrate()
{
    QSqlQuery q(m_db);

    q.exec("CREATE TABLE IF NOT EXISTS clipboard_items ("
           "id TEXT PRIMARY KEY,"
           "content TEXT NOT NULL,"
           "source_app TEXT,"
           "created_at INTEGER NOT NULL"
           ")");

    q.exec("CREATE TABLE IF NOT EXISTS notes ("
           "id TEXT PRIMARY KEY,"
           "title TEXT NOT NULL DEFAULT '',"
           "content TEXT NOT NULL DEFAULT '',"
           "created_at INTEGER NOT NULL,"
           "updated_at INTEGER NOT NULL"
           ")");
}

void Database::saveItem(const ClipboardItem &item)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO clipboard_items (id, content, source_app, created_at) "
              "VALUES (?, ?, ?, ?)");
    q.addBindValue(item.id);
    q.addBindValue(item.content);
    q.addBindValue(item.sourceApp);
    q.addBindValue(item.createdAt);
    if (!q.exec())
        qWarning() << "saveItem failed:" << q.lastError().text();
}

QVector<ClipboardItem> Database::recentItems(int limit)
{
    QVector<ClipboardItem> items;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, content, source_app, created_at FROM clipboard_items "
              "ORDER BY created_at DESC LIMIT ?");
    q.addBindValue(limit);
    if (!q.exec()) {
        qWarning() << "recentItems failed:" << q.lastError().text();
        return items;
    }
    while (q.next()) {
        items.append({
            q.value(0).toString(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toLongLong()
        });
    }
    return items;
}

QVector<ClipboardItem> Database::searchItems(const QString &query, int limit)
{
    QVector<ClipboardItem> items;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, content, source_app, created_at FROM clipboard_items "
              "WHERE content LIKE ? ORDER BY created_at DESC LIMIT ?");
    q.addBindValue("%" + query + "%");
    q.addBindValue(limit);
    if (!q.exec()) {
        qWarning() << "searchItems failed:" << q.lastError().text();
        return items;
    }
    while (q.next()) {
        items.append({
            q.value(0).toString(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toLongLong()
        });
    }
    return items;
}

void Database::deleteItem(const QString &id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM clipboard_items WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec())
        qWarning() << "deleteItem failed:" << q.lastError().text();
}

void Database::clearHistory()
{
    QSqlQuery q(m_db);
    q.exec("DELETE FROM clipboard_items");
}

void Database::saveNote(const Note &note)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO notes (id, title, content, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(note.id);
    q.addBindValue(note.title);
    q.addBindValue(note.content);
    q.addBindValue(note.createdAt);
    q.addBindValue(note.updatedAt);
    if (!q.exec())
        qWarning() << "saveNote failed:" << q.lastError().text();
}

QVector<Note> Database::allNotes()
{
    QVector<Note> notes;
    QSqlQuery q(m_db);
    q.exec("SELECT id, title, content, created_at, updated_at FROM notes ORDER BY updated_at DESC");
    while (q.next()) {
        notes.append({
            q.value(0).toString(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toLongLong(),
            q.value(4).toLongLong()
        });
    }
    return notes;
}

void Database::deleteNote(const QString &id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM notes WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec())
        qWarning() << "deleteNote failed:" << q.lastError().text();
}
```

- [ ] **Step 5: Update main.cpp to wire up Database**

```cpp
#include <QApplication>
#include <QIcon>
#include "ui/PopupWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Clipboard Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("clipboard-manager");
    app.setWindowIcon(QIcon(":/app_icon_64.png"));

    PopupWidget popup;
    popup.show();

    return app.exec();
}
```

- [ ] **Step 6: Update PopupWidget.h — add Database include**

```cpp
#include "database/Database.h"
...
    Database *m_db;
```

- [ ] **Step 7: Update PopupWidget.cpp — construct Database**

```cpp
#include "PopupWidget.h"
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QApplication>

PopupWidget::PopupWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(420, 500);

    m_db = new Database(this);

    m_tabWidget = new QTabWidget(this);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_tabWidget);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/app_icon_64.png"), this);
    m_trayIcon->setToolTip("Clipboard Manager");
    auto *trayMenu = new QMenu(this);
    trayMenu->addAction("Show", this, &QWidget::show);
    trayMenu->addAction("Quit", qApp, &QApplication::quit);
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();
}

PopupWidget::~PopupWidget() {}
```

- [ ] **Step 8: Build and verify**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
cmake --build build 2>&1
```

Expected: Build succeeds.

### Task 3: ClipboardMonitor

**Files:**
- Create: `src/clipboard/ClipboardMonitor.h`
- Create: `src/clipboard/ClipboardMonitor.cpp`
- Modify: `src/ui/PopupWidget.h`
- Modify: `src/ui/PopupWidget.cpp`
- Modify: `src/main.cpp`

- [ ] **Step 1: Write ClipboardMonitor.h**

```cpp
#ifndef CLIPBOARDMONITOR_H
#define CLIPBOARDMONITOR_H

#include <QObject>
#include <QTimer>

class QClipboard;
class Database;

class ClipboardMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardMonitor(Database *db, QObject *parent = nullptr);

signals:
    void newItem();

private slots:
    void checkClipboard();

private:
    QClipboard *m_clipboard;
    Database *m_db;
    QTimer m_timer;
    QString m_lastContent;
};

#endif
```

- [ ] **Step 2: Write ClipboardMonitor.cpp**

```cpp
#include "ClipboardMonitor.h"
#include "clipboard/ClipboardItem.h"
#include "database/Database.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QDateTime>

ClipboardMonitor::ClipboardMonitor(Database *db, QObject *parent)
    : QObject(parent)
    , m_clipboard(QGuiApplication::clipboard())
    , m_db(db)
{
    connect(&m_timer, &QTimer::timeout, this, &ClipboardMonitor::checkClipboard);
    m_timer.start(500);
}

void ClipboardMonitor::checkClipboard()
{
    QString text = m_clipboard->text();
    if (text.isEmpty() || text == m_lastContent)
        return;

    m_lastContent = text;

    QString sourceApp = m_clipboard->owner() ? "unknown" : QString();

    auto now = QDateTime::currentSecsSinceEpoch();
    ClipboardItem item = {
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        text,
        sourceApp,
        now
    };

    m_db->saveItem(item);
    emit newItem();
}
```

- [ ] **Step 3: Add include to ClipboardMonitor.h (for QUuid)**

```cpp
#include <QUuid>
```

Actually, QUuid will work via the Qt6 headers. Let me fix the .cpp to include it.

- [ ] **Step 4: Fix ClipboardMonitor.cpp — add QUuid include**

```cpp
#include "ClipboardMonitor.h"
#include "clipboard/ClipboardItem.h"
#include "database/Database.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QDateTime>
#include <QUuid>
```

- [ ] **Step 5: Update PopupWidget.h — add ClipboardMonitor include**

```cpp
#include "clipboard/ClipboardMonitor.h"
...
    ClipboardMonitor *m_monitor;
```

- [ ] **Step 6: Update PopupWidget.cpp — construct ClipboardMonitor after m_db**

Add to PopupWidget constructor after `m_db = new Database(this);`:

```cpp
    m_monitor = new ClipboardMonitor(m_db, this);
```

- [ ] **Step 7: Build and verify**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
cmake --build build 2>&1
```

Expected: Build succeeds.

### Task 4: HistoryTab

**Files:**
- Create: `src/ui/HistoryTab.h`
- Create: `src/ui/HistoryTab.cpp`
- Modify: `src/ui/PopupWidget.cpp`

- [ ] **Step 1: Write HistoryTab.h**

```cpp
#ifndef HISTORYTAB_H
#define HISTORYTAB_H

#include <QWidget>

class QListWidget;
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
```

- [ ] **Step 2: Write HistoryTab.cpp**

```cpp
#include "HistoryTab.h"
#include "database/Database.h"
#include "clipboard/ClipboardItem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QGuiApplication>
#include <QClipboard>
#include <QMenu>
#include <QDateTime>

HistoryTab::HistoryTab(Database *db, QWidget *parent)
    : QWidget(parent)
    , m_db(db)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    m_search = new QLineEdit(this);
    m_search->setPlaceholderText("Search clipboard history...");
    layout->addWidget(m_search);

    m_list = new QListWidget(this);
    layout->addWidget(m_list, 1);

    connect(m_search, &QLineEdit::textChanged, this, &HistoryTab::onSearch);
    connect(m_list, &QListWidget::itemClicked, this, &HistoryTab::onItemClicked);
    connect(m_list, &QWidget::customContextMenuRequested, this, &HistoryTab::onContextMenu);
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);

    refresh();
}

void HistoryTab::refresh()
{
    m_list->clear();
    auto items = m_search->text().isEmpty()
        ? m_db->recentItems(100)
        : m_db->searchItems(m_search->text(), 100);

    for (const auto &item : items) {
        auto *listItem = new QListWidgetItem();
        QString preview = item.content.left(80);
        if (item.content.length() > 80)
            preview += "...";
        preview.replace('\n', ' ');

        QString source = item.sourceApp.isEmpty() ? "unknown" : item.sourceApp;
        QString timeStr = QDateTime::fromSecsSinceEpoch(item.createdAt)
                              .toString("hh:mm AP");

        listItem->setText(preview + "\n" + timeStr + "  •  " + source);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setData(Qt::UserRole + 1, item.content);
        listItem->setToolTip(item.content);
        m_list->addItem(listItem);
    }
}

void HistoryTab::onSearch(const QString &text)
{
    Q_UNUSED(text);
    refresh();
}

void HistoryTab::onItemClicked(QListWidgetItem *item)
{
    QString content = item->data(Qt::UserRole + 1).toString();
    QGuiApplication::clipboard()->setText(content);
}

void HistoryTab::onContextMenu(const QPoint &pos)
{
    auto *item = m_list->itemAt(pos);
    if (!item) return;

    QMenu menu(this);
    menu.addAction("Copy", [item]() {
        QGuiApplication::clipboard()->setText(item->data(Qt::UserRole + 1).toString());
    });
    menu.addAction("Delete", [this, item]() {
        m_db->deleteItem(item->data(Qt::UserRole).toString());
        refresh();
    });
    menu.exec(m_list->mapToGlobal(pos));
}
```

- [ ] **Step 3: Update PopupWidget.cpp — add HistoryTab to tab widget**

Replace the tab widget construction block:

```cpp
    m_db = new Database(this);
    m_monitor = new ClipboardMonitor(m_db, this);

    m_tabWidget = new QTabWidget(this);

    m_historyTab = new HistoryTab(m_db, this);
    m_tabWidget->addTab(m_historyTab, "History");

    connect(m_monitor, &ClipboardMonitor::newItem, m_historyTab, &HistoryTab::refresh);
```

- [ ] **Step 4: Build and verify**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
cmake --build build 2>&1
```

Expected: Build succeeds.

### Task 5: NotesTab

**Files:**
- Create: `src/ui/NotesTab.h`
- Create: `src/ui/NotesTab.cpp`
- Modify: `src/ui/PopupWidget.cpp`

- [ ] **Step 1: Write NotesTab.h**

```cpp
#ifndef NOTESTAB_H
#define NOTESTAB_H

#include <QWidget>

class QListWidget;
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
```

- [ ] **Step 2: Write NotesTab.cpp**

```cpp
#include "NotesTab.h"
#include "database/Note.h"
#include "database/Database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>

NotesTab::NotesTab(Database *db, QWidget *parent)
    : QWidget(parent)
    , m_db(db)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    auto *splitter = new QSplitter(Qt::Horizontal, this);

    // Left panel: note list + buttons
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

    // Right panel: editor
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

void NotesTab::refreshList()
{
    int prevRow = m_list->currentRow();
    m_list->blockSignals(true);
    m_list->clear();

    auto notes = m_db->allNotes();
    for (const auto &note : notes) {
        QString title = note.title.isEmpty()
            ? note.content.left(40).replace('\n', ' ')
            : note.title;
        auto *item = new QListWidgetItem(title);
        item->setData(Qt::UserRole, note.id);
        item->setData(Qt::UserRole + 1, note.content);
        item->setData(Qt::UserRole + 2, note.title);
        m_list->addItem(item);
    }

    m_list->blockSignals(false);
    if (prevRow >= 0 && prevRow < m_list->count())
        m_list->setCurrentRow(prevRow);
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
    QString title = item->data(Qt::UserRole + 2).toString();

    m_updatingEditor = true;
    m_editor->setEnabled(true);
    m_editor->setPlainText(content);
    m_deleteBtn->setEnabled(true);
    m_updatingEditor = false;
}

void NotesTab::onNewNote()
{
    Note note = Note::create("Untitled", "");
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

        item->setData(Qt::UserRole + 1, content);
        item->setData(Qt::UserRole + 2, title);
        item->setText(title);

        auto now = QDateTime::currentSecsSinceEpoch();
        Note note = {
            m_currentNoteId, title, content,
            item->data(Qt::UserRole + 3).toLongLong(), now
        };
        m_db->saveNote(note);
        return;
    }
}
```

Note: The `Note::create` call sets `createdAt` and `updatedAt`, but when loading from DB we don't track `createdAt` per-item in the QListWidget. The Note struct's `createdAt` field in the save path needs a proper value. Let me fix this.

Actually wait, when we load from DB in `allNotes()`, we get the full struct including `createdAt`. Let me store it in the list item too. Let me update the refreshList() and onContentChanged() methods.

Actually, let me simplify: the `onContentChanged` save will use the existing note's `createdAt` by storing it in `Qt::UserRole + 3`.

Let me update the refreshList():

```cpp
void NotesTab::refreshList()
{
    int prevRow = m_list->currentRow();
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
    if (prevRow >= 0 && prevRow < m_list->count())
        m_list->setCurrentRow(prevRow);
}
```

And in onNoteSelected, also store display title. Let me also include the note title in the item.

Actually I need to redo this more carefully. Let me rewrite the whole NotesTab.cpp properly.

- [ ] **Step 3: Rewrite NotesTab.cpp more carefully**

```cpp
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
```

- [ ] **Step 4: Update PopupWidget.cpp — add NotesTab**

```cpp
    m_notesTab = new NotesTab(m_db, this);
    m_tabWidget->addTab(m_notesTab, "Notes");
```

Also add the member initialization in PopupWidget.h.

- [ ] **Step 5: Update PopupWidget.h — add NotesTab include and member**

```cpp
#include "ui/NotesTab.h"
...
    NotesTab *m_notesTab;
```

- [ ] **Step 6: Build and verify**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
cmake --build build 2>&1
```

Expected: Build succeeds.

### Task 6: Tray behavior + popup positioning + polish

**Files:**
- Modify: `src/ui/PopupWidget.h`
- Modify: `src/ui/PopupWidget.cpp`

- [ ] **Step 1: Update PopupWidget.h — add tray toggle, positioning, focus-loss close**

```cpp
#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QWidget>

class QSystemTrayIcon;
class QTabWidget;
class QCloseEvent;
class HistoryTab;
class NotesTab;
class ClipboardMonitor;
class Database;

class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr);
    ~PopupWidget();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void toggleVisibility();

private:
    void positionPopup();

    QSystemTrayIcon *m_trayIcon;
    QTabWidget *m_tabWidget;
    HistoryTab *m_historyTab;
    NotesTab *m_notesTab;
    ClipboardMonitor *m_monitor;
    Database *m_db;
};

#endif
```

- [ ] **Step 2: Rewrite PopupWidget.cpp with full behavior**

```cpp
#include "PopupWidget.h"
#include "ui/HistoryTab.h"
#include "ui/NotesTab.h"
#include "clipboard/ClipboardMonitor.h"
#include "database/Database.h"
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QApplication>
#include <QCloseEvent>
#include <QScreen>
#include <QWindow>
#include <QGuiApplication>

PopupWidget::PopupWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Clipboard Manager");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_ShowWithoutActivating, false);
    resize(420, 500);

    m_db = new Database(this);
    m_monitor = new ClipboardMonitor(m_db, this);

    m_tabWidget = new QTabWidget(this);

    m_historyTab = new HistoryTab(m_db, this);
    m_tabWidget->addTab(m_historyTab, "History");

    m_notesTab = new NotesTab(m_db, this);
    m_tabWidget->addTab(m_notesTab, "Notes");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_tabWidget);

    connect(m_monitor, &ClipboardMonitor::newItem, m_historyTab, &HistoryTab::refresh);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/app_icon_64.png"), this);
    m_trayIcon->setToolTip("Clipboard Manager");
    auto *trayMenu = new QMenu(this);
    trayMenu->addAction("Show/Hide", this, &PopupWidget::toggleVisibility);
    trayMenu->addSeparator();
    trayMenu->addAction("Quit", qApp, &QApplication::quit);
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            toggleVisibility();
    });

    qApp->installEventFilter(this);
}

PopupWidget::~PopupWidget() {}

void PopupWidget::toggleVisibility()
{
    if (isVisible()) {
        hide();
    } else {
        positionPopup();
        show();
        raise();
        activateWindow();
        m_historyTab->refresh();
    }
}

void PopupWidget::positionPopup()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QRect screenGeom = screen->availableGeometry();
    int x = screenGeom.right() - width() - 20;
    int y = screenGeom.top() + 40;
    move(x, y);
}

void PopupWidget::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

bool PopupWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationDeactivate && isVisible()) {
        hide();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
```

- [ ] **Step 3: Change main.cpp to hide instead of show (tray-only start)**

```cpp
#include <QApplication>
#include <QIcon>
#include "ui/PopupWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Clipboard Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("clipboard-manager");
    app.setWindowIcon(QIcon(":/app_icon_64.png"));

    PopupWidget popup;

    return app.exec();
}
```

- [ ] **Step 4: Fix CMakeLists.txt — add resource file for icon**

Add to CMakeLists.txt:
```cmake
set(CMAKE_AUTORCC ON)
```
And create `resources.qrc`:
```xml
<RCC>
    <qresource prefix="/">
        <file>assets/app_icon_64.png</file>
    </qresource>
</RCC>
```

Then add to sources:
```cmake
add_executable(clipboard-manager-qt
    resources.qrc
    src/main.cpp
    ...
)
```

- [ ] **Step 5: Write resources.qrc**

```xml
<RCC>
    <qresource prefix="/">
        <file>assets/app_icon_64.png</file>
    </qresource>
</RCC>
```

- [ ] **Step 6: Build and verify**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt6) 2>&1
cmake --build build 2>&1
```

Expected: Build succeeds with no errors or warnings.

### Task 7: Verify app runs

- [ ] **Step 1: Run the app briefly and check for startup errors**

```bash
cd /Users/ionutale/developer/clipboard-manager/clipboard-manager-qt
./build/clipboard-manager-qt &
sleep 2
kill %1 2>/dev/null
```

Expected: No crash, no error output. Tray icon appears in menu bar.

- [ ] **Step 2: Verify database file created**

```bash
ls -la ~/.clipboard-manager/db.sqlite
```

Expected: db.sqlite exists and has non-zero size.
