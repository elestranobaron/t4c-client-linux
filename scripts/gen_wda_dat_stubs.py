#!/usr/bin/env python3
"""Generate minimal empty XOR-encrypted WDA .dat stubs for Linux server runtime."""

from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

SERVER = Path(__file__).resolve().parents[2] / "FinalStEp0/T4Serv3"
EXE = SERVER / "Exe Server"

STUBS = [
    ("QUEST_key.h", "QUEST_key", "QuestBook.dat"),
    ("PROFESSION_key.h", "PROFESSION_key", "NMS_Profession.dat"),
    ("EVENTS_key.h", "EVENTS_key", "NMS_Events.dat"),
]

RUNTIME_DIRS = [
    SERVER / "runtime/WDA",
    SERVER / "build/WDA",
]


def load_key(header_path: Path, symbol: str) -> bytes:
    text = header_path.read_text(encoding="utf-8", errors="replace")
    m = re.search(rf"unsigned char {symbol}\[\]\s*=\s*\{{([^}}]+)\}}", text, re.S)
    if not m:
        raise RuntimeError(f"key {symbol} not found in {header_path}")
    nums = [int(x, 0) for x in re.findall(r"0x[0-9A-Fa-f]+", m.group(1))]
    return bytes(nums)


def encrypt_payload(payload: bytes, key: bytes) -> bytes:
    out = bytearray(payload)
    for i, b in enumerate(out):
        out[i] = b ^ key[i % len(key)]
    return bytes(out)


def main() -> int:
    for key_header, key_symbol, filename in STUBS:
        key = load_key(EXE / key_header, key_symbol)
        payload = struct.pack("<I", 0)  # zero entries
        blob = encrypt_payload(payload, key)
        for dest_dir in RUNTIME_DIRS:
            dest_dir.mkdir(parents=True, exist_ok=True)
            out = dest_dir / filename
            out.write_bytes(blob)
            print(f"Wrote {out} ({len(blob)} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
