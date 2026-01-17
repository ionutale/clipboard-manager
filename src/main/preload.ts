import { contextBridge, ipcRenderer } from 'electron';

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('electronAPI', {
  getClipboardHistory: () => ipcRenderer.invoke('get-clipboard-history'),
  copyToClipboard: (text: string) => ipcRenderer.invoke('copy-to-clipboard', text),
  clearHistory: () => ipcRenderer.invoke('clear-history'),
  onClipboardUpdate: (callback: (history: string[]) => void) => {
    const listener = (_event: Electron.IpcRendererEvent, history: string[]) => callback(history);
    ipcRenderer.on('clipboard-update', listener);
    // Return cleanup function
    return () => ipcRenderer.removeListener('clipboard-update', listener);
  },
});
