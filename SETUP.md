# SETUP — Client T4C 1.72 Linux (FinalStEp2)

Méthode **une seule commande** pour réintégrer les fichiers manquants après un clone.

> Les assets du jeu (≈ 486 Mo, propriétaires Vircom) ne sont **pas** dans git.
> Ils sont réintégrés depuis une install « Original Client 1.72 » que le testeur
> doit posséder (CD / archive T4C_V1R7X).

## Ce que git contient / ne contient pas

| Dans git                                   | PAS dans git (réintégré par le setup) |
|--------------------------------------------|----------------------------------------|
| `src/` (code C++)                          | `runtime/` (assets jeu : Game Files, FX, Music, Images…) |
| `scripts/` (build, setup, e2e)             | `build/` (binaires compilés)            |
| `CMakeLists.txt`, `CHANGELOG.md`, `SETUP.md` | `logs/`, `debug/` (sorties d'exécution) |

## Prérequis (Debian/Ubuntu/Arch)

- `cmake`, `g++` (C++17), `python3`
- SDL2 : `libsdl2-dev` (+ `libsdl2-ttf-dev` si présent dans CMakeLists)
- Une copie de **Original Client 1.72** (dossier contenant `Game Files/`, `FX Files/`, …)

## Méthode en 1 coup

```bash
git clone <repo-client> FinalStEp2
cd FinalStEp2

# Chemin vers l'install T4C 1.72 originale du testeur :
export T4C_ORIGINAL_CLIENT="/chemin/vers/Original Client 1.72"

./scripts/setup_from_clone.sh
```

Le script fait, dans l'ordre :

1. **Vérif outils** (cmake, g++, python3, sdl2)
2. **Réintégration assets** → `runtime/` (`scripts/copy_runtime_assets.sh`, copies uniquement)
3. **Vérif fichiers critiques** (`Game Files/V2DataI.did`, `TMI_Map.dat`, `english.elng`…) — liste précise de ce qui manque le cas échéant
4. **Build** + génération des tables (`scripts/build_client.sh`)
5. **Tests de non-régression** (inclus dans le build)

À la fin il affiche la commande de lancement :

```bash
T4C_RUNTIME=$PWD/runtime ./build/linux/t4c_client <ip_serveur> 11677
```

## Vérifier sans rien copier

```bash
./scripts/setup_from_clone.sh --check
```

Liste ce qui est présent/manquant (outils + assets) sans rien modifier.

## Test complet client+serveur (E2E)

Nécessite le dépôt serveur installé à côté (voir `T4Serv3/SETUP.md`) :

```bash
FinalStEp2/        # ce dépôt
FinalStEp0/T4Serv3 # dépôt serveur (chemin par défaut, sinon T4C_SERVER_DIR=…)
```

Puis :

```bash
./scripts/run_e2e_test.sh    # serveur + sonde auth + journal horodaté
```
