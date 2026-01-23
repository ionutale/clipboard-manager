package services

import (
	"os"

	"github.com/tobischo/gokeepasslib/v3"
)

type KdbxEntry struct {
	Title    string `json:"title"`
	UserName string `json:"username"`
	Password string `json:"password"`
	URL      string `json:"url"`
	Notes    string `json:"notes"`
}

type KdbxGroup struct {
	Name    string      `json:"name"`
	Groups  []KdbxGroup `json:"groups"`
	Entries []KdbxEntry `json:"entries"`
}

func UnlockKdbx(path string, password string) (KdbxGroup, error) {
	file, err := os.Open(path)
	if err != nil {
		return KdbxGroup{}, err
	}
	defer file.Close()

	db := gokeepasslib.NewDatabase()
	db.Credentials = gokeepasslib.NewPasswordCredentials(password)

	if err := gokeepasslib.NewDecoder(file).Decode(db); err != nil {
		return KdbxGroup{}, err
	}

	if err := db.UnlockProtectedEntries(); err != nil {
		return KdbxGroup{}, err
	}

	// Helper to convert recursive groups
	var convertGroup func(g gokeepasslib.Group) KdbxGroup
	convertGroup = func(g gokeepasslib.Group) KdbxGroup {
		out := KdbxGroup{
			Name: g.Name,
		}
		for _, sub := range g.Groups {
			out.Groups = append(out.Groups, convertGroup(sub))
		}
		for _, ent := range g.Entries {
			out.Entries = append(out.Entries, KdbxEntry{
				Title:    ent.GetTitle(),
				UserName: ent.GetContent("UserName"),
				Password: ent.GetPassword(),
				URL:      ent.GetContent("URL"),
				Notes:    ent.GetContent("Notes"),
			})
		}
		return out
	}

	// Assuming Root group exists
	if len(db.Content.Root.Groups) > 0 {
		return convertGroup(db.Content.Root.Groups[0]), nil
	}

	return KdbxGroup{}, nil
}
