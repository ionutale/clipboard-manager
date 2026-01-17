import { contextBridge, ipcRenderer } from 'electron';

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('electronAPI', {
  getClipboardHistory: () => ipcRenderer.invoke('get-clipboard-history'),
  copyToClipboard: (text: string) => ipcRenderer.invoke('copy-to-clipboard', text),
  clearHistory: () => ipcRenderer.invoke('clear-history'),
  onClipboardUpdate: (callback: (history: string[]) => void) => {
    ipcRenderer.on('clipboard-update', (_event, history) => callback(history));
  },
});
