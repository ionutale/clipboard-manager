package services

import (
	"database/sql"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"github.com/google/uuid"
	_ "modernc.org/sqlite"
)

type DatabaseService struct {
	db *sql.DB
}

func NewDatabaseService() (*DatabaseService, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	appDir := filepath.Join(home, ".clipboard-manager")
	if err := os.MkdirAll(appDir, 0755); err != nil {
		return nil, err
	}

	dbPath := filepath.Join(appDir, "db.sqlite")
	db, err := sql.Open("sqlite", dbPath)
	if err != nil {
		return nil, err
	}

	if err := initSchema(db); err != nil {
		return nil, err
	}

	return &DatabaseService{db: db}, nil
}

func initSchema(db *sql.DB) error {
	query := `
	CREATE TABLE IF NOT EXISTS clipboardItem (
		id TEXT PRIMARY KEY,
		content TEXT NOT NULL,
		type TEXT NOT NULL,
		timestamp DATETIME NOT NULL
	);
	`
	_, err := db.Exec(query)
	return err
}

func (s *DatabaseService) SaveItem(item ClipboardItem) error {
	// Simple deduplication: remove if content+type matches most recent? 
	// For now just insert. 
    // Ideally we might want to delete older duplicates, but let's stick to insert for now.
    
    // Check if ID is empty, generate one
    if item.ID == "" {
        item.ID = uuid.New().String()
    }

	query := `INSERT INTO clipboardItem (id, content, type, timestamp) VALUES (?, ?, ?, ?)`
	_, err := s.db.Exec(query, item.ID, item.Content, item.Type, item.Timestamp)
	return err
}

func (s *DatabaseService) GetRecentItems(limit int) ([]ClipboardItem, error) {
	query := `SELECT id, content, type, timestamp FROM clipboardItem ORDER BY timestamp DESC LIMIT ?`
	rows, err := s.db.Query(query, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var items []ClipboardItem
	for rows.Next() {
		var item ClipboardItem
		var typeStr string
		var timestamp time.Time
		if err := rows.Scan(&item.ID, &item.Content, &typeStr, &timestamp); err != nil {
			continue
		}
		item.Type = ClipboardItemType(typeStr)
        item.Timestamp = timestamp
		items = append(items, item)
	}
	return items, nil
}
