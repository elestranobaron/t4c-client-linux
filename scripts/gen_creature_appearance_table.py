#!/usr/bin/env python3
"""Generate appearance(Type) -> sprite-name table for creatures/NPCs.

Joins three sources from the Windows client:
  1. ObjectListing.h          : __NAME -> numeric appearance value
  2. VisualObjectList.cpp      : `case __NAME: Set = N;`  (Type -> VObject3D index)
  3. VisualObjectList.cpp      : `case N: ... LoadSprite3D(..., "Sprite", ...)` (Set -> sprite name)

Output: src/network/T4CCreatureAppearanceTable.inc
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OBJLIST = ROOT / "client/T4C Client/ObjectListing.h"
VLIST = ROOT / "client/T4C Client/VisualObjectList.cpp"
OUT = ROOT / "src/network/T4CCreatureAppearanceTable.inc"

def parse_object_listing(text):
    names = {}
    for m in re.finditer(r'const\s+unsigned\s+short\s+(__\w+)\s*=\s*(\d+)\s*;', text):
        names[m.group(1)] = int(m.group(2))
    return names

def parse_type_to_set(text):
    # `case __NAME: Set = N; break;`  (whitespace flexible)
    mapping = {}
    for m in re.finditer(r'case\s+(__\w+)\s*:\s*Set\s*=\s*(\d+)\s*;', text):
        mapping[m.group(1)] = int(m.group(2))
    return mapping

def parse_set_to_sprite(text):
    # Find each `case N:` in LoadObject and the first LoadSprite3D/2 string after it.
    mapping = {}
    # Collect (pos, setIndex) for numeric case labels
    cases = [(m.start(), int(m.group(1))) for m in re.finditer(r'\bcase\s+(\d+)\s*:', text)]
    for i, (pos, setidx) in enumerate(cases):
        end = cases[i + 1][0] if i + 1 < len(cases) else len(text)
        block = text[pos:end]
        sm = re.search(r'LoadSprite3D2?\s*\([^;]*?"([^"]+)"', block)
        if sm and setidx not in mapping:
            mapping[setidx] = sm.group(1)
    return mapping

def main():
    objtext = OBJLIST.read_text(encoding="utf-8", errors="ignore")
    vtext = VLIST.read_text(encoding="utf-8", errors="ignore")

    names = parse_object_listing(objtext)
    type_to_set = parse_type_to_set(vtext)
    set_to_sprite = parse_set_to_sprite(vtext)

    entries = {}  # value -> sprite
    for name, setidx in type_to_set.items():
        if name not in names:
            continue
        value = names[name]
        sprite = set_to_sprite.get(setidx)
        if not sprite:
            continue
        entries[value] = sprite

    rows = sorted(entries.items())
    lines = []
    lines.append("// Auto-genere par scripts/gen_creature_appearance_table.py — NE PAS EDITER.")
    lines.append("// appearance (Type serveur) -> nom de sprite de base (sprite 3D).")
    lines.append("struct T4CCreatureSprite { std::uint16_t appearance; const char *sprite; };")
    lines.append("static constexpr T4CCreatureSprite kCreatureSpriteTable[] = {")
    for value, sprite in rows:
        lines.append('    {%5d, "%s"},' % (value, sprite))
    lines.append("};")
    OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")

    print("ObjectListing names :", len(names))
    print("Type->Set mappings  :", len(type_to_set))
    print("Set->Sprite mappings:", len(set_to_sprite))
    print("Composed entries    :", len(entries))
    print("Wrote", OUT)
    # Spot checks
    for v in (20006, 20001, 20003, 20008, 20012, 10005, 10006):
        print("  check", v, "->", entries.get(v))

if __name__ == "__main__":
    sys.exit(main())
