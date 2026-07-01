package main

import (
	"bytes"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func TestIncludeRegex(t *testing.T) {
	tests := []struct {
		line    string
		matched bool
		path    string
	}{
		{`#include "file.h"`, true, `file.h`},
		{`#include "sub/file.h"`, true, `sub/file.h`},
		{`#include  "file.h"`, true, `file.h`},
		{`#include "file2.h" // comment`, true, `file2.h`},
		{`#include <vector>`, false, ``},
		{`int x = 0;`, false, ``},
	}

	for _, tt := range tests {
		matches := includeRegex.FindStringSubmatch(tt.line)
		if tt.matched {
			if len(matches) < 2 {
				t.Errorf("Expected line %q to match include, but got no match", tt.line)
			} else if matches[1] != tt.path {
				t.Errorf("Expected path %q, got %q for line %q", tt.path, matches[1], tt.line)
			}
		} else {
			if len(matches) > 1 {
				t.Errorf("Expected line %q to NOT match include, but got %q", tt.line, matches[1])
			}
		}
	}
}

func TestAmalgamationTopologicalSort(t *testing.T) {
	// Create a temp dir for mock headers
	tmpDir, err := os.MkdirTemp("", "amalgamator-test")
	if err != nil {
		t.Fatalf("Failed to create temp dir: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	// Write linear dependencies:
	// a.h has no includes
	// b.h includes "a.h"
	// c.h includes "b.h"
	aContent := "void funcA() {}"
	bContent := `#include "a.h"` + "\n" + "void funcB() { funcA(); }"
	cContent := `#include "b.h"` + "\n" + "void funcC() { funcB(); }"

	err = os.WriteFile(filepath.Join(tmpDir, "a.h"), []byte(aContent), 0644)
	if err != nil {
		t.Fatalf("Failed to write a.h: %v", err)
	}
	err = os.WriteFile(filepath.Join(tmpDir, "b.h"), []byte(bContent), 0644)
	if err != nil {
		t.Fatalf("Failed to write b.h: %v", err)
	}
	err = os.WriteFile(filepath.Join(tmpDir, "c.h"), []byte(cContent), 0644)
	if err != nil {
		t.Fatalf("Failed to write c.h: %v", err)
	}

	var logs bytes.Buffer
	outputDir, err := os.MkdirTemp("", "amalgamator-test-out")
	if err != nil {
		t.Fatalf("Failed to create output temp dir: %v", err)
	}
	defer os.RemoveAll(outputDir)

	lines, err := AmalgamateHeaderFiles(tmpDir, "rfc", outputDir, &logs)
	if err != nil {
		t.Fatalf("Amalgamation failed: %v\nLogs: %s", err, logs.String())
	}

	if lines <= 0 {
		t.Errorf("Expected positive line count, got %d", lines)
	}

	// Read generated header
	generatedPath := filepath.Join(outputDir, "rfc.h")
	contentBytes, err := os.ReadFile(generatedPath)
	if err != nil {
		t.Fatalf("Failed to read generated header: %v", err)
	}
	content := string(contentBytes)

	// Check that a.h is printed before b.h, and b.h before c.h
	idxA := strings.Index(content, "=========== a.h ===========")
	idxB := strings.Index(content, "=========== b.h ===========")
	idxC := strings.Index(content, "=========== c.h ===========")

	if idxA == -1 || idxB == -1 || idxC == -1 {
		t.Errorf("Missing file sections in generated header. Indexes: a:%d, b:%d, c:%d", idxA, idxB, idxC)
	}

	if idxA > idxB || idxB > idxC {
		t.Errorf("Incorrect topological sort order. Indexes: a:%d, b:%d, c:%d", idxA, idxB, idxC)
	}
}

func TestCircularDependencyDetection(t *testing.T) {
	tmpDir, err := os.MkdirTemp("", "amalgamator-test-circular")
	if err != nil {
		t.Fatalf("Failed to create temp dir: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	// Write circular dependencies:
	// a.h includes "b.h"
	// b.h includes "a.h"
	aContent := `#include "b.h"` + "\n" + "void funcA() {}"
	bContent := `#include "a.h"` + "\n" + "void funcB() {}"

	err = os.WriteFile(filepath.Join(tmpDir, "a.h"), []byte(aContent), 0644)
	if err != nil {
		t.Fatalf("Failed to write a.h: %v", err)
	}
	err = os.WriteFile(filepath.Join(tmpDir, "b.h"), []byte(bContent), 0644)
	if err != nil {
		t.Fatalf("Failed to write b.h: %v", err)
	}

	var logs bytes.Buffer
	outputDir, err := os.MkdirTemp("", "amalgamator-test-circular-out")
	if err != nil {
		t.Fatalf("Failed to create output temp dir: %v", err)
	}
	defer os.RemoveAll(outputDir)

	_, err = AmalgamateHeaderFiles(tmpDir, "rfc", outputDir, &logs)
	if err == nil {
		t.Fatal("Expected error due to circular dependencies, but got success")
	}

	if !strings.Contains(err.Error(), "circular dependency") {
		t.Errorf("Expected circular dependency error message, got: %v", err)
	}
}
