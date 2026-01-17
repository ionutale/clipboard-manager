# AI Coding Instructions

This file guides AI agents (GitHub Copilot) working in the Clipboard Manager codebase.

## Project Context

- **Name**: Clipboard Manager
- **Tech Stack**:
  - **Primary**: Electron + TypeScript + Svelte + Vite + pnpm
  - **Native macOS**: Swift + SwiftUI (located in `xcode/ClipboardManager`)
- **Architecture**:
  - **Electron**: Multi-process app with secure IPC communication
    - **Main Process** ([src/main/main.ts](src/main/main.ts)): Handles clipboard monitoring, system tray (Menu Bar Extra), window lifecycle
    - **Preload Script** ([src/main/preload.cjs](src/main/preload.cjs)): Secure CommonJS IPC bridge
    - **Renderer Process** ([src/renderer/](src/renderer/)): Svelte UI compiled by Vite
  - **Native**: macOS Status Bar app usingpopovers, built with Swift Package Manager

## Core Architecture Principles

### Process Separation & Security (Electron)

- Main process has full Node.js/Electron API access; renderer is sandboxed
- All IPC must go through the preload script's `contextBridge`
- Type-safe IPC: Define contracts in [src/types/electron.d.ts](src/types/electron.d.ts)

### Native macOS Pattern (Swift)

- Lives in `xcode/ClipboardManager`
- Uses system `NSPasteboard` polling (0.5s interval)
- `LSUIElement = true` is implied to stay in the menu bar
- UI built with SwiftUI `LazyVStack` for performance

### Clipboard Monitoring Pattern

- Polling-based monitoring (500ms interval) in both implementations
- Deduplication: New entries move to top, removing old duplicates
- Updates pushed to renderer (Electron) or State (Swift)

## Development Workflows

### Electron

```bash
pnpm dev          # Concurrent Vite dev server + Electron (hot reload for renderer)
pnpm build        # Compile both processes (Vite for renderer, tsc for main)
```

### Native macOS

```bash
cd xcode/ClipboardManager
swift run         # Run via Swift Package Manager
# Or open Package.swift in Xcode to build/debug
```

### TypeScript Configuration

- **Renderer** ([tsconfig.json](tsconfig.json)): `moduleResolution: "bundler"` for Vite compatibility
- **Main** ([tsconfig.main.json](tsconfig.main.json)): `moduleResolution: "node"` for Electron/Node.js compatibility
- Both use `module: "ESNext"` with ES imports

## Coding Standards

### File Organization

- Main process: `src/main/*.ts` (compiled to `dist/main/`)
- Renderer: `src/renderer/*.{ts,svelte}` (bundled to `dist/renderer/`)
- Shared types: `src/types/*.d.ts`

### Svelte Conventions

- Use `<script lang="ts">` for all components
- Prefer reactive statements (`$:`) for derived state
- Keep business logic in TypeScript modules; components should be thin

### IPC Patterns

```typescript
// Main process: Use ipcMain.handle for async operations
ipcMain.handle('operation-name', async (_event, arg) => { ... });

// Preload: Expose via contextBridge
contextBridge.exposeInMainWorld('electronAPI', {
  operationName: (arg) => ipcRenderer.invoke('operation-name', arg)
});

// Renderer: Call via window.electronAPI
await window.electronAPI.operationName(arg);
```

### Error Handling

- Return `Promise<boolean>` for operations (true = success)
- Log errors in main process using `console.error`
- Show user-friendly messages in renderer

## Critical Dependencies

- `electron`: Desktop framework
- `svelte` + `@sveltejs/vite-plugin-svelte`: UI framework + Vite integration
- `vite`: Dev server and bundler for renderer
- `concurrently`: Run Vite + Electron simultaneously in dev mode

## asdf Version Management

- Node.js and pnpm versions pinned in [.tool-versions](.tool-versions)
- Run `asdf install` to sync versions before development
