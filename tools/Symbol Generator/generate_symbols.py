#!/usr/bin/env python3
"""
generate_symbols.py

Converts universal-ctags JSON Lines output into a minimal symbols.md:
just construct name -> file path (+ inheritance, + namespace if not global).
No member lists, no line numbers. Pure "what exists and where" index.

Rationale: in practice, agents use symbols.md only to resolve "which file
has this construct" and then read the real file for everything else
(exact signatures, members, implementation). The member list was dead
weight; this trims symbols.md down to the part that's actually used.

Usage:
    python generate_symbols.py <symbols.json> <symbols.md> <project_root>

    project_root: path to the framework folder (absolute or relative to cwd).
                  All file paths in symbols.md will be relative to this folder.
                  Defaults to cwd if omitted.
"""

import sys
import json
import os
from collections import defaultdict

# Constructs we want to list. Covers classes, structs, unions, enums,
# typedefs, macros (#define), and free functions.
CONTAINER_KINDS = {"class", "struct", "union"}
OTHER_KINDS = {"enum", "typedef", "macro", "function"}
KEEP_KINDS = CONTAINER_KINDS | OTHER_KINDS

KIND_LABEL = {
    "class": "Class",
    "struct": "Struct",
    "union": "Union",
    "enum": "Enum",
    "typedef": "Typedef",
    "macro": "Macro",
    "function": "Function",
}


def load_symbols(json_path):
    """Read ctags JSON Lines output, skipping non-symbol control lines."""
    symbols = []
    with open(json_path, "r", encoding="utf-8-sig") as f:
        for line_no, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            try:
                obj = json.loads(line)
            except json.JSONDecodeError:
                print(f"WARN: skipping unparsable line {line_no}", file=sys.stderr)
                continue
            if obj.get("_type") != "tag":
                continue
            symbols.append(obj)
    return symbols


def normalize_path(path, project_root):
    """Make file paths relative to the PARENT of project_root, with forward
    slashes. This preserves the rfc/ folder prefix in symbols.md so agents
    running from the parent directory can open files directly.
    e.g. project_root=C:/rfc_v1.2/rfc -> paths become rfc/containers/KFoo.h"""
    path = path.replace("\\", "/")
    parent = os.path.dirname(os.path.abspath(project_root))
    try:
        abs_path = os.path.abspath(path).replace("\\", "/")
        rel = os.path.relpath(abs_path, parent).replace("\\", "/")
        # Guard against paths escaping the project root
        if not rel.startswith(".."):
            return rel
    except ValueError:
        # Different drive on Windows — relpath can't compute
        pass
    return path


def build_entries(symbols, project_root):
    """
    Collect one entry per top-level construct we care about:
    class/struct/union/enum/typedef/macro/function that is NOT a member
    of a class (i.e. scopeKind is not class/struct/union).

    Each entry: (path, namespace, kind, name, inherits)
    """
    entries = []

    for sym in symbols:
        kind = sym.get("kind", "")
        if kind not in KEEP_KINDS:
            continue

        scope_kind = sym.get("scopeKind", "")
        # Skip members of a class/struct/union — we only want top-level
        # constructs, not their methods/fields.
        if scope_kind in ("class", "struct", "union"):
            continue

        path = normalize_path(sym.get("path", "unknown"), project_root)
        name = sym.get("name", "<unknown>")
        namespace = sym.get("scope", "") if scope_kind == "namespace" else ""
        inherits = sym.get("inherits", "") if kind in CONTAINER_KINDS else ""

        entries.append({
            "path": path,
            "namespace": namespace,
            "kind": kind,
            "name": name,
            "inherits": inherits,
        })

    return entries


def render_spec(entries, out_path, project_root):
    # Sort all entries globally: namespace first, then name
    all_entries = sorted(entries, key=lambda e: (e["namespace"], e["name"]))

    # de-dupe (e.g. forward declaration + definition both tagged)
    seen = set()
    deduped = []
    for e in all_entries:
        key = (e["namespace"], e["kind"], e["name"], e["inherits"], e["path"])
        if key in seen:
            continue
        seen.add(key)
        deduped.append(e)

    lines = []
    lines.append("# Framework Skeleton Spec (Minimal)")
    lines.append("")
    lines.append(
        "Flat navigation index: one line per construct with its file path. "
        "No member lists — read the source file for exact signatures, "
        "members, and implementation. See AGENTS.md for usage rules. "
        "paths are relative to current directory. "
        "if the current dir is `c:/myprojects` then the real path of `rfc/gui/KComponent.h` file is `c:/myprojects/rfc/gui/KComponent.h`"
    )
    lines.append("")

    for e in deduped:
        label = KIND_LABEL.get(e["kind"], e["kind"].capitalize())
        ns = e["namespace"]
        name = e["name"]
        path = e["path"]

        entry_line = f"- **{label}**: `{name}`"
        if ns:
            entry_line += f" *(ns: {ns})*"
        if e["inherits"]:
            entry_line += f" (Inherits: `{e['inherits']}`)"
        entry_line += f" — `{path}`"
        lines.append(entry_line)

    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))


def main():
    if len(sys.argv) not in (3, 4):
        print(
            "Usage: python generate_symbols.py <symbols.json> <symbols.md> [project_root]",
            file=sys.stderr,
        )
        sys.exit(1)

    json_path = sys.argv[1]
    out_path = sys.argv[2]
    project_root = sys.argv[3] if len(sys.argv) == 4 else os.getcwd()

    if not os.path.isfile(json_path):
        print(f"ERROR: {json_path} not found", file=sys.stderr)
        sys.exit(1)

    if not os.path.isdir(project_root):
        print(f"ERROR: project_root '{project_root}' is not a directory", file=sys.stderr)
        sys.exit(1)

    project_root = os.path.abspath(project_root)
    print(f"Project root: {project_root}", file=sys.stderr)

    symbols = load_symbols(json_path)
    print(f"Loaded {len(symbols)} symbols from {json_path}", file=sys.stderr)

    entries = build_entries(symbols, project_root)
    render_spec(entries, out_path, project_root)

    print(f"Wrote {out_path} ({len(entries)} top-level constructs)", file=sys.stderr)


if __name__ == "__main__":
    main()
