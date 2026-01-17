import { app, BrowserWindow, clipboard, globalShortcut, ipcMain } from 'electron';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

let mainWindow: BrowserWindow | null = null;
let clipboardHistory: string[] = [];
const MAX_HISTORY = 100;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });

  // In development, load from Vite dev server
  if (process.env.NODE_ENV === 'development') {
    mainWindow.loadURL('http://localhost:5173');
    mainWindow.webContents.openDevTools();
  } else {
    mainWindow.loadFile(path.join(__dirname, '../renderer/index.html'));
  }

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

function startClipboardMonitoring() {
  let previousText = clipboard.readText();

  // Check clipboard every 500ms
  setInterval(() => {
    const currentText = clipboard.readText();

    if (currentText && currentText !== previousText) {
      previousText = currentText;

      // Add to history, avoiding duplicates
      clipboardHistory = [currentText, ...clipboardHistory.filter(item => item !== currentText)];

      // Limit history size
      if (clipboardHistory.length > MAX_HISTORY) {
        clipboardHistory = clipboardHistory.slice(0, MAX_HISTORY);
      }

      // Notify renderer
      if (mainWindow) {
        mainWindow.webContents.send('clipboard-update', clipboardHistory);
      }
    }
  }, 500);
}

app.whenReady().then(() => {
  createWindow();
  startClipboardMonitoring();

  // Register global shortcut to show/hide window
  globalShortcut.register('CommandOrControl+Shift+V', () => {
    if (mainWindow) {
      if (mainWindow.isVisible()) {
        mainWindow.hide();
      } else {
        mainWindow.show();
        mainWindow.focus();
      }
    }
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
