package main

import (
	"clipboard-manager-wails/services"
	"clipboard-manager-wails/tray"
	"context"
	"fmt"

	"github.com/wailsapp/wails/v2/pkg/runtime"
)

// App struct
type App struct {
	ctx       context.Context
	db        *services.DatabaseService
	storage   *services.StorageService
	clipboard *services.ClipboardService
}

// NewApp creates a new App application struct
func NewApp() *App {
	// Initialize Services
	// Error handling is skipped for simplicity, but ideally should be handled
	db, err := services.NewDatabaseService()
	if err != nil {
		fmt.Printf("Error init DB: %v\n", err)
	}
	
	storage, err := services.NewStorageService()
	if err != nil {
		fmt.Printf("Error init Storage: %v\n", err)
	}

	clip := services.NewClipboardService(db, storage)

	return &App{
		db:        db,
		storage:   storage,
		clipboard: clip,
	}
}

// startup is called when the app starts. The context is saved
// so we can call the runtime methods
func (a *App) startup(ctx context.Context) {
	a.ctx = ctx
	
	// Setup System Tray
	tray.SetupTray(ctx)
	
	// Hook up the callback
	a.clipboard.OnNewItem = func(item services.ClipboardItem) {
		runtime.EventsEmit(a.ctx, "clipboard-new-item", item)
	}

	// Start monitoring
	if err := a.clipboard.Start(); err != nil {
		fmt.Printf("Failed to start clipboard: %v\n", err)
	}
}

// GetHistory returns recent items
func (a *App) GetHistory() []services.ClipboardItem {
	items, err := a.db.GetRecentItems(100)
	if err != nil {
		return []services.ClipboardItem{}
	}
	return items
}

// CopyToClipboard puts an item back to system clipboard
func (a *App) CopyToClipboard(item services.ClipboardItem) {
	a.clipboard.Write(item)
}

// Greet returns a greeting for the given name
func (a *App) Greet(name string) string {
	return fmt.Sprintf("Hello %s, It's show time!", name)
}
