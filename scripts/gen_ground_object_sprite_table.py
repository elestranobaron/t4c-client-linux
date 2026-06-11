#!/usr/bin/env python3
"""Regenerate src/game/T4CGroundObjectSpriteTable.inc from Windows VisualObjectList.cpp."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VOL = ROOT / "client/T4C Client/VisualObjectList.cpp"
APP = ROOT / "client/T4C Client/Apparence.h"
OUT = ROOT / "src/game/T4CGroundObjectSpriteTable.inc"

START_MARK = "//static summable items"
END_SENTINEL = "pVObjectAttack["

PATTERNS = [
    re.compile(r'GetVSFObject\s*\(\s*(\d+)\s*\)->CreateSprite\s*\(\s*"([^"]+)"'),
    re.compile(r'pVObject\s*\[\s*([^\]]+)\s*\]\.CreateSprite\s*\(\s*"([^"]+)"'),
]


def load_objgroup_constants(path: Path) -> dict[str, int]:
    consts: dict[str, int] = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.match(r"const unsigned int (__\w+)\s*=\s*(\d+)", line)
        if m:
            consts[m.group(1)] = int(m.group(2))
    return consts


def strip_comments(line: str) -> str | None:
    if re.match(r"^\s*//", line):
        return None
    return re.sub(r"//.*$", "", line)


def eval_index(expr: str, consts: dict[str, int]) -> int | None:
    expr = expr.strip()
    if expr.isdigit():
        return int(expr)
    m = re.match(r"(__\w+)(?:\s*-\s*(\d+))?(?:\s*\+\s*(\d+))?", expr)
    if m and m.group(1) in consts:
        base = consts[m.group(1)]
        if m.group(2):
            base -= int(m.group(2))
        if m.group(3):
            base += int(m.group(3))
        return base
    return None


def main() -> int:
    if not VOL.is_file() or not APP.is_file():
        print(f"Missing source: {VOL} or {APP}", file=sys.stderr)
        return 1

    consts = load_objgroup_constants(APP)
    text = VOL.read_text(encoding="utf-8", errors="replace")
    start = text.index(START_MARK)
    block_start = text.index("GetVSFObject(  0)", start)
    end = text.index(END_SENTINEL, block_start)
    block = text[block_start:end]

    rows: dict[int, str] = {}
    for raw in block.splitlines():
        line = strip_comments(raw)
        if not line or "CreateSprite" not in line:
            continue
        for pat in PATTERNS:
            m = pat.search(line)
            if not m:
                continue
            idx = eval_index(m.group(1), consts)
            if idx is None:
                continue
            rows[idx] = m.group(2)
            break

    sorted_rows = sorted(rows.items())
    lines = [
        "// Auto-generated — do not edit; run scripts/gen_ground_object_sprite_table.py",
        f"// Source: {VOL.name} [{START_MARK} .. {END_SENTINEL}]",
        f"// Rows: {len(sorted_rows)}",
    ]
    for idx, sprite in sorted_rows:
        lines.append(f'    {{{idx}, "{sprite}"}},')
    OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")

    print(f"Wrote {len(sorted_rows)} rows to {OUT}")
    missing_doors = [i for i in range(15, 27) if i not in rows]
    if missing_doors:
        print(f"WARN: missing door objgroups: {missing_doors}", file=sys.stderr)
    if 131 in rows or 132 in rows:
        print("WARN: rows 131/132 present — check commented source lines", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
