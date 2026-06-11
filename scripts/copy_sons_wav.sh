#!/usr/bin/env bash
# Copie les .wav depuis finalstep (build/data/sons) vers runtime/ du client 1.72 Linux.
# Musiques zone → Music Files/ ; effets → FX Files/ (pour usage futur).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEST="$ROOT/runtime"
DEFAULT_SRC="${ROOT}/../finalstep/client/build/data/sons"
SRC="${T4C_SONS_WAV:-$DEFAULT_SRC}"

if [[ ! -d "$SRC" ]]; then
  echo "[sons] Source introuvable: $SRC" >&2
  echo "  Définir T4C_SONS_WAV=/chemin/vers/sons" >&2
  exit 1
fi

MUSIC_DIR="$DEST/Music Files"
FX_DIR="$DEST/FX Files"
mkdir -p "$MUSIC_DIR" "$FX_DIR"

music_n=0
fx_n=0

shopt -s nullglob
for f in "$SRC"/*.wav; do
  base="$(basename "$f")"
  if [[ "$base" == *Music*.wav ]] || [[ "$base" == *music*.wav ]]; then
    cp -a "$f" "$MUSIC_DIR/"
    music_n=$((music_n + 1))
  else
    cp -a "$f" "$FX_DIR/"
    fx_n=$((fx_n + 1))
  fi
done
shopt -u nullglob

echo "[sons] Source : $SRC"
echo "[sons] Musiques (*Music*.wav) → $MUSIC_DIR ($music_n fichiers)"
echo "[sons] Effets (autres .wav)     → $FX_DIR ($fx_n fichiers)"
if [[ -f "$MUSIC_DIR/Sadness Music.wav" ]]; then
  echo "[sons] OK — Sadness Music.wav present"
else
  echo "[sons] WARN — Sadness Music.wav absent" >&2
fi
