package tray

func setupTrayImpl(clickHandler func()) {
	// Windows implementation requires 'walk' or 'systray'.
    // Leaving as stub or text-only for 'Lightweight' request for now,
    // as Windows tray requires significant boilerplate code or a large library.
    // For now, Windows users will use the Taskbar icon.
}
