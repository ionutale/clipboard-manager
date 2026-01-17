export interface ElectronAPI {
  getClipboardHistory: () => Promise<string[]>;
  copyToClipboard: (text: string) => Promise<boolean>;
  clearHistory: () => Promise<boolean>;
  onClipboardUpdate: (callback: (history: string[]) => void) => void;
}

declare global {
  interface Window {
    electronAPI: ElectronAPI;
  }
}
