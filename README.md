# Clipboard Manager

A robust clipboard management application built with Electron, TypeScript, and Svelte.

## Features

- 📋 Automatic clipboard monitoring and history tracking
- 🔍 Search through clipboard history
- ⌨️ Global keyboard shortcut (Cmd/Ctrl+Shift+V)
- 🎨 Modern, clean UI built with Svelte
- 💾 Stores up to 100 clipboard items

## Tech Stack

- **Electron** - Cross-platform desktop app framework
- **TypeScript** - Type-safe development
- **Svelte** - Reactive UI framework
- **Vite** - Fast build tool for the renderer process
- **pnpm** - Fast, disk space efficient package manager
- **asdf** - Version manager for Node.js and pnpm

## Development

### Prerequisites

1. Install [asdf](https://asdf-vm.com/)
2. Install required plugins:
   ```bash
   asdf plugin add nodejs
   asdf plugin add pnpm
   ```
3. Install versions specified in `.tool-versions`:
   ```bash
   asdf install
   ```

### Setup

```bash
# Install dependencies
pnpm install

# Run in development mode
pnpm dev

# Type check
pnpm typecheck

# Lint
pnpm lint

# Format code
pnpm format
```

### Building

```bash
# Build for production
pnpm build

# Package for your platform
pnpm package

# Or specify platform
pnpm package:mac
pnpm package:win
pnpm package:linux
```

## Project Structure

```
clipboard-manager/
├── src/
│   ├── main/           # Electron main process
│   │   ├── main.ts     # Entry point, window management, clipboard monitoring
│   │   └── preload.ts  # Secure bridge between main and renderer
│   ├── renderer/       # Svelte UI
│   │   ├── App.svelte  # Main UI component
│   │   └── main.ts     # Renderer entry point
│   └── types/          # TypeScript definitions
├── dist/               # Compiled output
├── release/            # Packaged applications
└── index.html          # HTML shell
```

## License

MIT
