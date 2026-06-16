# FinalStEp2 — T4C Client 1.72 Linux + Windows

Client **The 4th Coming 1.72** (`CLT_VERSION 1720`) — dual-build Windows (Vircom) / Linux (SDL3).

## Moteur séparé ?

**Non.** Pas de TnC, pas de `.dec` / `.rmap`. Décodage assets **natif** (`V2Database`, `TileSet`) + renderer SDL3 dans le même projet.  
→ [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md)

## Structure

```text
FinalStEp2/
├── CHANGELOG.md           Progression horodatée
├── README.md              Ce fichier
├── client/                Source Vircom 1.72 (build Windows)
├── runtime/               Assets runtime copiés (Original Client 1.72)
├── docs/                  Guides détaillés
├── scripts/               Copie assets, build (futur)
└── src/                   Code Linux-only (futur)
```

## Assets runtime

Tous les assets nécessaires au runtime sont regroupés dans **`runtime/`** (~401 Mo, copie locale).

Guide complet : [`docs/RUNTIME_ASSETS.md`](docs/RUNTIME_ASSETS.md)

Regénérer les copies (sans rien supprimer à la source) :

```bash
./scripts/copy_runtime_assets.sh
```

Source par défaut : Original Client 1.72 sur disque dur. Override :

```bash
T4C_ORIGINAL_CLIENT=/chemin/vers/install ./scripts/copy_runtime_assets.sh
```

## Builds

| OS | Commande | Sortie |
|---|---|---|
| Windows | `client/T4C Client.sln` (Visual Studio 2022) | `T4C Client.exe` |
| Linux | CMake + `./scripts/build_client.sh` (à venir) | `t4c_client` |

Les deux builds sont **indépendants** (pas de SDL requis sur Windows, pas de DirectX sur Linux).

### Build Windows (Visual Studio 2022)

Ouvrir `client/T4C Client.sln` puis générer en **`Release | Win32`**, ou en ligne de commande :

Dans un invite de commande MS-DOS
t4c-client-linux>
```powershell
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "client\T4C Client.sln" /t:Build /p:Configuration=Release
```

> Le flag `/Zc:threadSafeInit-` est **requis** (déjà dans le `.vcxproj`) : il restaure la sémantique d'init des `static` locales de l'ère VC6 et évite les interblocages d'initialisation introduits par VS2022.

Le binaire est produit dans `client/T4C Client/Release/T4C Client.exe`.

### Lancer le client

1. Récupérer les assets runtime (voir **Assets runtime** ci-dessus).
2. Copier l'exécutable dans `runtime/` (à côté des assets) :

```powershell
Copy-Item "client\T4C Client\Release\T4C Client.exe" "runtime\T4C Client.exe" -Force
```

3. Lancer `runtime/T4C Client.exe`.

> Sur les **releases GitHub**, l'`.exe` est fourni en *asset* (il n'est pas versionné car `runtime/` est gitignoré) : le télécharger et le copier dans `runtime/` après avoir récupéré les assets.

## Serveur cible

Serveur **2008 v1r72** (recréation Linux) — protocole et version **1720** alignés.

## Avertissement

T4C est une propriété intellectuelle Vircom. Projet à des fins de recherche, préservation et portage.
