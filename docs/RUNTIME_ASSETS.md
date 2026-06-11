# Guide des assets runtime — T4C Client 1.72

Référence pour remplir `runtime/` (copie locale de **Original Client 1.72**).  
Le client lit ces fichiers **tels quels** — pas de conversion `.dec` / `.rmap`.

> **Politique copies :** utiliser `./scripts/copy_runtime_assets.sh` (copie uniquement, rien n'est supprimé à la source).

---

## Layout canonique (`runtime/`)

Le répertoire de travail du client (Windows : dossier de `T4C.EXE` ; Linux : `T4C_RUNTIME` ou répertoire courant) doit ressembler à :

```text
runtime/                          ← racine runtime (T4C_RUNTIME)
├── english.elng                  ← textes jeu
├── EnglishGUI.elng               ← textes interface
├── french.elng                   (optionnel)
├── FrenchGUI.elng                (optionnel)
├── iplist.txt                    (optionnel, patch)
├── license.txt                   (optionnel)
├── Fonts/
│   └── t4cbeaulieuxV2.ttf
├── Game Files/                   ← cœur graphique + cartes
├── Images/                       ← écrans chargement / démarrage (PCX)
├── Music Files/                  ← musiques zone (MP3)
├── FX Files/                     ← effets ambiance (MP3)
└── help/                         ← aide in-game (optionnel phase UI)
```

**Attention Linux :** le code Vircom utilise `Music Files`, `FX Files`, `Images` (casse mixte).  
Les copies dans `runtime/` respectent déjà cette casse.

---

## Où coller quoi — tableau rapide

| Dossier / fichier | Source (Original Client 1.72) | Obligatoire | Rôle |
|---|---|:---:|---|
| `Game Files/` | `Game Files/` | **Oui** | Sprites V2, cartes, lumières, données UI |
| `Fonts/` | `Fonts/` | **Oui** | Police client |
| `Music Files/` | `music files/` ou `finalstep/.../data/sons` | **Oui** (audio) | Musiques zone / login — **`.wav`** pour SDL3 (`copy_sons_wav.sh`) |
| `FX Files/` | `fx files/` | Recommandé | Ambiances (cavernes, crypte) |
| `Images/` | `images/` | **Oui** (launcher) | PCX load/start par résolution |
| `english.elng` | racine install | **Oui** | Chaînes de texte |
| `EnglishGUI.elng` | racine install | **Oui** | Chaînes interface |
| `french.elng` / `FrenchGUI.elng` | racine install | Si FR | Localisation |
| `help/` | `help/` | Optionnel | Aide |
| `iplist.txt` | racine install | Optionnel | Patch / serveurs |

**Ne pas copier dans runtime** (pas des assets runtime) : `t4c.exe`, `t4cconfig.exe`, DLL Windows, `_Redist/`, `Download/`.

---

## `Game Files/` — détail par famille

### Sprites V2 (affichage monde + UI)

| Fichiers | Rôle | Référence code |
|---|---|---|
| `V2DataI.did` | Index sprites principaux | `V2Database.cpp` |
| `V2Data00.dda` … `V2Data07.dda`, `V2Data25.dda` | Banques sprites | idem |
| `V2ColorI.dpd` | Palettes sprites | `V2PalManager.cpp` |
| `V2NMSDataI.did` | Index sprites NM | `V2Database.cpp` |
| `v2nmsdata*.dda`, `V2NMSData05.dda`, `V2NMSData06.dda` | Banques NM | idem |
| `v2nmscolori.dpd` | Palettes NM | `V2PalManager.cpp` |

### Cartes (monde visible)

Le client charge des préfixes `V2_WorldMap.Map`, `V2_DungeonMap.Map`, etc.  
Sur disque (install 1.72) les fichiers sont en **`v2_worldmap.map1`**, **`v2_worldmap.map2`**, **`mapa`**, **`mapm`**, **`mapl`**, **`mapx`** (Windows : casse ignorée).

| Préfixe logique | Fichiers présents | Monde |
|---|---|---|
| `V2_WorldMap.Map` | `v2_worldmap.map*` | Monde 0 |
| `V2_DungeonMap.Map` | `v2_dungeonmap.map*` | Donjon 1 |
| `V2_CavernMap.Map` | `v2_cavernmap.map*` | Cavernes 2 |
| `V2_Underworld.Map` | `v2_underworld.map*` | Underworld 3 |
| `V2_LeoWorld.Map` | `v2_leoworld.map*` | Extension |
| `V2_Extension01.Map` … `03` | `v2_extension0*.map*` | Extensions |

Autres cartes / métadonnées :

| Fichier | Rôle |
|---|---|
| `SmoothTiles.bin` | Tuiles lissées |
| `Map.ID1`, `Map.ID2` | Index cartes |
| `RT_Map.dat` | Real-time map (GM / outils) |
| `TMI_Map.dat` | TMI map |
| `Zone_Map.dat` | Zones musique / régions |
| `zonemapinfo.zfn` | Config zones carte |
| `maps.dat` | Métadonnées maps |

### Objets au sol, lumières, inventaire

| Fichier | Rôle |
|---|---|
| `cliitmpd.bin` | Liste décor items client |
| `useitembdi.dat`, `useitembdd.dat` | Noms objets inventaire |
| `UseMapBDI.dat`, `UseMapBDD.dat` | Noms tuiles GM |
| `gmtdr.bin` | GM draw |
| `light*.raw` | Textures lumières (torches, etc.) |
| `SNMCD._`, `SNMCF._`, `SNMCI._` | Données NM client |

### Absents de l'install 1.72 copiée (non bloquant immédiat)

| Fichier référencé code | Statut install | Impact |
|---|---|---|
| `gmnmb.bin` | Absent | UI enchères / GM NM — phase tardive |
| `VSBInfo.Txt` | Présent (`vsbinfo.txt`) | Legacy audio VSB — 1.72 utilise surtout MP3 |

---

## `Music Files/` — pistes attendues

Référence : `GameMusic.cpp` (chemins `Music Files\*.mp3`).

| Fichier | Usage |
|---|---|
| `SadnessNew.mp3` / `Sadness.mp3` | Écran sélection perso |
| `Forest.mp3` / `ForestNew.mp3` | Monde extérieur |
| `dungeons.mp3` | Donjons |
| `Caverns.mp3` / `CavernsNew.mp3` | Cavernes |
| `Outdoors.mp3` | Zones outdoor |
| `Boss.mp3` | Zones boss |
| `Forest noises.mp3` | Ambiance |
| `Halloween01.mp3`, `NoelMusic01.mp3` | Événements |

## `FX Files/`

| Fichier | Usage |
|---|---|
| `Cavern01.mp3` | FX caverne |
| `crypt01.mp3` | FX crypte |

## `Images/`

PCX nommés par résolution : `load800600.pcx`, `start1280720.pcx`, etc.  
Pattern code : `Images\load{W}{H}.PCX`, `Images\start{W}{H}.PCX`.

---

## Variable d'environnement (Linux, futur)

```bash
export T4C_RUNTIME=/mnt/debian/home/tom/Public/FinalStEp2/runtime
```

Le binaire Linux résoudra les chemins depuis cette racine (équivalent du dossier contenant `T4C.EXE` sous Windows).

---

## Vérification rapide

```bash
# Présence minimale pour boot graphique
test -f runtime/Game\ Files/V2DataI.did && test -f runtime/Game\ Files/SmoothTiles.bin
test -d runtime/Music\ Files && test -f runtime/english.elng
test -d runtime/Images && test -d runtime/Fonts
find runtime/Game\ Files -name 'v2_worldmap.map1' | grep -q .
echo "Runtime OK"
```

---

## Taille de référence

Copie du 2026-06-02 : ~**401 Mo** total (`Game Files/` ~325 Mo, `music files` ~39 Mo, reste).

Source documentée dans `runtime/SOURCE.txt`.
