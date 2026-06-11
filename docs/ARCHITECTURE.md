# Architecture — T4C Client 1.72 dual-build (FinalStEp2)

## Moteur séparé type TnC ? **Non.**

Ce projet **n'utilise pas** `t4c-world-engine_sdl3` ni le pipeline mestoph (`.dec` / `.rmap` / `convert2`).

| | Port 1.68 + TnC | **FinalStEp2 (1.72)** |
|---|---|---|
| Moteur graphique | Repo séparé (TnC) | **Inclus dans le client** |
| Assets runtime | Convertis offline | **Natifs** (`runtime/`) |
| Décodage sprites | `.dec` pré-générés | `V2Database` (runtime, code client) |
| Décodage cartes | `.rmap` pré-générés | `TileSet` + fichiers `.map*` natifs |
| Présentation | SDL3 via TnC | SDL3 via modules propres (`src/render/`, sans shim DirectDraw) |

Tout vit dans **un seul dépôt** :

```text
FinalStEp2/
├── client/          Source Vircom 1.72 (Windows / référence)
├── src/             Code Linux-only (réseau, GUI SDL3, assets V2, render SDL3) — à venir
├── runtime/         Assets copiés (Original Client 1.72)
├── docs/            Documentation
└── scripts/         Outils (copie assets, build futur)
```

## Deux builds indépendants

| Plateforme | Entrée build | Binaire cible |
|---|---|---|
| Windows | `client/T4C Client.sln` | `T4C.EXE` |
| Linux | `CMakeLists.txt` (racine ou `client/`, à venir) | `t4c_client` |

Aucune plateforme ne requiert l'autre pour compiler.

## Interfaces cibles (sans shim)

- `INetworkSession` — protocole TFC 1.72
- `IAssetStore` — lecture `V2Data*.dda` (algorithmes extraits de `V2Database.cpp`)
- `IMapStore` — cartes natives (`SmoothTiles.bin`, `v2_*map.map*`, etc.)
- `IRenderBackend` — SDL3 natif (Linux) / DirectDraw (Windows legacy)

Voir le CHANGELOG pour la progression phase par phase.
