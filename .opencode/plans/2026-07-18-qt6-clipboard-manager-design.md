# Qt6 Clipboard Manager — Design Spec

## Overview

A cross-platform clipboard manager built with Qt6 (C++17, QtWidgets). Fourth implementation alongside existing Electron, Wails (Go), and Swift versions. Targets macOS, Windows, and Linux with a single codebase, native look via QtWidgets platform theme.

## Goals

- System tray app with popup UI
- Clipboard text history (last 100 items) with search
- Notes CRUD (create, read, update, delete)
- KDBX password database support (v2+, deferred)
- Cross-platform: macOS, Windows, Linux
- Memory-efficient (~20-40MB)

## Non-Goals (v1)

- Image/file clipboard monitoring (text only)
- KDBX support (deferred to v2)
- Syncing or cloud features
- Plugin system

## Architecture

Single-process Qt application. No IPC.

```
TrayManager (QSystemTrayIcon)
  └── PopupWidget (QWidget, frameless, activated on tray click)
        └── QTabWidget
              ├── HistoryTab (clipboard list + search)
              └── NotesTab (list + editor)
               
Signals/slots connect UI to backend services:

ClipboardMonitor ──> Database ──> HistoryTab
User actions     ──> Database ──> NotesTab
```

## Data Layer

### Storage Location
`~/.clipboard-manager/db.sqlite` (same dir as existing implementations)

### Schema

```sql
CREATE TABLE clipboard_items (
    id          TEXT PRIMARY KEY,
    content     TEXT NOT NULL,
    source_app  TEXT,
    created_at  INTEGER NOT NULL
);

CREATE TABLE notes (
    id          TEXT PRIMARY KEY,
    title       TEXT NOT NULL DEFAULT '',
    content     TEXT NOT NULL DEFAULT '',
    created_at  INTEGER NOT NULL,
    updated_at  INTEGER NOT NULL
);
```

### C++ Models (POD structs)

```cpp
struct ClipboardItem {
    QString id;
    QString content;
    QString sourceApp;
    qint64 createdAt;
};

struct Note {
    QString id;
    QString title;
    QString content;
    qint64 createdAt;
    qint64 updatedAt;
};
```

### History Limit
`DELETE FROM clipboard_items WHERE id NOT IN (SELECT id FROM clipboard_items ORDER BY created_at DESC LIMIT 100)`

## UI Design

### Popup Window
- Frameless `QWidget`, positioned near tray icon (macOS) or screen center
- Always-on-top while open, closes on focus loss (or Esc)
- ~420x500px default size

### History Tab
- `QLineEdit` for search (filters in real-time)
- `QListWidget` with custom delegate showing content preview + timestamp + source
- Click → copy to clipboard + flash feedback
- Right-click context menu: Copy, Delete
- Keyboard navigation: ↑↓ to select, Enter to copy

### Notes Tab
- Splitter: `QListWidget` (left, ~30%) + `QTextEdit` (right, ~70%)
- Click note to edit content in right pane; changes save on focus loss
- + button to create new note
- Delete button or right-click → delete

### System Tray
- `QSystemTrayIcon` with app icon
- Left-click (macOS) or right-click (Windows/Linux) → toggle popup
- Context menu: Show/Hide, Quit

## Clipboard Monitoring

- `QTimer` at 500ms interval checks `QClipboard::text()`
- Compare against last known text; skip if unchanged
- Deduplicate: if last item in DB matches current content, skip save
- Save new items asynchronously (no UI freeze)
- On app start, seed last item from DB as baseline

## Project Structure

```
clipboard-manager-qt/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── app/
│   │   ├── Application.h/cpp
│   │   └── TrayManager.h/cpp
│   ├── clipboard/
│   │   ├── ClipboardMonitor.h/cpp
│   │   └── ClipboardItem.h
│   ├── database/
│   │   ├── Database.h/cpp
│   │   └── Note.h
│   ├── ui/
│   │   ├── PopupWidget.h/cpp
│   │   ├── HistoryTab.h/cpp
│   │   └── NotesTab.h/cpp
│   └── utils/
│       └── PlatformUtil.h/cpp
└── assets/
    └── icon.png / icon.ico / icon.icns
```

## Build System

- CMake 3.16+ with `find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)`
- Qt6 installed via Homebrew on macOS, system packages on Linux, online installer on Windows
- Single `CMakeLists.txt`, no subdirectories needed for v1

## Error Handling

- Database errors: log via `qWarning()`, show non-blocking status message in UI
- Clipboard read failures: silently retry on next poll cycle
- Graceful shutdown: save state, stop timer, close DB

## Implementation Order

1. CMake project + `main.cpp` (skeleton app with empty window)
2. `Database` class (create/read for items + notes)
3. `ClipboardMonitor` (polling + save to DB)
4. `TrayManager` + `PopupWidget` (tray icon showing window)
5. `HistoryTab` (list + search + copy)
6. `NotesTab` (CRUD with splitter layout)
7. Polish: keyboard shortcuts, context menus, icon, packaging
