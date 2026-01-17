export interface ClipboardItem {
  content: string;
  timestamp: number;
}

export interface ElectronAPI {
  getClipboardHistory: () => Promise<ClipboardItem[]>;
  copyToClipboard: (text: string) => Promise<boolean>;
  clearHistory: () => Promise<boolean>;
  onClipboardUpdate: (callback: (history: ClipboardItem[]) => void) => void;
}

declare global {
  interface Window {
    electronAPI: ElectronAPI;
  }
}
