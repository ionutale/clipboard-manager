package services

import (
	"time"
)

type ClipboardItemType string

const (
	TypeClipboardText  ClipboardItemType = "text"
	TypeClipboardImage ClipboardItemType = "image"
	TypeClipboardFile  ClipboardItemType = "file"
	TypeClipboardKDBX  ClipboardItemType = "kdbx"
)

type ClipboardItem struct {
	ID        string            `json:"id"`
	Content   string            `json:"content"` // Text content or File Path
	Type      ClipboardItemType `json:"type"`
	Timestamp time.Time         `json:"timestamp"`
}