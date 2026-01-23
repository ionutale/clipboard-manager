package services
package services

import (
	"time"
)

type ClipboardItemType string

const (
	TypeClipboardText  ClipboardItemType = "text"
	TypeClipboardImage ClipboardItemType = "image"
	TypeClipboardFile  ClipboardItemType = "file"
)

type ClipboardItem struct {
	ID        string            `json:"id"`
	Content   string            `json:"content"` // Text content or File Path



}	Timestamp time.Time         `json:"timestamp"`	Type      ClipboardItemType `json:"type"`