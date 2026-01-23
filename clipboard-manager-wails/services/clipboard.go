package services

import (
	"context"
	"fmt"
	"os"
	"strings"
	"time"

	"golang.design/x/clipboard"
)

type ClipboardService struct {
	db      *DatabaseService
	storage *StorageService
	ctx     context.Context // Wails context for events (optional or passed in methods)
    // Callback to notify App
    OnNewItem func(ClipboardItem)
}

func NewClipboardService(db *DatabaseService, storage *StorageService) *ClipboardService {
	return &ClipboardService{
		db:      db,
		storage: storage,
	}
}

func (s *ClipboardService) Start() error {
	err := clipboard.Init()
	if err != nil {
		return fmt.Errorf("failed to init clipboard: %w", err)
	}

	// Watch Text
	go s.watchText()
	// Watch Image
	go s.watchImage()

	return nil
}

func (s *ClipboardService) watchText() {
	ch := clipboard.Watch(context.Background(), clipboard.FmtText)
	for data := range ch {
		text := string(data)
		if text == "" {
			continue
		}

		item := ClipboardItem{
			Content:   text,
			Type:      TypeClipboardText,
			Timestamp: time.Now(),
		}

		// Check for KDBX file
		if strings.HasSuffix(strings.ToLower(text), ".kdbx") {
			if _, err := os.Stat(text); err == nil {
				item.Type = TypeClipboardKDBX
			}
		}

		s.processItem(item)
	}
}

func (s *ClipboardService) watchImage() {
	ch := clipboard.Watch(context.Background(), clipboard.FmtImage)
	for data := range ch {
		if len(data) == 0 {
			continue
		}

		path, err := s.storage.SaveImage(data)
		if err != nil {
			fmt.Printf("Failed to save image: %v\n", err)
			continue
		}

		item := ClipboardItem{
			Content:   path,
			Type:      TypeClipboardImage,
			Timestamp: time.Now(),
		}
		s.processItem(item)
	}
}

func (s *ClipboardService) processItem(item ClipboardItem) {
    // Save to DB
    if err := s.db.SaveItem(item); err != nil {
        fmt.Printf("Error saving to DB: %v\n", err)
    }

    // Notify UI
    if s.OnNewItem != nil {
        s.OnNewItem(item)
    }
}

// Write to clipboard
func (s *ClipboardService) Write(item ClipboardItem) error {
    if item.Type == TypeClipboardText {
        clipboard.Write(clipboard.FmtText, []byte(item.Content))
    } else if item.Type == TypeClipboardImage {
        // Read file back
        // logical implementation TODO
        // clipboard.Write(clipboard.FmtImage, data)
    }
    return nil
}
