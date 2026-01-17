import { app, BrowserWindow, clipboard, globalShortcut, ipcMain, Tray, Menu, nativeImage, Point } from 'electron';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

let mainWindow: BrowserWindow | null = null;
let tray: Tray | null = null;
let clipboardHistory: string[] = [];
const MAX_HISTORY = 100;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 350,
    height: 600,
    show: false,
    frame: false,
    fullscreenable: false,
    resizable: false,
    transparent: true,
    webPreferences: {
      preload: path.join(__dirname, 'preload.cjs'),
      contextIsolation: true,
      nodeIntegration: false,
      sandbox: false,
      backgroundThrottling: false,
    },
  });

  console.log('[Main] Preload path:', path.join(__dirname, 'preload.cjs'));

  // In development, load from Vite dev server
  if (!app.isPackaged) {
    mainWindow.loadURL('http://localhost:5173');
    mainWindow.webContents.openDevTools({ mode: 'detach' });
  } else {
    mainWindow.loadFile(path.join(__dirname, '../renderer/index.html'));
  }

  // Hide window when it loses focus
  mainWindow.on('blur', () => {
    if (!mainWindow?.webContents.isDevToolsOpened()) {
      mainWindow?.hide();
    }
  });

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

function createTray() {
  const icon = nativeImage.createEmpty();
  tray = new Tray(icon);
  tray.setToolTip('Clipboard Manager');
  tray.setTitle('📋'); // macOS specific: sets title in menu bar

  tray.on('click', (_event, bounds) => {
    // bounds might be undefined on some platforms, but usually consistent on macOS
    toggleWindow(bounds);
  });

  tray.on('right-click', () => {
    const contextMenu = Menu.buildFromTemplate([
      { label: 'Quit', type: 'normal', click: () => app.quit() }
    ]);
    tray?.popUpContextMenu(contextMenu);
  });
}

function toggleWindow(trayBounds?: Electron.Rectangle) {
  if (mainWindow?.isVisible()) {
    mainWindow.hide();
  } else {
    showWindow(trayBounds);
  }
}

function showWindow(trayBounds?: Electron.Rectangle) {
  const position = getWindowPosition(trayBounds);
  mainWindow?.setPosition(position.x, position.y, false);
  mainWindow?.show();
  mainWindow?.focus();
}

function getWindowPosition(trayBounds?: Electron.Rectangle): Point {
  const windowBounds = mainWindow!.getBounds();
  const bounds = trayBounds || tray!.getBounds();

  // Center window horizontally below the tray icon
  const x = Math.round(bounds.x + (bounds.width / 2) - (windowBounds.width / 2));
  // Position window 4 pixels vertically below the tray icon
  const y = Math.round(bounds.y + bounds.height + 4);

  return { x, y };
}

function startClipboardMonitoring() {
  let previousText = clipboard.readText();
  console.log('[Clipboard] Initial text:', previousText ? `"${previousText.slice(0, 50)}..."` : '(empty)');

  // Check clipboard every 500ms
  setInterval(() => {
    const currentText = clipboard.readText();

    if (currentText && currentText !== previousText) {
      console.log('[Clipboard] New content detected:', currentText.slice(0, 100));
      previousText = currentText;

      // Add to history, avoiding duplicates
      clipboardHistory = [currentText, ...clipboardHistory.filter(item => item !== currentText)];

      // Limit history size
      if (clipboardHistory.length > MAX_HISTORY) {
        clipboardHistory = clipboardHistory.slice(0, MAX_HISTORY);
      }

      console.log('[Clipboard] History size:', clipboardHistory.length);

      // Notify renderer
      if (mainWindow) {
        mainWindow.webContents.send('clipboard-update', clipboardHistory);
        console.log('[Clipboard] Sent update to renderer');
      }
    }
  }, 500);
}

app.whenReady().then(() => {
  if (process.platform === 'darwin') {
    app.dock.hide();
  }

  createWindow();
  createTray();
  startClipboardMonitoring();

  // Register global shortcut to show/hide window
  globalShortcut.register('CommandOrControl+Shift+V', () => {
    toggleWindow();
  });

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('will-quit', () => {
  globalShortcut.unregisterAll();
});

// IPC handlers
ipcMain.handle('get-clipboard-history', () => {
  return clipboardHistory;
});

ipcMain.handle('copy-to-clipboard', (_event, text: string) => {
  clipboard.writeText(text);
  return true;
});

ipcMain.handle('clear-history', () => {
  clipboardHistory = [];
  return true;
});
