#!/usr/bin/env python3
"""Regenerate src/game/T4CInvItemSpriteTable.inc from VisualObjectList.cpp InvItemIcons bindings."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VOL = ROOT / "client/T4C Client/VisualObjectList.cpp"
APP = ROOT / "client/T4C Client/Apparence.h"
OUT = ROOT / "src/game/T4CInvItemSpriteTable.inc"

START_MARK = "#define BIND_INV"
END_MARK = "ProfessionIcons.BindSprite"


def load_objgroup_constants(path: Path) -> dict[str, int]:
    consts: dict[str, int] = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.match(r"const unsigned int (__\w+)\s*=\s*(\d+)", line)
        if m:
            consts[m.group(1)] = int(m.group(2))
    return consts


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
    end = text.index(END_MARK, start)
    block = text[start:end]

    rows: dict[int, str] = {}

    bind_inv = re.compile(r'BIND_INV(?:_PAL)?\s*\(\s*([^,]+)\s*,\s*"([^"]+)"')
    bind_sprite = re.compile(
        r'InvItemIcons\.BindSprite\s*\(\s*"([^"]+)"\s*,\s*([^,\)]+)(?:\s*,\s*\d+\s*)?\)'
    )

    for raw in block.splitlines():
        line = re.sub(r"//.*$", "", raw)
        m = bind_inv.search(line)
        if m:
            idx = eval_index(m.group(1), consts)
            if idx is not None:
                rows[idx] = m.group(2)
            continue
        m = bind_sprite.search(line)
        if m:
            idx = eval_index(m.group(2), consts)
            if idx is not None:
                rows[idx] = m.group(1)

    sorted_rows = sorted(rows.items())
    lines = [
        "// Auto-generated — do not edit; run scripts/gen_inv_item_sprite_table.py",
        f"// Source: {VOL.name} InvItemIcons BIND_INV / BindSprite",
        f"// Rows: {len(sorted_rows)}",
    ]
    for idx, sprite in sorted_rows:
        lines.append(f'    {{{idx}, "{sprite}"}},')
    OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {len(sorted_rows)} rows to {OUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
