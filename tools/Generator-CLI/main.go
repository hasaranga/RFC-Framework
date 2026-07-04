package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

func reorderArgs(args []string) []string {
	if len(args) <= 1 {
		return args
	}

	// Set of flags that require an argument
	hasValue := map[string]bool{
		"-rfc-dir": true, "-r": true,
		"-out-dir": true, "-o": true,
		"-name":    true, "-n": true,
		"-modules": true, "-m": true,
	}

	var flags []string
	var pos []string

	i := 1
	for i < len(args) {
		arg := args[i]
		if strings.HasPrefix(arg, "-") {
			// Check if it's in the format -flag=value
			if strings.Contains(arg, "=") {
				flags = append(flags, arg)
				i++
				continue
			}

			if hasValue[arg] {
				if i+1 < len(args) {
					flags = append(flags, arg, args[i+1])
					i += 2
				} else {
					flags = append(flags, arg)
					i++
				}
			} else {
				flags = append(flags, arg)
				i++
			}
		} else {
			pos = append(pos, arg)
			i++
		}
	}

	result := []string{args[0]}
	result = append(result, flags...)
	result = append(result, pos...)
	return result
}

func main() {
	// Reorder os.Args so that flags can be specified anywhere (e.g. after positional arguments)
	os.Args = reorderArgs(os.Args)

	var rfcPath string
	flag.StringVar(&rfcPath, "rfc-dir", "", "Path to the RFC root directory containing module folders")
	flag.StringVar(&rfcPath, "r", "", "Path to the RFC root directory (shorthand)")

	var outputPath string
	flag.StringVar(&outputPath, "out-dir", ".", "Directory to output the amalgamated files")
	flag.StringVar(&outputPath, "o", ".", "Directory to output the amalgamated files (shorthand)")

	var baseName string
	flag.StringVar(&baseName, "name", "rfc", "Base name of the amalgamated files (generates <name>.h and <name>.cpp)")
	flag.StringVar(&baseName, "n", "rfc", "Base name of the amalgamated files (shorthand)")

	var listModules bool
	flag.BoolVar(&listModules, "list", false, "List all available modules and their descriptions")
	flag.BoolVar(&listModules, "l", false, "List all available modules (shorthand)")

	var modulesList string
	flag.StringVar(&modulesList, "modules", "", "Comma-separated list of modules to include")
	flag.StringVar(&modulesList, "m", "", "Comma-separated list of modules to include (shorthand)")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "Generator-CLI - A tool to combine selected RFC modules into a single header and C++ source pair.\n\n")
		fmt.Fprintf(os.Stderr, "Usage:\n")
		fmt.Fprintf(os.Stderr, "  Generator-CLI [options] [modules...]\n\n")
		fmt.Fprintf(os.Stderr, "Examples:\n")
		fmt.Fprintf(os.Stderr, "  Generator-CLI gui file thread\n")
		fmt.Fprintf(os.Stderr, "  Generator-CLI -m gui,file,thread -o ./output\n")
		fmt.Fprintf(os.Stderr, "  Generator-CLI --rfc-dir C:\\agent\\rfc_test\\rfc -o build -n rfc_amalgamated\n\n")
		fmt.Fprintf(os.Stderr, "Options:\n")
		flag.PrintDefaults()
	}

	flag.Parse()

	// 1. Resolve RFC directory path
	if rfcPath == "" {
		rfcPath = detectRFCPath()
		if rfcPath == "" {
			fmt.Fprintln(os.Stderr, "Error: Could not locate RFC directory. Please specify it using --rfc-dir / -r option.")
			flag.Usage()
			os.Exit(1)
		}
	} else {
		// Clean the path
		rfcPath = filepath.Clean(rfcPath)
		if !isRFCFolder(rfcPath) {
			// If they specified the rfc dir but it doesn't contain core/module.xml, check if there's a child "rfc" folder
			childRFC := filepath.Join(rfcPath, "rfc")
			if isRFCFolder(childRFC) {
				rfcPath = childRFC
			} else {
				fmt.Fprintf(os.Stderr, "Warning: The path %s does not appear to be a valid RFC directory (core/module.xml not found).\n", rfcPath)
			}
		}
	}

	// 2. Load available modules
	modules, err := LoadModules(rfcPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error loading modules from %s: %v\n", rfcPath, err)
		os.Exit(1)
	}

	// Build maps for quick lookup and description printing
	moduleMap := make(map[string]*RFCModule)
	lowerModuleMap := make(map[string]*RFCModule)
	for _, m := range modules {
		moduleMap[m.Name] = m
		lowerModuleMap[strings.ToLower(m.Name)] = m
	}

	// 3. Handle list option
	if listModules {
		fmt.Printf("Available modules in %s:\n\n", rfcPath)
		for _, m := range modules {
			fixedStr := ""
			if m.IsFixed {
				fixedStr = " [fixed/always-on]"
			}
			fmt.Printf("- %s%s\n", m.Name, fixedStr)
			if m.Description != "" {
				fmt.Printf("  Description: %s\n", m.Description)
			}
			if len(m.AllDependencies) > 0 {
				fmt.Printf("  Dependencies: %s\n", strings.Join(m.AllDependencies, ", "))
			}
			if m.Platform != "" {
				fmt.Printf("  OS Support:   %s\n", m.Platform)
			}
			fmt.Println()
		}
		os.Exit(0)
	}

	// 4. Determine requested modules (from both command-line flag and positional args)
	requestedMap := make(map[string]bool)

	// A. Check fixed modules (e.g. Core) by default
	for _, m := range modules {
		if m.IsFixed {
			requestedMap[strings.ToLower(m.Name)] = true
		}
	}

	// B. Parse --modules/-m option
	if modulesList != "" {
		parts := strings.Split(modulesList, ",")
		for _, p := range parts {
			name := strings.TrimSpace(p)
			if name != "" {
				requestedMap[strings.ToLower(name)] = true
			}
		}
	}

	// C. Parse positional arguments
	for _, arg := range flag.Args() {
		name := strings.TrimSpace(arg)
		if name != "" {
			requestedMap[strings.ToLower(name)] = true
		}
	}

	// Validate all requested modules and check them
	var invalidModules []string
	for reqName := range requestedMap {
		m, ok := lowerModuleMap[reqName]
		if !ok {
			invalidModules = append(invalidModules, reqName)
		} else {
			m.Checked = true
		}
	}

	if len(invalidModules) > 0 {
		fmt.Fprintf(os.Stderr, "Error: The following requested modules do not exist: %s\n\n", strings.Join(invalidModules, ", "))
		fmt.Fprintln(os.Stderr, "Valid module names are:")
		for _, m := range modules {
			fmt.Fprintf(os.Stderr, "  - %s\n", m.Name)
		}
		os.Exit(1)
	}

	// 5. Automatically include recursive dependencies for all checked modules
	for _, m := range modules {
		if m.Checked {
			for _, depName := range m.AllDependencies {
				if depMod, ok := moduleMap[depName]; ok {
					depMod.Checked = true
				}
			}
		}
	}

	// 6. Double check dependencies consistency
	var checkedModules []*RFCModule
	var checkedNames []string
	for _, m := range modules {
		if m.Checked {
			checkedModules = append(checkedModules, m)
			checkedNames = append(checkedNames, m.Name)
		}
	}

	depIssue := CheckForMissingDeps(checkedModules)
	if depIssue != "" {
		fmt.Fprintf(os.Stderr, "Error: Dependency issue detected: %s\n", depIssue)
		os.Exit(1)
	}

	// Print summary of what we are doing
	fmt.Printf("RFC directory:   %s\n", rfcPath)
	fmt.Printf("Output directory:%s\n", outputPath)
	fmt.Printf("Output files:    %s.h and %s.cpp\n", baseName, baseName)
	fmt.Printf("Selected modules:%s\n\n", strings.Join(checkedNames, ", "))

	// Create output folder if it doesn't exist
	err = os.MkdirAll(outputPath, 0755)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: Failed to create output directory %s: %v\n", outputPath, err)
		os.Exit(1)
	}

	// 7. Amalgamate
	success, err := runAmalgamation(checkedModules, baseName, outputPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error during amalgamation: %v\n", err)
		os.Exit(1)
	}

	if success {
		fmt.Println("Success: Amalgamated files were successfully generated!")
	} else {
		fmt.Fprintln(os.Stderr, "Error: Generation finished with errors.")
		os.Exit(1)
	}
}

func detectRFCPath() string {
	// 1. Check if "rfc" exists in current working directory
	cwd, err := os.Getwd()
	if err == nil {
		path := filepath.Join(cwd, "rfc")
		if isRFCFolder(path) {
			return path
		}
		// Try current directory directly (in case they are inside the rfc folder)
		if isRFCFolder(cwd) {
			return cwd
		}
		
		// 2. Try parent directories up to 5 levels
		dir := cwd
		for i := 0; i < 5; i++ {
			parent := filepath.Dir(dir)
			if parent == dir { // root reached
				break
			}
			path = filepath.Join(parent, "rfc")
			if isRFCFolder(path) {
				return path
			}
			dir = parent
		}
	}

	// 3. Try executable directory
	execPath, err := os.Executable()
	if err == nil {
		execDir := filepath.Dir(execPath)
		path := filepath.Join(execDir, "rfc")
		if isRFCFolder(path) {
			return path
		}
		// Walk up from execDir as well
		dir := execDir
		for i := 0; i < 5; i++ {
			parent := filepath.Dir(dir)
			if parent == dir {
				break
			}
			path = filepath.Join(parent, "rfc")
			if isRFCFolder(path) {
				return path
			}
			dir = parent
		}
	}

	return ""
}

func isRFCFolder(path string) bool {
	// A valid RFC folder should have a "core" subdirectory containing "module.xml"
	coreXml := filepath.Join(path, "core", "module.xml")
	_, err := os.Stat(coreXml)
	return err == nil
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

func runAmalgamation(checkedModules []*RFCModule, outputName string, outputDir string) (bool, error) {
	tmpPath, err := os.MkdirTemp("", "rfc-generator-cli")
	if err != nil {
		return false, fmt.Errorf("failed to create temp dir: %w", err)
	}
	defer os.RemoveAll(tmpPath)

	fmt.Printf("Copying files to temp directory: %s\n", tmpPath)
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

	fmt.Println("Starting amalgamation...")
	success, err := MakeAmalgamatedRFC(tmpPath, outputName, outputDir, os.Stdout)
	if err != nil {
		return false, fmt.Errorf("amalgamation failed: %w", err)
	}

	return success, nil
}
