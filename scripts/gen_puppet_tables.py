#!/usr/bin/env python3
"""Generate puppet tables from Windows client sources (Puppet.cpp + VisualObjectList.cpp).

Outputs (do not edit by hand):
  src/game/T4CPuppetResolve.inc   — SetPuppet slot×PUPEQ → sprite (+ variant, hidden)
  src/game/T4CPuppetize.inc       — Puppetize OBJGROUP → slot×PUPEQ
  src/game/T4CPuppetBodyOrder.inc — BodyOrderR standing draw order

Regenerate: python3 scripts/gen_puppet_tables.py
"""
from __future__ import annotations

import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PUPPET_CPP = ROOT / "client/T4C Client/Puppet.cpp"
PUPPET_H = ROOT / "client/T4C Client/Puppet.h"
APPARENCE_H = ROOT / "client/T4C Client/Apparence.h"
VLIST_CPP = ROOT / "client/T4C Client/VisualObjectList.cpp"
OUT_RESOLVE = ROOT / "src/game/T4CPuppetResolve.inc"
OUT_PUPPETIZE = ROOT / "src/game/T4CPuppetize.inc"
OUT_BODYORDER = ROOT / "src/game/T4CPuppetBodyOrder.inc"

SECTION_TO_SLOT = {
    "HAND LEFT": 0,
    "ARM LEFT": 1,
    "FOOT": 2,
    "LEGS": 3,
    "BODY": 4,
    "HEAD": 5,
    "HAND R": 6,
    "ARM R": 7,
    "WEAPON": 8,
    "Shield": 9,
    "BOOT": 10,
    "HAT": 11,
    "CAPE": 12,
    "BACKBODY": 13,
    "CAPE 2": 14,
    "HAIR": 15,
    "ROBE LEGS": 16,
}

PUP_CONST = {
    "PUP_HAND_LEFT": 0,
    "PUP_ARM_LEFT": 1,
    "PUP_FOOT": 2,
    "PUP_LEGS": 3,
    "PUP_BODY": 4,
    "PUP_HEAD": 5,
    "PUP_HAND_RIGHT": 6,
    "PUP_ARM_RIGHT": 7,
    "PUP_WEAPON": 8,
    "PUP_SHIELD": 9,
    "PUP_BOOT": 10,
    "PUP_HAT": 11,
    "PUP_CAPE": 12,
    "PUP_BACKBODY": 13,
    "PUP_CAPE_2": 14,
    "PUP_HAIR": 15,
    "PUP_ROBELEGS": 16,
    "PUP_MASK": 17,
    "PUP_WEAPON2": 18,
}

PUPPETIZE_FIELD = {
    "CAPE": "cape",
    "W_RIGHT": "weaponR",
    "HELM": "helm",
    "GLOVES": "gloves",
    "LEGS": "legs",
    "FEET": "feet",
    "W_LEFT": "weaponL",
    "BODY": "body",
    "HAIR": "hair",
}


def parse_defines(text: str, prefix: str) -> dict[str, int]:
    out: dict[str, int] = {}
    for m in re.finditer(rf"#define\s+({prefix}\w+)\s+(\d+)", text):
        out[m.group(1)] = int(m.group(2))
    return out


def parse_objgroups(text: str) -> dict[str, int]:
    out: dict[str, int] = {}
    for m in re.finditer(r"const\s+unsigned\s+int\s+(__OBJGROUP_\w+)\s*=\s*(\d+)\s*;", text):
        out[m.group(1)] = int(m.group(2))
    return out


def extract_set_puppet(text: str) -> str:
    start = text.find("void Puppet::SetPuppet")
    if start < 0:
        raise SystemExit("SetPuppet not found")
    end = text.find("Object->nbBodyPart = 19", start)
    if end < 0:
        raise SystemExit("nbBodyPart marker not found")
    return text[start:end]


def parse_load_sprite(line: str) -> tuple[str, int] | None:
    m = re.search(r'LoadSprite3D\s*\([^"]*"([^"]+)"', line)
    if not m:
        return None
    name = m.group(1)
    # optional trailing variant index (last numeric arg before closing paren)
    tail = line[m.end() :]
    vm = re.search(r",\s*(\d+)\s*\)\s*;", tail)
    variant = int(vm.group(1)) if vm else 0
    return name, variant


def split_sections(set_puppet: str) -> list[tuple[int, str]]:
    lines = set_puppet.splitlines()
    sections: list[tuple[int, str]] = []
    current_slot: int | None = None
    buf: list[str] = []

    def flush():
        nonlocal buf, current_slot
        if current_slot is not None and buf:
            sections.append((current_slot, "\n".join(buf)))
        buf = []

    for line in lines:
        cm = re.search(r"//\s*(HAND LEFT|ARM LEFT|FOOT|LEGS|BODY|HEAD|HAND R|ARM R|WEAPON|Shield|BOOT|HAT|CAPE|BACKBODY|CAPE 2|HAIR|ROBE LEGS)", line)
        if cm:
            flush()
            label = cm.group(1)
            if label == "HAND R":
                label = "HAND R"
            current_slot = SECTION_TO_SLOT.get(label)
            if current_slot is None and label == "ROBE LEGS":
                current_slot = 16
            buf = [line]
            continue
        if current_slot is not None:
            if line.strip().startswith("// MASK") or "Pow2[17]" in line and "VisiblePart" in line:
                # mask handled inside HAIR/others — skip separate for now
                pass
            buf.append(line)
            if re.search(r"^\s*//\s*(HAND|ARM|FOOT|LEGS|BODY|HEAD|WEAPON|Shield|BOOT|HAT|CAPE|BACK|ROBE|HAIR)", line) and cm is None:
                pass
    flush()
    # MASK (17) et WEAPON2 (18) — switch explicite sans commentaire de section
    for slot_idx in (17, 18):
        pat = rf"switch\s*\(\s*Object->PuppetInfo\[{slot_idx}\]\s*\)"
        m = re.search(pat, set_puppet)
        if m:
            sub = set_puppet[m.start() : m.start() + 8000]
            sections.append((slot_idx, sub))
    return sections


def parse_case_blocks(section_text: str) -> list[tuple[list[int], str]]:
    """Return list of (pupeq_ids, block_body)."""
    blocks: list[tuple[list[int], str]] = []
    lines = section_text.splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if re.match(r"\s*case\s+", line):
            labels: list[int] = []
            body: list[str] = []
            while i < len(lines):
                line = lines[i]
                cm = re.match(r"\s*case\s+(\d+)\s*:", line)
                cpm = re.match(r"\s*case\s+(PUPEQ_\w+)\s*:", line)
                if cm:
                    labels.append(int(cm.group(1)))
                    i += 1
                    continue
                if cpm:
                    labels.append(cpm.group(1))  # type: ignore
                    i += 1
                    continue
                if re.match(r"\s*case\s+", line):
                    break
                if re.match(r"\s*default\s*:", line):
                    break
                body.append(line)
                if re.search(r"\bbreak\s*;", line):
                    i += 1
                    break
                i += 1
            blocks.append((labels, "\n".join(body)))
            continue
        i += 1
    return blocks


def resolve_pupeq_labels(labels: list, pupeq_map: dict[str, int]) -> list[int]:
    ids: list[int] = []
    for lb in labels:
        if isinstance(lb, int):
            ids.append(lb)
        else:
            ids.append(pupeq_map.get(lb, 0))
    return ids


def analyze_block(body: str) -> dict:
    hidden = bool(re.search(r"VisiblePart\s*-=\s*Pow2", body)) and "LoadSprite3D" not in body
    male: tuple[str, int] | None = None
    female: tuple[str, int] | None = None
    neutral: tuple[str, int] | None = None

    if hidden:
        return {"hidden": True, "male": None, "female": None, "neutral": None}

    if "Object->Type == 10011" in body:
        male_chunk = body.split("else")[0] if "else" in body else body
        female_chunk = body.split("else", 1)[1] if "else" in body else ""
        for line in male_chunk.splitlines():
            parsed = parse_load_sprite(line)
            if parsed:
                male = parsed
                break
        for line in female_chunk.splitlines():
            parsed = parse_load_sprite(line)
            if parsed:
                female = parsed
                break
    else:
        for line in body.splitlines():
            parsed = parse_load_sprite(line)
            if parsed:
                neutral = parsed
                break

    return {"hidden": False, "male": male, "female": female, "neutral": neutral}


def parse_set_puppet_resolve(set_puppet: str, pupeq_map: dict[str, int]) -> dict[tuple[int, int, int], dict]:
    """Key: (slot, pupeq, gender) gender: 0=male 1=female 2=neutral"""
    table: dict[tuple[int, int, int], dict] = {}
    for slot, section in split_sections(set_puppet):
        for labels, body in parse_case_blocks(section):
            pupeqs = resolve_pupeq_labels(labels, pupeq_map)
            info = analyze_block(body)
            for pq in pupeqs:
                if info["hidden"]:
                    for g in (0, 1, 2):
                        table[(slot, pq, g)] = {"hidden": True, "sprite": None, "variant": 0}
                    continue
                if info["neutral"]:
                    sp, var = info["neutral"]
                    for g in (0, 1):
                        table[(slot, pq, g)] = {"hidden": False, "sprite": sp, "variant": var}
                else:
                    if info["male"]:
                        sp, var = info["male"]
                        table[(slot, pq, 0)] = {"hidden": False, "sprite": sp, "variant": var}
                    if info["female"]:
                        sp, var = info["female"]
                        table[(slot, pq, 1)] = {"hidden": False, "sprite": sp, "variant": var}
    return table


def extract_puppetize(vlist: str, objgroups: dict[str, int], pupeq_map: dict[str, int]) -> list[tuple[str, int, int, int]]:
    """Returns (field, objgroup_value, slot, pupeq)."""
    marker = "void Puppetize( TFCObject *Object, WORD BODY, WORD FEET, WORD GLOVES, WORD HELM, WORD LEGS, WORD W_RIGHT,"
    start = vlist.rfind(marker)
    if start < 0:
        raise SystemExit("Puppetize not found")
    end = vlist.find("\nBOOL VisualObjectList::Found", start)
    if end < 0:
        end = start + 250000
    chunk = vlist[start:end]

    rows: list[tuple[str, int, int, int]] = []
    current_field: str | None = None
    current_obj: int | None = None

    for line in chunk.splitlines():
        sw = re.match(r"\s*switch\s*\(\s*(\w+)\s*\)", line)
        if sw and sw.group(1) in PUPPETIZE_FIELD:
            current_field = sw.group(1)
            current_obj = None
            continue
        cm = re.match(r"\s*case\s+(__OBJGROUP_\w+)\s*:", line)
        if cm and current_field:
            name = cm.group(1)
            current_obj = objgroups.get(name)
            continue
        if current_field and current_obj is not None:
            am = re.search(r"Object->PuppetInfo\[(\w+)\]\s*=\s*(PUPEQ_\w+)", line)
            if am:
                pup_slot = PUP_CONST.get(am.group(1))
                pupeq = pupeq_map.get(am.group(2))
                if pup_slot is not None and pupeq is not None:
                    rows.append((current_field, current_obj, pup_slot, pupeq))
            if re.match(r"\s*break\s*;", line):
                current_obj = None
    return rows


def extract_body_order_r(puppet: str) -> dict[int, list[int]]:
    """direction index -> [19 slots in draw order] (from [dir][layer][0])."""
    orders: dict[int, list[int]] = {}
    for m in re.finditer(
        r"BodyOrderR\[(\d+)\]\[(\d+)\]\[i\]\s*=\s*(PUP_\w+);", puppet
    ):
        direction = int(m.group(1))
        layer = int(m.group(2))
        pup = PUP_CONST.get(m.group(3))
        if pup is None:
            continue
        orders.setdefault(direction, [0] * 19)
        if layer < 19:
            orders[direction][layer] = pup
    return orders


def emit_resolve(table: dict[tuple[int, int, int], dict]) -> str:
    lines = [
        "// Auto-genere par scripts/gen_puppet_tables.py — NE PAS EDITER.",
        "#pragma once",
        "",
        "struct T4CPuppetResolveRow {",
        "    std::uint8_t slot;",
        "    std::uint16_t pupeq;",
        "    std::uint8_t female;  // 0=male 1=female",
        "    std::uint8_t hidden;",
        "    std::uint8_t variant;",
        "    const char *sprite;",
        "};",
        "",
    ]
    by_slot: dict[int, list[tuple[int, int, dict]]] = defaultdict(list)
    for (slot, pq, gender), info in sorted(table.items()):
        if gender > 1:
            continue
        by_slot[slot].append((pq, gender, info))

    all_rows: list[str] = []
    slot_offsets: list[tuple[int, int, int]] = []
    offset = 0
    for slot in range(19):
        entries = by_slot.get(slot, [])
        slot_offsets.append((slot, offset, len(entries)))
        for pq, gender, info in sorted(entries):
            sp = info.get("sprite")
            sprite_lit = "nullptr" if not sp else f'"{sp}"'
            hidden = 1 if info.get("hidden") else 0
            variant = info.get("variant", 0)
            all_rows.append(
                f"    {{ {slot}, {pq}, {gender}, {hidden}, {variant}, {sprite_lit} }},"
            )
        offset += len(entries)

    lines.append("static const T4CPuppetResolveRow kPuppetResolveRows[] = {")
    lines.extend(all_rows or ["    // empty"])
    lines.append("};")
    lines.append("")
    lines.append("static const int kPuppetResolveRowCount = "
                 f"{len(all_rows)};")
    lines.append("")
    lines.append("static const std::uint16_t kPuppetResolveSlotOffset[19] = {")
    off_map = {s: o for s, o, _ in slot_offsets}
    counts = {s: c for s, _, c in slot_offsets}
    cum = 0
    off_vals = []
    cnt_vals = []
    for s in range(19):
        off_vals.append(str(cum))
        cnt_vals.append(str(counts.get(s, 0)))
        cum += counts.get(s, 0)
    lines.append("    " + ", ".join(off_vals) + ",  // offsets")
    lines.append("};")
    lines.append("static const std::uint8_t kPuppetResolveSlotCount[19] = {")
    lines.append("    " + ", ".join(cnt_vals))
    lines.append("};")
    lines.append("")
    lines.append("inline const T4CPuppetResolveRow *T4CPuppetResolveLookup(")
    lines.append("        const std::uint8_t slot, const std::uint16_t pupeq, const bool female) {")
    lines.append("    if (slot >= 19) return nullptr;")
    lines.append("    const int base = kPuppetResolveSlotOffset[slot];")
    lines.append("    const int n = kPuppetResolveSlotCount[slot];")
    lines.append("    const std::uint8_t g = female ? 1 : 0;")
    lines.append("    for (int i = 0; i < n; ++i) {")
    lines.append("        const T4CPuppetResolveRow &r = kPuppetResolveRows[base + i];")
    lines.append("        if (r.pupeq == pupeq && r.female == g) return &r;")
    lines.append("    }")
    lines.append("    // fallback: gender-neutral row (weapons)")
    lines.append("    for (int i = 0; i < n; ++i) {")
    lines.append("        const T4CPuppetResolveRow &r = kPuppetResolveRows[base + i];")
    lines.append("        if (r.pupeq == pupeq && r.female == 0) return &r;")
    lines.append("    }")
    lines.append("    return nullptr;")
    lines.append("}")
    lines.append("")
    return "\n".join(lines)


def emit_puppetize(rows: list[tuple[str, int, int, int]]) -> str:
    lines = [
        "// Auto-genere par scripts/gen_puppet_tables.py — NE PAS EDITER.",
        "#pragma once",
        "",
        "struct T4CPuppetizeRow {",
        "    std::uint16_t objgroup;",
        "    std::uint8_t slot;",
        "    std::uint8_t pupeq;",
        "};",
        "",
        "static const T4CPuppetizeRow kPuppetizeRows[] = {",
    ]
    for field, obj, slot, pq in sorted(rows):
        lines.append(f"    {{ {obj}, {slot}, {pq} }},  // {field}")
    lines.append("};")
    lines.append(f"static const int kPuppetizeRowCount = {len(rows)};")
    lines.append("")
    return "\n".join(lines)


def emit_body_order(orders: dict[int, list[int]]) -> str:
    lines = [
        "// Auto-genere par scripts/gen_puppet_tables.py — NE PAS EDITER.",
        "#pragma once",
        "",
        "// BodyOrderR[direction][layer] — standing (Puppet.cpp)",
        "static const std::uint8_t kPuppetBodyOrderR[10][19] = {",
    ]
    for d in range(10):
        row = orders.get(d, [0] * 19)
        lines.append("    {" + ", ".join(str(x) for x in row) + "},")
    lines.append("};")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    puppet_text = PUPPET_CPP.read_text(encoding="utf-8", errors="ignore")
    puppet_h = PUPPET_H.read_text(encoding="utf-8", errors="ignore")
    app_text = APPARENCE_H.read_text(encoding="utf-8", errors="ignore")
    vlist_text = VLIST_CPP.read_text(encoding="utf-8", errors="ignore")

    pupeq_map = parse_defines(puppet_h, "PUPEQ_")
    objgroups = parse_objgroups(app_text)

    set_puppet = extract_set_puppet(puppet_text)
    resolve_table = parse_set_puppet_resolve(set_puppet, pupeq_map)
    puppetize_rows = extract_puppetize(vlist_text, objgroups, pupeq_map)
    body_orders = extract_body_order_r(puppet_text)

    OUT_RESOLVE.write_text(emit_resolve(resolve_table), encoding="utf-8")
    OUT_PUPPETIZE.write_text(emit_puppetize(puppetize_rows), encoding="utf-8")
    OUT_BODYORDER.write_text(emit_body_order(body_orders), encoding="utf-8")

    sprites = {v["sprite"] for v in resolve_table.values() if v.get("sprite")}
    print(f"PUPEQ defines     : {len(pupeq_map)}")
    print(f"OBJGROUP defines  : {len(objgroups)}")
    print(f"Resolve entries   : {len(resolve_table)}")
    print(f"Unique sprites    : {len(sprites)}")
    print(f"Puppetize rows    : {len(puppetize_rows)}")
    print(f"BodyOrderR dirs   : {len(body_orders)}")
    print(f"Wrote {OUT_RESOLVE.name}, {OUT_PUPPETIZE.name}, {OUT_BODYORDER.name}")

    # spot checks Lighthaven
    checks = [
        (4, pupeq_map.get("PUPEQ_WHITEROBE", 12), 0),
        (4, pupeq_map.get("PUPEQ_SET1", 9), 0),
        (8, pupeq_map.get("PUPEQ_BATTLESWORD", 5), 0),
        (9, pupeq_map.get("PUPEQ_ROMANSHIELD", 1), 0),
    ]
    for slot, pq, g in checks:
        key = (slot, pq, g)
        print(f"  slot {slot} pupeq {pq} -> {resolve_table.get(key)}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
