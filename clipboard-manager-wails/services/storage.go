package services

import (
	"fmt"
	"io"
	"os"
	"path/filepath"

	"github.com/google/uuid"
)

type StorageService struct {
	storageDir string
}

func NewStorageService() (*StorageService, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	storageDir := filepath.Join(home, ".clipboard-manager", "files")
	if err := os.MkdirAll(storageDir, 0755); err != nil {
		return nil, err
	}

	return &StorageService{storageDir: storageDir}, nil
}

func (s *StorageService) SaveImage(data []byte) (string, error) {
	filename := uuid.New().String() + ".png"
	path := filepath.Join(s.storageDir, filename)

	if err := os.WriteFile(path, data, 0644); err != nil {
		return "", err
	}
	return path, nil
}

func (s *StorageService) SaveFile(srcPath string) (string, error) {
	// Verify source exists
	srcInfo, err := os.Stat(srcPath)
	if err != nil {
		return "", err
	}
	if srcInfo.IsDir() {
		return "", fmt.Errorf("cannot save directory")
	}

	filename := uuid.New().String() + "_" + filepath.Base(srcPath)
	destPath := filepath.Join(s.storageDir, filename)

	// Copy file
	source, err := os.Open(srcPath)
	if err != nil {
		return "", err
	}
	defer source.Close()

	destination, err := os.Create(destPath)
	if err != nil {
		return "", err
	}
	defer destination.Close()

	if _, err := io.Copy(destination, source); err != nil {
		return "", err
	}

	return destPath, nil
}
