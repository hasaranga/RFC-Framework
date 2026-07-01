package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/lxn/walk"
	. "github.com/lxn/walk/declarative"
	"github.com/lxn/win"
)

type ModuleModel struct {
	walk.TableModelBase
	items    []*RFCModule
	owner    walk.Form
	updateUI func()
}

func (m *ModuleModel) RowCount() int {
	return len(m.items)
}

func (m *ModuleModel) Value(row, col int) interface{} {
	if col != 0 {
		return nil
	}
	return m.items[row].Name + " Module"
}

func (m *ModuleModel) Checked(row int) bool {
	return m.items[row].Checked
}

func (m *ModuleModel) SetChecked(row int, checked bool) error {
	item := m.items[row]
	if item.IsFixed {
		item.Checked = true
		m.PublishRowChanged(row)
		return nil
	}

	if checked {
		// Identify unchecked dependencies
		var uncheckedDeps []*RFCModule
		for _, depName := range item.AllDependencies {
			var depMod *RFCModule
			for _, mItem := range m.items {
				if mItem.Name == depName {
					depMod = mItem
					break
				}
			}
			if depMod != nil && !depMod.Checked {
				uncheckedDeps = append(uncheckedDeps, depMod)
			}
		}

		if len(uncheckedDeps) > 0 {
			var depNames []string
			for _, dm := range uncheckedDeps {
				depNames = append(depNames, dm.Name)
			}
			depNamesStr := strings.Join(depNames, ", ")

			res := walk.MsgBox(m.owner, "Confirm",
				"Following new dependencies will be added. Do you want to continue?\r\n\r\nDependencies: "+depNamesStr,
				walk.MsgBoxYesNo|walk.MsgBoxIconQuestion)

			if res == win.IDYES {
				for _, dm := range uncheckedDeps {
					dm.Checked = true
				}
				item.Checked = true
			} else {
				item.Checked = false
			}
		} else {
			item.Checked = true
		}
	} else {
		item.Checked = false
	}

	m.PublishRowsReset()
	m.updateUI()
	return nil
}

func main() {
	var mainWindow *walk.MainWindow
	var outputDirEdit *walk.LineEdit
	var tableView *walk.TableView
	var descriptionEdit *walk.TextEdit
	var statusLabel *walk.Label
	var generateBtn *walk.PushButton
	var browseBtn *walk.PushButton

	// 1. Locate modules path and load modules
	modulesPath := getModulesPath()
	modules, err := LoadModules(modulesPath)
	if err != nil {
		log.Fatalf("Failed to load modules: %v", err)
	}

	model := &ModuleModel{
		items: modules,
	}

	updateUIStatus := func() {
		var checkedObjs []*RFCModule
		for _, m := range model.items {
			if m.Checked {
				checkedObjs = append(checkedObjs, m)
			}
		}

		stat := CheckForMissingDeps(checkedObjs)
		if stat != "" {
			statusLabel.SetText(stat)
			statusLabel.SetTextColor(walk.RGB(239, 68, 68)) // Red
			generateBtn.SetEnabled(false)
		} else {
			statusLabel.SetText("no issues")
			statusLabel.SetTextColor(walk.RGB(16, 185, 129)) // Green
			
			if len(checkedObjs) > 0 && outputDirEdit.Text() != "" {
				generateBtn.SetEnabled(true)
			} else {
				generateBtn.SetEnabled(false)
			}
		}
	}

	model.updateUI = updateUIStatus

	// 2. Build MainWindow
	err = MainWindow{
		AssignTo: &mainWindow,
		Title:    "RFC Generator",
		MinSize:  Size{Width: 523, Height: 463},
		MaxSize:  Size{Width: 523, Height: 463},
		Layout:   VBox{Margins: Margins{Left: 14, Top: 14, Right: 14, Bottom: 14}, Spacing: 10},
		Children: []Widget{
			Composite{
				Layout: HBox{Margins: Margins{Left: 0, Top: 0, Right: 0, Bottom: 0}, Spacing: 10},
				Children: []Widget{
					Label{
						Text:    "Output Dir",
						MinSize: Size{Width: 63, Height: 0},
					},
					LineEdit{
						AssignTo: &outputDirEdit,
						ReadOnly: true,
					},
					PushButton{
						AssignTo: &browseBtn,
						Text:     "...",
						MinSize:  Size{Width: 29, Height: 0},
						OnClicked: func() {
							dlg := new(walk.FileDialog)
							dlg.Title = "Select Output Folder"
							if ok, err := dlg.ShowBrowseFolder(mainWindow); err != nil {
								walk.MsgBox(mainWindow, "Error", err.Error(), walk.MsgBoxIconError)
							} else if ok {
								outputDirEdit.SetText(dlg.FilePath)

								configFile := filepath.Join(dlg.FilePath, "rfc.conf")
								if _, err := os.Stat(configFile); err == nil {
									data, err := os.ReadFile(configFile)
									if err == nil {
										lines := strings.Split(string(data), "\n")
										loadedMap := make(map[string]bool)
										for _, line := range lines {
											trimmed := strings.TrimSpace(line)
											if trimmed != "" {
												loadedMap[trimmed] = true
											}
										}

										for _, item := range model.items {
											if !item.IsFixed {
												item.Checked = loadedMap[item.Name]
											}
										}
										model.PublishRowsReset()
									}
								}
								updateUIStatus()
							}
						},
					},
				},
			},
			Label{
				Text: "Available Modules",
			},
			TableView{
				AssignTo:       &tableView,
				HeaderHidden:   true,
				CheckBoxes:     true,
				MultiSelection: false,
				Columns: []TableViewColumn{
					{Title: "Module Name"},
				},
				Model: model,
				OnCurrentIndexChanged: func() {
					idx := tableView.CurrentIndex()
					if idx >= 0 && idx < len(model.items) {
						item := model.items[idx]
						depsStr := strings.Join(item.AllDependencies, ", ")
						descText := fmt.Sprintf("%s\r\n\r\nDependencies: %s\r\nOS Support: %s",
							item.Description, depsStr, item.Platform)
						descriptionEdit.SetText(descText)
					}
				},
			},
			TextEdit{
				AssignTo: &descriptionEdit,
				ReadOnly: true,
				VScroll:  true,
			},
			Composite{
				Layout: HBox{Margins: Margins{Left: 0, Top: 0, Right: 0, Bottom: 0}, Spacing: 10},
				Children: []Widget{
					Label{
						Text: "Status:",
					},
					Label{
						AssignTo: &statusLabel,
						Text:     "no issues",
					},
					HSpacer{},
					PushButton{
						AssignTo: &generateBtn,
						Text:     "Generate",
						Enabled:  false,
						OnClicked: func() {
							outDir := outputDirEdit.Text()
							if outDir == "" {
								walk.MsgBox(mainWindow, "Error", "Please select the output folder!", walk.MsgBoxIconError)
								return
							}

							var checkedNames []string
							var checkedObjs []*RFCModule
							for _, m := range model.items {
								if m.Checked {
									checkedNames = append(checkedNames, m.Name)
									checkedObjs = append(checkedObjs, m)
								}
							}

							configFile := filepath.Join(outDir, "rfc.conf")
							configData := strings.Join(checkedNames, "\n")
							_ = os.WriteFile(configFile, []byte(configData), 0644)

							// Disable UI
							browseBtn.SetEnabled(false)
							generateBtn.SetEnabled(false)
							tableView.SetEnabled(false)

							go func() {
								var logs bytes.Buffer
								success, err := handleGenerate(checkedObjs, outDir, &logs)

								mainWindow.Synchronize(func() {
									browseBtn.SetEnabled(true)
									tableView.SetEnabled(true)
									updateUIStatus()

									if success {
										walk.MsgBox(mainWindow, "Success", "Files were successfully generated!", walk.MsgBoxIconInformation)
									} else {
										errMsg := "Error occurred while processing files!"
										if err != nil {
											errMsg += "\n\nDetails: " + err.Error()
										}
										walk.MsgBox(mainWindow, "Error", errMsg, walk.MsgBoxIconError)
									}
								})
							}()
						},
					},
				},
			},
		},
	}.Create()

	if err != nil {
		log.Fatalf("Failed to create MainWindow: %v", err)
	}

	model.owner = mainWindow

	updateUIStatus()
	mainWindow.Run()
}

func getModulesPath() string {
	execPath, err := os.Executable()
	if err == nil {
		execDir := filepath.Dir(execPath)
		mods, _ := LoadModules(execDir)
		if len(mods) > 0 {
			return execDir
		}
	}

	devDir := `d:\RFC Modular\rfc`
	if _, err := os.Stat(devDir); err == nil {
		mods, _ := LoadModules(devDir)
		if len(mods) > 0 {
			return devDir
		}
	}

	cwd, err := os.Getwd()
	if err == nil {
		return cwd
	}

	return ""
}

func CopyDir(src string, dst string) error {
	return filepath.Walk(src, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		rel, err := filepath.Rel(src, path)
		if err != nil {
			return err
		}

		target := filepath.Join(dst, rel)
		if info.IsDir() {
			return os.MkdirAll(target, info.Mode())
		}

		srcFile, err := os.Open(path)
		if err != nil {
			return err
		}
		defer srcFile.Close()

		dstFile, err := os.OpenFile(target, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, info.Mode())
		if err != nil {
			return err
		}
		defer dstFile.Close()

		_, err = io.Copy(dstFile, srcFile)
		return err
	})
}

func handleGenerate(checkedModules []*RFCModule, outputDir string, logs io.Writer) (bool, error) {
	tmpPath, err := os.MkdirTemp("", "rfc-generator")
	if err != nil {
		return false, fmt.Errorf("failed to create temp dir: %w", err)
	}
	defer os.RemoveAll(tmpPath)

	fmt.Fprintf(logs, "Copying files to temp directory: %s\n", tmpPath)
	for _, m := range checkedModules {
		folderName := filepath.Base(m.Dir)
		targetDir := filepath.Join(tmpPath, folderName)
		err := os.MkdirAll(targetDir, 0755)
		if err != nil {
			return false, fmt.Errorf("failed to create directory: %w", err)
		}

		err = CopyDir(m.Dir, targetDir)
		if err != nil {
			return false, fmt.Errorf("failed to copy module %s: %w", m.Name, err)
		}
	}

	fmt.Fprintln(logs, "\nStarting amalgamation...")
	success, err := MakeAmalgamatedRFC(tmpPath, "rfc", outputDir, logs)
	if err != nil {
		return false, fmt.Errorf("amalgamation failed: %w", err)
	}

	return success, nil
}
