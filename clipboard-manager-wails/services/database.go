package services

import (
	"database/sql"
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
	CREATE TABLE IF NOT EXISTS notes (
		id TEXT PRIMARY KEY,
		title TEXT NOT NULL,
		content TEXT NOT NULL,
		createdAt DATETIME NOT NULL
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

func (s *DatabaseService) GetNotes() ([]Note, error) {
	query := `SELECT id, title, content, createdAt FROM notes ORDER BY createdAt DESC`
	rows, err := s.db.Query(query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var notes []Note
	for rows.Next() {
		var note Note
		if err := rows.Scan(&note.ID, &note.Title, &note.Content, &note.CreatedAt); err != nil {
			continue
		}
		notes = append(notes, note)
	}
	return notes, nil
}

func (s *DatabaseService) CreateNote(title, content string) (Note, error) {
	note := Note{
		ID:        uuid.New().String(),
		Title:     title,
		Content:   content,
		CreatedAt: time.Now(),
	}
	query := `INSERT INTO notes (id, title, content, createdAt) VALUES (?, ?, ?, ?)`
	_, err := s.db.Exec(query, note.ID, note.Title, note.Content, note.CreatedAt)
	if err != nil {
		return Note{}, err
	}
	return note, nil
}

func (s *DatabaseService) UpdateNote(id, title, content string) error {
	query := `UPDATE notes SET title = ?, content = ? WHERE id = ?`
	_, err := s.db.Exec(query, title, content, id)
	return err
}

func (s *DatabaseService) DeleteNote(id string) error {
	query := `DELETE FROM notes WHERE id = ?`
	_, err := s.db.Exec(query, id)
	return err
}
