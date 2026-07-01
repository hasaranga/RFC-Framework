package main

import (
	"encoding/xml"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

// XMLModule matches the layout of module.xml files in the C++ project
type XMLModule struct {
	XMLName      xml.Name `xml:"xml"`
	Name         string   `xml:"name"`
	Fixed        string   `xml:"fixed"`
	Dependencies string   `xml:"dependencies"`
	Platform     string   `xml:"platform"`
	Description  string   `xml:"description"`
}

// RFCModule is the Go representation of a module, returned to the UI
type RFCModule struct {
	Name                 string   `json:"name"`
	Platform             string   `json:"platform"`
	Description          string   `json:"description"`
	IsFixed              bool     `json:"isFixed"`
	TopLevelDependencies []string `json:"topLevelDependencies"`
	AllDependencies      []string `json:"allDependencies"`
	Dir                  string   `json:"dir"`
	Checked              bool     `json:"checked"`
}

// LoadModules scans a directory for subdirectories containing module.xml
func LoadModules(currentPath string) ([]*RFCModule, error) {
	entries, err := os.ReadDir(currentPath)
	if err != nil {
		return nil, err
	}

	var modules []*RFCModule
	moduleMap := make(map[string]*RFCModule)

	for _, entry := range entries {
		if entry.IsDir() {
			dirPath := filepath.Join(currentPath, entry.Name())
			moduleFilePath := filepath.Join(dirPath, "module.xml")

			if _, err := os.Stat(moduleFilePath); err == nil {
				xmlFile, err := os.Open(moduleFilePath)
				if err != nil {
					continue
				}

				byteValue, err := io.ReadAll(xmlFile)
				xmlFile.Close()
				if err != nil {
					continue
				}

				var xmlMod XMLModule
				err = xml.Unmarshal(byteValue, &xmlMod)
				if err != nil {
					continue
				}

				name := strings.TrimSpace(xmlMod.Name)
				if name == "" {
					continue
				}

				var topDeps []string
				depsStr := strings.TrimSpace(xmlMod.Dependencies)
				if depsStr != "" {
					for _, d := range strings.Split(depsStr, ",") {
						dClean := strings.TrimSpace(d)
						if dClean != "" {
							topDeps = append(topDeps, dClean)
						}
					}
				}

				mod := &RFCModule{
					Name:                 name,
					Platform:             strings.TrimSpace(xmlMod.Platform),
					Description:          strings.TrimSpace(xmlMod.Description),
					IsFixed:              strings.TrimSpace(xmlMod.Fixed) == "true",
					TopLevelDependencies: topDeps,
					AllDependencies:      []string{},
					Dir:                  dirPath,
					Checked:              strings.TrimSpace(xmlMod.Fixed) == "true",
				}

				modules = append(modules, mod)
				moduleMap[mod.Name] = mod
			}
		}
	}

	// Resolve recursive dependencies using depth-first search
	for _, m := range modules {
		var allDeps []string
		allDeps, err := fillAllDependencies(m.Name, allDeps, moduleMap)
		if err != nil {
			return nil, err
		}
		m.AllDependencies = allDeps
	}

	return modules, nil
}

// fillAllDependencies maps dependencies recursively (depth-first search)
func fillAllDependencies(moduleName string, allDeps []string, moduleMap map[string]*RFCModule) ([]string, error) {
	m, ok := moduleMap[moduleName]
	if !ok {
		return allDeps, fmt.Errorf("missing dependency: %s", moduleName)
	}

	for _, depName := range m.TopLevelDependencies {
		found := false
		for _, d := range allDeps {
			if d == depName {
				found = true
				break
			}
		}
		if !found {
			allDeps = append(allDeps, depName)
			var err error
			allDeps, err = fillAllDependencies(depName, allDeps, moduleMap)
			if err != nil {
				return nil, err
			}
		}
	}

	return allDeps, nil
}

// CheckForMissingDeps checks if any checked module is missing a checked dependency
func CheckForMissingDeps(modules []*RFCModule) string {
	moduleMap := make(map[string]*RFCModule)
	for _, m := range modules {
		moduleMap[m.Name] = m
	}

	for _, m := range modules {
		if m.Checked {
			for _, depName := range m.AllDependencies {
				depMod, ok := moduleMap[depName]
				if !ok || !depMod.Checked {
					return fmt.Sprintf("%s module requires %s module!", m.Name, depName)
				}
			}
		}
	}

	return ""
}
