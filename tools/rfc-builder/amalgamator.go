package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
	"time"
)

var includeRegex = regexp.MustCompile(`#include\s*"([^"]+)"`)

// HeaderFile holds the representation of a parsed C++ header file
type HeaderFile struct {
	FilePath string   // complete path
	FileBuff string   // file content without local includes
	IncList  []string // resolved absolute paths of local includes
}

// ReadHeaderFile parses a header file and extracts its local includes
func ReadHeaderFile(path string, logger io.Writer) (*HeaderFile, error) {
	fmt.Fprintln(logger, "Processing: "+path)

	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	hf := &HeaderFile{
		FilePath: path,
		IncList:  []string{},
	}

	scanner := bufio.NewScanner(file)
	curDir := filepath.Dir(path)

	for scanner.Scan() {
		line := scanner.Text()
		matches := includeRegex.FindStringSubmatch(line)
		if len(matches) > 1 {
			relPath := matches[1]
			// Resolve path and clean it up
			incFilePath := filepath.Clean(filepath.Join(curDir, relPath))

			// Check if file exists
			if _, err := os.Stat(incFilePath); err == nil {
				hf.IncList = append(hf.IncList, incFilePath)
			} else {
				fmt.Fprintln(logger, "Error: missing "+incFilePath)
				return nil, fmt.Errorf("missing header: %s", incFilePath)
			}
		} else {
			hf.FileBuff += "\r\n" + line
		}
	}

	if err := scanner.Err(); err != nil {
		return nil, err
	}

	return hf, nil
}

// SearchForHeaderFiles scans a directory for all .h files recursively
func SearchForHeaderFiles(sourceDir string, logger io.Writer) ([]*HeaderFile, error) {
	var headers []*HeaderFile

	err := filepath.Walk(sourceDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !info.IsDir() && filepath.Ext(path) == ".h" {
			hf, err := ReadHeaderFile(path, logger)
			if err != nil {
				return err
			}
			headers = append(headers, hf)
		}
		return nil
	})

	if err != nil {
		return nil, err
	}

	return headers, nil
}

// isRequiredHeadersWritten checks if all headers in incList are already written
func isRequiredHeadersWritten(incList []string, writtenFiles map[string]bool) bool {
	for _, inc := range incList {
		if !writtenFiles[inc] {
			return false
		}
	}
	return true
}

// AmalgamateHeaderFiles performs dependency-ordered amalgamation on header files
func AmalgamateHeaderFiles(frameworkPath string, frameworkName string, outputDir string, logger io.Writer) (int, error) {
	headers, err := SearchForHeaderFiles(frameworkPath, logger)
	if err != nil {
		return 0, err
	}

	if len(headers) == 0 {
		return 0, fmt.Errorf("no header files found in %s", frameworkPath)
	}

	// Sort headers ascending by the length of their dependencies
	sort.Slice(headers, func(i, j int) bool {
		return len(headers[i].IncList) < len(headers[j].IncList)
	})

	// Format timestamp in yyyy-MM-dd hh:mm tt format (e.g. 2026-06-19 08:52 AM)
	timestamp := time.Now().Format("2006-01-02 03:04 PM")
	outHeaderFile := "\r\n// ========== RFC Generator v1.0 - " + timestamp + " ==========\r\n"

	guardName := "_" + strings.ToUpper(strings.ReplaceAll(frameworkName, " ", "_"))
	outHeaderFile += "\r\n#ifndef " + guardName + "_H_\r\n#define " + guardName + "_H_ \r\n\r\n#define AMALGAMATED_VERSION\r\n"

	writtenMap := make(map[string]bool)
	var writtenList []*HeaderFile
	var remaining []*HeaderFile

	// 1. Process 0-dependency headers
	for _, hf := range headers {
		if len(hf.IncList) == 0 {
			fmt.Fprintln(logger, "Adding: "+hf.FilePath)
			outHeaderFile += "\r\n\r\n// =========== " + filepath.Base(hf.FilePath) + " ===========" + hf.FileBuff
			writtenMap[hf.FilePath] = true
			writtenList = append(writtenList, hf)
		} else {
			remaining = append(remaining, hf)
		}
	}

	if len(writtenList) == 0 {
		fmt.Fprintln(logger, "\nError: Circular dependency detected (no base files with 0 includes)!")
		return 0, fmt.Errorf("circular dependency detected (no files with 0 includes)")
	}

	// 2. Process remaining headers with dependency ordering
	for len(remaining) > 0 {
		progress := false
		var nextRemaining []*HeaderFile

		for _, hf := range remaining {
			if len(hf.IncList) <= len(writtenMap) && isRequiredHeadersWritten(hf.IncList, writtenMap) {
				fmt.Fprintln(logger, "Adding: "+hf.FilePath)
				outHeaderFile += "\r\n\r\n// =========== " + filepath.Base(hf.FilePath) + " ===========" + hf.FileBuff
				writtenMap[hf.FilePath] = true
				progress = true
			} else {
				nextRemaining = append(nextRemaining, hf)
			}
		}

		if !progress {
			fmt.Fprintln(logger, "\nError: Circular dependency detected among remaining headers!")
			return 0, fmt.Errorf("circular dependency detected among remaining headers")
		}
		remaining = nextRemaining
	}

	outHeaderFile += "\r\n\r\n#endif\r\n\r\n"

	// Count total lines in the amalgamated buffer
	lineCount := strings.Count(outHeaderFile, "\n") + 1

	fmt.Fprintf(logger, "\nWriting: %s.h lines: %d\n", frameworkName, lineCount)
	
	outPath := filepath.Join(outputDir, frameworkName+".h")
	err = os.WriteFile(outPath, []byte(outHeaderFile), 0644)
	if err != nil {
		return 0, err
	}

	return lineCount, nil
}

// AmalgamateSourceFiles concatenates source files skipping local header includes
func AmalgamateSourceFiles(frameworkPath string, frameworkName string, outputDir string, logger io.Writer) error {
	var cppFiles []string
	err := filepath.Walk(frameworkPath, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !info.IsDir() && filepath.Ext(path) == ".cpp" {
			cppFiles = append(cppFiles, path)
		}
		return nil
	})
	if err != nil {
		return err
	}

	outSourceFile := "\r\n// ========== RFC Generator v1.0 ==========\r\n"
	outSourceFile += "\r\n\r\n#include \"" + frameworkName + ".h\"\r\n"

	var lineCount int
	for _, cppFile := range cppFiles {
		fmt.Fprintln(logger, "Adding: "+cppFile)
		outSourceFile += "\r\n\r\n// =========== " + filepath.Base(cppFile) + " ==========="

		file, err := os.Open(cppFile)
		if err != nil {
			return err
		}

		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			line := scanner.Text()
			lineCount++
			if !includeRegex.MatchString(line) {
				outSourceFile += "\r\n" + line
			}
		}
		file.Close()
		if err := scanner.Err(); err != nil {
			return err
		}
	}

	fmt.Fprintf(logger, "\nWriting: %s.cpp lines: %d\n\n", frameworkName, lineCount)

	outPath := filepath.Join(outputDir, frameworkName+".cpp")
	return os.WriteFile(outPath, []byte(outSourceFile), 0644)
}

// MakeAmalgamatedRFC is the entry point for both header and source amalgamation
func MakeAmalgamatedRFC(frameworkPath string, outputName string, outputDir string, logger io.Writer) (bool, error) {
	_, err := AmalgamateHeaderFiles(frameworkPath, outputName, outputDir, logger)
	if err != nil {
		return false, err
	}

	err = AmalgamateSourceFiles(frameworkPath, outputName, outputDir, logger)
	if err != nil {
		return false, err
	}

	return true, nil
}
