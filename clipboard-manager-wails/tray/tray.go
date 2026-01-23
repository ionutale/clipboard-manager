package tray

import (
	"context"

	"github.com/wailsapp/wails/v2/pkg/runtime"
)

// SetupTray configures the system tray.
// It is platform-specific.
func SetupTray(ctx context.Context) {
	setupTrayImpl(func() {
		// For now, always Show on click.
		// Wails v2 doesn't expose a simple "IsHidden()" method to toggle reliable.
		runtime.WindowShow(ctx)
	})
}