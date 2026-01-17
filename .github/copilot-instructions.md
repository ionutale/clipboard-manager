# AI Coding Instructions

This file guides AI agents (GitHub Copilot) working in the Clipboard Manager codebase.

## Project Context
- **Name**: Clipboard Manager
- **Tech Stack**: Electron + TypeScript + Svelte + Vite + pnpm
- **Architecture**: Multi-process Electron app with secure IPC communication
  - **Main Process** ([src/main/main.ts](src/main/main.ts)): Handles clipboard monitoring, global shortcuts, window lifecycle
  - **Preload Script** ([src/main/preload.ts](src/main/preload.ts)): Secure IPC bridge using `contextBridge`
  - **Renderer Process** ([src/renderer/](src/renderer/)): Svelte UI compiled by Vite

## Core Architecture Principles

### Process Separation & Security
- Main process has full Node.js/Electron API access; renderer is sandboxed
- **Never** use `nodeIntegration: true` or disable `contextIsolation`
- All IPC must go through the preload script's `contextBridge`
- Type-safe IPC: Define contracts in [src/types/electron.d.ts](src/types/electron.d.ts)

### Clipboard Monitoring Pattern
- Polling-based monitoring (500ms interval) in main process
- Deduplication: New entries move to top, removing old duplicates
- Max history limit (100 items) enforced via array slicing
- Updates pushed to renderer via `webContents.send('clipboard-update')`

## Development Workflows

```bash
pnpm dev          # Concurrent Vite dev server + Electron (hot reload for renderer)
pnpm build        # Compile both processes (Vite for renderer, tsc for main)
pnpm typecheck    # Check types across both tsconfigs
pnpm package      # Build + package for current platform
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
