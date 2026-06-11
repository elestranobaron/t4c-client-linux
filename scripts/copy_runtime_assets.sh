#!/usr/bin/env bash
# Copie les assets runtime depuis Original Client 1.72 vers FinalStEp2/runtime/
# Politique : COPIES UNIQUEMENT — ne supprime ni ne déplace rien à la source.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/runtime"

# Source canonique (install Vircom 1.72)
DEFAULT_ORIG="/mnt/hard/underground/Nouveau dossier (2)/T4C_V1R7X/1.7/1.7/T4C Client/Original Client 1.72"
ORIG="${T4C_ORIGINAL_CLIENT:-$DEFAULT_ORIG}"

if [[ ! -d "$ORIG" ]]; then
    echo "[copy_runtime] Source introuvable: $ORIG" >&2
    echo "  Définir T4C_ORIGINAL_CLIENT=/chemin/vers/Original Client 1.72" >&2
    exit 1
fi

mkdir -p "$DEST"

echo "[copy_runtime] Source : $ORIG"
echo "[copy_runtime] Cible  : $DEST"

cp -a "$ORIG/Game Files" "$DEST/"
cp -a "$ORIG/Fonts" "$DEST/"
cp -a "$ORIG/music files" "$DEST/Music Files"
cp -a "$ORIG/fx files" "$DEST/FX Files"
cp -a "$ORIG/images" "$DEST/Images"
cp -a "$ORIG/english.elng" "$ORIG/EnglishGUI.elng" "$ORIG/french.elng" "$ORIG/FrenchGUI.elng" "$DEST/"
cp -a "$ORIG/iplist.txt" "$ORIG/license.txt" "$DEST/" 2>/dev/null || true
cp -a "$ORIG/help" "$DEST/" 2>/dev/null || true

# Sons WAV (finalstep ou autre) — musiques + SFX pour SDL3
if [[ -x "$ROOT/scripts/copy_sons_wav.sh" ]]; then
  T4C_SONS_WAV="${T4C_SONS_WAV:-$ROOT/../finalstep/client/build/data/sons}" \
    "$ROOT/scripts/copy_sons_wav.sh" || true
fi

echo "[copy_runtime] OK — $(du -sh "$DEST" | cut -f1) total"
