const { contextBridge, ipcRenderer } = require('electron');

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('electronAPI', {
  getClipboardHistory: () => ipcRenderer.invoke('get-clipboard-history'),
  copyToClipboard: (text) => ipcRenderer.invoke('copy-to-clipboard', text),
  clearHistory: () => ipcRenderer.invoke('clear-history'),
  onClipboardUpdate: (callback) => {
    const listener = (_event, history) => callback(history);
    ipcRenderer.on('clipboard-update', listener);
    // Return cleanup function
    return () => ipcRenderer.removeListener('clipboard-update', listener);
  },
});
