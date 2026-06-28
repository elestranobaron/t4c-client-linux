# Changelog — FinalStEp2 (T4C Client 1.72)

Historique de progression — discussions, décisions, travail passé et à venir.  
Convention : **`YYYY-MM-DD HH:MM:SS`** par entrée significative.

Familles : **Décision**, **Assets**, **Build**, **Réseau**, **Rendu**, **Serveur**, **Doc**.

---

## 2026-06-28 07:29:00 — FIX MAJEUR : troncature de l'ID d'unité sur les moves (PNJ figés, doublons, cadavre fantôme)

**Famille : Réseau / Rendu — cause racine d'un bug ancien et difficile**

### Symptômes (longue date)
- Les PNJ / monstres **ne se déplaçaient pas** à l'écran (semblaient figés ou ne bougeaient que par à-coups de combat).
- **Dédoublement** d'une créature dès le premier coup.
- À la mort : le **skin restait visible et s'animait au survol** au lieu de disparaître.

### Cause racine
`QueueRemoteMove(...)` déclarait son paramètre `unitId` en **`std::uint16_t`**, alors que les
IDs serveur sont sur **32 bits**. Chaque move distant voyait donc son ID **tronqué à 16 bits**
(`1052167 & 0xFFFF = 3591`, `1051820 & 0xFFFF = 3244`, …). Conséquence : le client maintenait
**deux unités par créature** :
- l'unité **réelle** (ID complet) — spawn, attaque, et **mort** corrects (le cadavre s'affiche bien) ;
- une unité **fantôme** (ID tronqué) qui recevait **tous les déplacements** mais n'était **jamais
  tuée** (aucun paquet de mort ne référence l'ID tronqué) → elle continuait à marcher, s'animer et
  restait sélectionnable indéfiniment.

Diagnostic obtenu via logs `[DIAG]` temporaires : les `move QUEUE id=3591` ne correspondaient
jamais aux `attaque/mort id=1052167` — preuve directe de la troncature.

### Correctif
- **`QueueRemoteMove`** : paramètre `unitId` passé de `std::uint16_t` → **`std::int32_t`**.
  Les moves, spawns, attaques et morts partagent désormais le **même ID 32 bits** → une seule
  unité par créature, qui se déplace, meurt et disparaît correctement.

### Rendu — contour cible
- **Contour de la cible sélectionnée** : couleur passée de bleu `(100,220,255)` → **blanc**
  (`CL_WHITE`), fidèle à Windows (`VisualObjectList.cpp` `dwOutlineColor = CL_WHITE`). Le contour
  reste limité à l'unité **sélectionnée** (comportement `TargetDlg`), inchangé par ailleurs.

### Vérifications
- Build propre, **105/105** tests de régression OK.
- Aucune régression vs dernier push (le fix corrige le routage d'ID, sans toucher au reste).

---

## 2026-06-28 05:00:00 — FIX dedoublement monstres : MoveObject combat + mort Windows

**Famille : Réseau / Rendu — fidelite VisualObjectList.cpp + Packet.cpp 10001**

### Cause dedoublement (1er coup / mort)
- Windows repositionne **attaquant ET defenseur** sur AXPos/DXPos a chaque `10001` (`MoveObject`).
- Linux ne bougeait que l'attaquant → monstre fige a l'ancienne tuile + combat a la nouvelle = **clone**.
- Mort : Windows garde le sprite **vivant 500 ms** (`Killed`) puis bascule `KillType` ; Linux affichait cadavre + vivant.

### Correctifs
1. **`QueueRemoteCombatSnap`** : snap display+serveur sur les deux IDs a chaque attaque (10001/10002).
2. **`beginDeath`** : `showCorpse=false` 500 ms (kT4CDyingPhaseMs), snap display a la mort.
3. **`HandleMissingUnit`** : ignore si unite deja tracée / mourante (evite doublon opcode 71).
4. **`kT4CNpcWalkAnimFrames=6`** : Rat n'a que a–f (plus de `Rat180-g` / frame fantome).

---

## 2026-06-28 04:15:00 — FIX puppet COMP_NULL + preload WarrioStMov

**Famille : Rendu**

- **`COMP_NULL` (type 3)** : frames vides Windows (ex. `PupNakedHandR045-h`) — plus de `decode KO`.
- **Preload joueur** : plus de StMov / nom nu / cadavre `WarrioC-*` au chargement monde.
- **Puppet `drawLayer`** : plus de fallback sur nom nu (`PupNakedHandL`) ; repli frame 0 si frame courante vide.

---

## 2026-06-28 04:00:00 — FIX rendu creatures : StMov supprimé, marche 8 frames (Bat)

**Famille : Rendu — cause visible « aucun changement » malgré binaire à jour**

### Diagnostic (logs utilisateur)
- Le binaire récent tournait bien (`preload bloquant : 986 sprites (creatures a la demande au spawn)`).
- Les monstres restaient invisibles / points colorés : le client cherchait des sprites **inexistants**
  dans `V2DataI.did` (`BatStMov*`, bare `Bat`, frame `Bat045-i` = 9e frame alors que Bat n'a que a–h).
- `drawUnitSprite KO … sprite=Bat` = échec après frame 8 du cycle marche.

### Correctifs
1. **`tryDrawNpcBase` / `tryDrawNpcOutline`** : plus de lookup StMov ni fallback sur le nom nu ;
   pose statique = frame 0 du cycle marche ; clamp 8 frames (`kT4CNpcWalkAnimFrames`).
2. **`T4CWalkAnimNextNpcFrame`** pour les unités distantes (mod 8, pas 9).
3. **Preload minimal** : plus de `appendUnique(base)` seul (évite `index absent 'Bat'`).
4. **Joueur classe NPC** : plus de StMov idle (`WarrioStMov*` absent du `.did`).

---

## 2026-06-28 03:30:00 — FIX mort/clone : attaques fantômes + tempête preload StMov

**Famille : Réseau / Rendu — bugs visibles dans les logs utilisateur**

### Symptômes (logs)
- Après `opcode 12` (mort), le serveur envoie encore `att=1052061` — le client recréait l'unité
  morte via `Attack`/`Update` si absente de `remoteUnits_` (seul `unit.dying` local bloquait).
- Milliers de `WARNING: [SpriteAtlas] index absent 'XXXStMov045-a'` — preload différé de
  **toute** la table creatures avec frames idle inexistantes dans le `.did`.

### Correctifs
1. **`T4CLoginSessionIsRemoteUnitDying()`** exporté ; `syncRemoteUnitsFromNetwork()` ignore
   Spawn/Move/Update/Attack pour les ids dans `g_dyingUnits`.
2. **`HandleAttackResult`** : `hpPercent <= 0` → `QueueRemoteDie()` immédiat (avant opcode 12) ;
   attaques/missing-unit ignorées si attaquant ou défenseur déjà mourant.
3. **Preload** : suppression du preload différé massif (`T4CCollectCreaturePreloadBases`) ;
   preload minimal à la demande au 1er spawn (`T4CAppendUnitSpritePreloadMinimal`).

---

## 2026-06-23 06:15:00 — FIX audit corruptions Composer : contour fantôme, picking morts, snap GetNearItems

**Famille : Rendu / Réseau — corrections issues de l'audit git diff**

### Contour bleu / fantôme invisible au survol
- `drawUnitOutline()` : plus de repli sur le sprite **vivant** quand le cadavre n'a pas de sprite
  (cause du « double » / ombre bleue animée à la souris après la mort).
- Suppression du fallback robe blanche `body=425` dans le contour (incohérent avec `KnownPuppet`
  invisible dans `drawUnitSprite`).
- Suppression du rectangle bleu `(100,220,255)` de secours (invention Composer).
- Contour réservé à la **cible sélectionnée** (`V3_TargetDlg`), plus au survol souris.

### Picking / sélection
- `isRemoteUnitSelectable()` : unités `dying`, `showCorpse`, puppets sans dress → non
  sélectionnables ni survolables (`pickUnitAtScreen`).

### Déplacement PNJ (régression snap GetNearItems)
- `syncRemoteUnitsFromNetwork()` : restauration du snap immédiat sur `Spawn` ou `|dx|>3`
  (GetNearItems / bande périphérique), comme au dernier commit.
- Interpolation « retard max 1 tuile » conservée **uniquement** pour les moves UDP 1–8
  (petit delta), conforme `VisualObjectList.cpp:15011`.

---

## 2026-06-23 05:30:00 — RÉGRESSION CRITIQUE : NPC immobiles (« surplace » + repositionnement seulement à l'entrée de zone)

**Famille : Réseau — correction d'une régression non commitée, non documentée**

### Symptôme
La plupart des NPC (sauf gardes d'Olin Haad et certaines unités, ex. « les femmes ») ne se
déplaçaient plus : animation de marche « sur place » puis téléport, position mise à jour
uniquement à l'entrée/sortie de zone (GetNearItems). Au dernier commit le déplacement
fonctionnait → régression introduite dans les changements non commités.

### Cause racine
`NormalizeServerAppearance()` (`T4CLoginSession.cpp`) contenait un ajout fautif :

```cpp
case 0:
    return 274;   // <-- ABERRANT : 274 = id sprite corps (dress), pas une apparence d'unité
```

Or le paquet de move Windows (`Packet.cpp` `TFCMoveID` → `Objects.MoveObject(ID, X, Y, …, TYPE, …)`)
envoie `TYPE = 0` pour signifier « pas de changement d'apparence : garde celle de l'objet existant ».
Conséquence du remap fautif : un move avec `TYPE = 0` devenait `appearance = 274`, ce qui :
1. **empoisonnait** l'apparence mémorisée (`RememberRemoteAppearance` → 274),
2. faisait **tomber le move** dans `QueueRemoteMove` (`IsRemoteDrawableUnit(274) == false` car 274 < 10001).

Résultat : les moves UDP par tuile étaient ignorés ; seules les annonces périodiques GetNearItems
repositionnaient l'unité (« téléport »), d'où le « surplace ».

### Correctif
Suppression du `case 0: return 274;`. `TYPE/appearance == 0` reste désormais `0`, donc
`QueueRemoteMove` retombe sur l'apparence réelle mémorisée (`g_remoteAppearance`) et applique le move,
exactement comme `MoveObject` côté Windows conserve le type de l'objet existant.

---

## 2026-06-23 03:40:00 — INVESTIGATION PNJ « robe blanche » + clone mort + marche/téléport (référence : client Windows d'origine)

**Famille : Rendu / Réseau — comportements alignés EXACTEMENT sur `client/T4C Client/`**

### Contexte
Trois symptômes signalés : (1) « PNJ en robe blanche » nommés « PNJ » partout dans Lighthaven
alors qu'il ne devrait y avoir que 2 NPC (Shovanis, Nevanis) ; (2) monstre tué laisse un
« clone » figé sur place ; (3) une unité « marche sur place puis se téléporte ailleurs ».
Décision : ne plus supposer, mais **lire le code d'origine comme spec authoritative**.

### Constat n°1 — Puppets non habillés = INVISIBLES dans l'original (preuve source)
- `ObjectListing.h` : `10011 = __PLAYER_HUMAN_PUPPET` (un **autre joueur**, pas un NPC),
  `10012 = __PLAYER_HUMAN_FEMALE`. Les vrais NPC humains sont `10005–10010`.
- `VisualObjectList.cpp:9775` : à la création, `Object->KnownPuppet = FALSE`.
- `VisualObjectList.cpp:22617` : `KnownPuppet = TRUE` **uniquement** à la réception de
  l'équipement (`RQ_PuppetInformation` → `Puppetize(...)`), keyé par `if (Object->ID == ID)`.
- `VisualObjectList.cpp:15734, 17062, 17380, 21364` (4 passes de rendu) :
  `if (!((Type==__PLAYER_HUMAN_PUPPET || Type==__PLAYER_HUMAN_FEMALE) && !KnownPuppet))`
  → un puppet dont l'équipement n'est pas encore reçu **n'est PAS dessiné**.
- **Conclusion** : les « PNJ robe blanche » étaient des puppets joueurs pas encore habillés.
  L'original les rend invisibles jusqu'à réception de l'équipement ; nous les dessinions en
  fallback robe blanche (`body=425`) avec le label « PNJ ».

### Constat n°2 — Pas de doublon possible par personne dans l'original
- `VisualObjectList.cpp:9649 Add(...)` : dédoublonnage par **ID** (`if (GetObject()->ID == ID)
  { AlreadyHere = true; return; }`, ligne 9673). L'habillage se fait **sur le même objet**.
- Donc une personne = **un seul objet par ID**, jamais « un robe blanche + un habillé ».
  Chez nous `remoteUnits_` est aussi keyé par `unitId` : un doublon n'est possible que si le
  serveur émet **deux ID distincts** pour la même entité (cas où l'original dupliquerait aussi).

### Constat n°3 — Modèle de déplacement de l'original (anti « marche sur place + téléport »)
- `VisualObjectList.cpp:15011` (Move2, `D->Type==0`) : à la réception d'un move, la position
  **logique** saute immédiatement à la tuile cible (`AbsX/OX = NewX`) ; seul un décalage
  **visuel d'au plus UNE tuile** glisse pour rattraper (`Object->MovX = (OX-newOX)*32`,
  résorbé frame par frame par `SpeedX/SpeedY`). Le retard visuel ne dépasse donc jamais 1 tuile.
- Notre code lerpait `displayX → serverX` sans borne et ne « snappait » qu'au-delà de **3 tuiles**
  → en cas de paquets UDP perdus, l'unité accumulait du retard (animation de marche sans
  déplacement visible = « sur place ») puis sautait (« téléport ») au prochain gros écart.

### Corrections appliquées (`src/`)
- `GameWorldScreen.cpp` `drawUnitSprite()` : un puppet (`10011/10012/15011/15012`) sans
  équipement reçu (`T4CLoginSessionGetRemotePuppetDress` == false) **n'est plus dessiné**
  (retour `false`) — fini la robe blanche fallback. Conforme à `VisualObjectList.cpp:15734`.
- `GameWorldScreen.cpp` boucle de rendu : plus de carré/point de fallback pour un puppet
  invisible non habillé.
- `GameWorldScreen.cpp` `pickUnitAtScreen()` : un puppet non habillé est **non
  sélectionnable / non survolable** (cohérent avec « invisible »).
- `GameWorldScreen.cpp` `tryDrawCorpse()` : suppression du fallback qui dessinait le sprite
  **vivant** quand le sprite cadavre (`BatC-*`…) est absent de l'atlas — ce fallback créait le
  « clone figé » du monstre mort. Sans sprite cadavre, le mort disparaît (au lieu de rester
  affiché vivant).
- `GameWorldScreen.cpp` `syncRemoteUnitsFromNetwork()` (Spawn/Move/Update/Attack) : retard
  visuel **borné à 1 tuile** (modèle `VisualObjectList.cpp:15011`). La position logique
  (`serverX/serverY`) est posée immédiatement ; si l'écart visuel > 1 tuile, `displayX/displayY`
  est replacé à 1 tuile de la cible dans la direction du déplacement. Plus de téléport >1 tuile,
  plus d'animation « sur place ».

### À retester côté utilisateur
- Lighthaven : ne doivent rester que les NPC réellement habillés (Shovanis/Nevanis…) ; les
  puppets joueurs non habillés sont invisibles tant que leur équipement n'est pas reçu.
- Mort d'un monstre : plus de clone vivant figé.
- Déplacement des unités distantes : glissement régulier, sans saut.

### Piste ouverte (non corrigée, à confirmer avec logs)
- Si un « robe blanche » réapparaît, capturer `id`+`app`+position : vérifier si le serveur
  émet bien `RQ_PuppetInformation`/opcode 70 pour cet ID (sinon il reste invisible, ce qui est
  le comportement d'origine), ou s'il s'agit d'un 2e ID distinct pour la même entité.

---

## 2026-06-23 04:50:00 — FIX mort (suppression unité vivante) + surplace PNJ + robes blanches dégagées

**Famille : Rendu — 3 demandes utilisateur explicites**

### 1. Robes blanches « PNJ » — SUPPRIMÉES définitivement
- `drawUnitSprite()` : un puppet (`10011/10012`) sans équipement reçu (`GetRemotePuppetDress`
  == false) **n'est plus dessiné** (return false), comme Windows (`VisualObjectList.cpp:15734`,
  `KnownPuppet==false` → pas de draw). Plus de fallback robe blanche `body=425`.
- Point de fallback et sélection/survol également désactivés pour ces puppets invisibles.

### 2. Monstres/PNJ qui « marchent sur place » à l'arrêt — CORRIGÉ
- Cause : pour une unité immobile sans sprite idle `StMov` (beaucoup sont absents de l'atlas,
  cf. warnings `…StMov…-x absent`), `tryDrawNpcBase()` retombait sur le sprite de **marche**
  indexé par une frame qui défile → marche immobile. Idem chemin puppet/joueur-NPC.
- Fix : à l'arrêt (`idleStanding`), on dessine une **pose statique (frame 0)** au lieu de la
  frame de marche qui défile. Appliqué à `tryDrawNpcBase()`, au chemin puppet et au chemin
  sprite joueur-NPC.

### 3. Mort d'un monstre : suppression de l'unité vivante (fin du « dédoublement »)
- Théorie utilisateur confirmée : l'animation de mort doit être séparée et **l'unité vivante
  doit être supprimée** à l'instant de la mort.
- Cause du clone : pendant la phase « dying » de 500 ms (`showCorpse == false`), on dessinait
  encore le **sprite vivant figé** à côté des autres monstres vivants → effet de doublon.
- Fix : `beginDeath()` passe **directement** à `showCorpse = true` (plus de phase de gel du
  sprite vivant). Les unités mourantes ne sont rendues que par `tryDrawCorpse()` ; sans sprite
  cadavre dédié, l'unité **disparaît** (au lieu de laisser un clone vivant). Le son d'agonie
  est déjà joué sur le coup fatal (`PlayCreatureHitSfx`). Le cadavre disparaît après expiration.

---

## 2026-06-23 03:55:00 — REVERT invisibilité puppet (régression « plus rien ne bouge ») + FIX joueur marche sur place

**Famille : Rendu — stop régression, retour comportement dernier commit**

### Régressions signalées
1. Le **personnage joueur à l'arrêt** joue une animation de marche **perpétuelle**.
2. **Tous les personnages distants sauf les gardes d'Olin Haad** ne se déplacent/affichent plus.

### Cause n°1 (joueur marche sur place) — RÉGRESSION corrigée
- `GameWorldScreen.cpp` rendu joueur : le rendu puppet (`T4CPuppetTryDrawPlayer`) indexe le
  cycle de **marche** (13 frames) avec `frameIndex` et n'a **pas** de cycle idle séparé. À
  l'arrêt on lui passait `playerIdleAnimFrame_` (qui défile 0→3 toutes les 150 ms) → marche
  sur place. Windows : un puppet à l'arrêt garde `SpriteNumber = 1` (statique).
- **Fix** : à l'arrêt, le puppet joueur utilise la frame **statique 0** (`puppetFrame`), au lieu
  de la frame idle qui défile. (L'anim idle StMov ne vaut que pour les sprites NPC non-puppet.)

### Cause n°2 (« plus rien ne bouge sauf les gardes ») — REVERT de l'invisibilité puppet
- Constat : **avant** le changement d'invisibilité, ces puppets étaient TOUS en robe blanche
  → `dress.known == false` → l'équipement (opcode 68) n'est **jamais appliqué** dans notre
  client. Donc rendre les puppets non habillés invisibles a transformé « robe blanche qui
  bouge » en « rien » : seuls les NPC non-puppet (gardes Olin Haad, type 10006) restaient.
- **Décision** : la bonne cible Windows n'est PAS de cacher, mais d'**habiller** (opcode 68).
  En attendant, on **annule** les 3 edits d'invisibilité pour stopper la régression :
  - `drawUnitSprite()` : retour du fallback robe blanche (`body=425`) pour puppet sans dress.
  - boucle de rendu : retour du point de fallback.
  - `pickUnitAtScreen()` : retour de la sélection/survol normale.
- **Conservé** (corrections valides) : joueur idle statique, retard visuel mouvement borné à
  1 tuile, suppression du fallback sprite vivant pour cadavre absent.

### Diagnostic ajouté
- `T4CLoginSession.cpp` `HandlePuppetInformation()` : log `[PHASE] <- puppet info (68) id=…
  body=… legs=… …` à chaque réception d'équipement. **But** : déterminer si le serveur
  envoie réellement l'opcode 68 pour ces unités. Le parsing (id long, puis body/feet/gloves/
  helm/legs/wR/wL/cape/hair/tag en short) est **identique** à l'original (`Packet.cpp:3265`).

### Prochaine étape (vraie correction Windows, à valider par le log)
- Si le log `puppet info (68)` **n'apparaît jamais** pour ces unités → le serveur ne répond
  pas à notre `RQ_PuppetInformation`, ou l'équipement transite par un autre canal (push inline) :
  à investiguer côté réception. C'est ÇA la cause racine des robes blanches.
- Si le log apparaît mais l'unité reste en robe blanche → bug d'application (`g_remotePuppet`
  / `GetRemotePuppetDress`).

---

## 2026-06-22 08:00:00 — FIX combat, coffres, animation sort

**Famille : Gameplay / Réseau**

- **Attaque** : clic simple sur un monstre (appearance 20000–24999) déclenche l'attaque ; les monstres n'étaient plus traités comme PNJ « à parler ».
- **Coffres** : clic = `RQ_UseObject` (ouvrir), plus `RQ_GetObject` (ramassage) — corrige le message « encombrement maximal ». Panneau coffre (opcodes 221/220/222) + prise d'objet (108).
- **Sorts** : opcode 64 `RQ_SpellEffect` oriente le perso + animation de lancer (geste attaque court).

---

## 2026-06-22 06:30:00 — FIX inventaire : sync sac (154) + equiper via paperdoll

**Famille : Réseau / Gameplay**

- **Sync sac** : `RequestViewBackpack` envoie `RQ_ViewBackpack2` (154) au lieu de `RQ_ViewInv` (156) — le client ne recevait pas le sac complet après ramassage.
- **Ramassage** : opcode 123 (`UpdateBackpackAfterUse`) ajoute l'objet localement s'il est inconnu, puis resync 154.
- **Equiper** : clic sur un emplacement vide du paperdoll avec un objet sélectionné → `RQ_EquipObject` (21), comme le drag-drop Windows.

---

## 2026-06-22 05:00:00 — FIX dialogue PNJ (retour ligne) + grille achat marchand

**Famille : Rendu / Réseau**

- **Dialogue** : texte PNJ avec retour à la ligne dans le panneau (clip + wrap, jusqu'à 7 lignes) ; panneau agrandi en bas.
- **Boutique** : parse opcode 41 corrigé — la 2e chaîne `req` par article n'était pas lue (désync paquet → liste vide). Grille centrée type Windows (colonnes Objet/Prix, lignes alternées, compteur d'articles).

---

**Famille : Rendu / Build / Réseau**

**Bandeau defilant** : ticker independant (`T4CScrollingBanner::startTicker`, thread 60 Hz) — defilement fluide pendant le chargement lourd ; UI presentee **avant** le decode sprites (12 ms/frame max).

**Chargement accelere** :
- Preload bloquant reduit : tuiles viewport + perso local (frame 0) + HUD/curseurs — **sans** preload massif de toutes les creatures ni des 4 classes.
- Creatures + cycles marche complets : preload **differe en jeu** (`pollDeferredSpritePreload`, 6 ms/frame).
- `PreloadSpritesForMs` : budget temps par frame au lieu de lots fixes.

**Regression opcode 46** : renvoi immediat apres PutPlayerInGame (13) ; le serveur ne timeout plus. Gameplay (`canMove_`, `ready_`) toujours bloque jusqu'a fin du preload + reponse 46.

---

## 2026-06-22 04:00:00 — FIX entree monde : opcode 46 differe apres chargement assets

**Famille : Réseau / Rendu**

**Symptôme :** le perso est `in_game=1` cote serveur (vulnerable aux PNJ/combat) pendant l'ecran « Chargement du monde… » — le client envoyait l'opcode 46 des la reception du PutPlayerInGame (13), avant la fin du decode sprites/carte.

**Correctif (aligne intention Windows `DoNotMove`) :**
- `g_deferredEnterWorld46` : opcode 46 envoye uniquement via `T4CLoginSessionSubmitWorldLoadComplete()` a la fin du preload (`finishAssetLoad`).
- Entree session (`RequestNearItems`, stats, sac…) reportee dans `completeSessionEnter()` apres reponse 46 OK.
- Teleport (57) : opcode 46 differe aussi (`g_deferredTeleportResync`) jusqu'au reload carte client ; `canMove_` bloque tant que `!IsWorldSessionReady()`.
- Ecran loading : phase « Connexion au monde… » pendant l'attente 46 post-assets.

---

## 2026-06-22 12:00:00 — Sprint final in-game : T2/T3/T5/T6 (parité Windows)

**Famille : Réseau / Rendu**

Correctifs alignés sur `docs/SPRINT_FINAL.md` (source de vérité = client Windows) :

- **T5 — Chat PNJ** : `sendChatLine` route le texte vers `RQ_DirectedTalk` (30) + message quand `g_talkState.active` (`T4CLoginSessionSendDirectedTalkMessage`, miroir `main2.cpp` L2475–2537). Les mots-clés (`acheter`, `oui`…) atteignent enfin le serveur.
- **T2 — Coffres↔portes** : remap `_I` → type canonique dans `T4CGroundObjectSprites.cpp` (table `SetMouseCursor` L156–188) avant lookup sprite ; 18/19/20 → coffres au lieu de RockDoor.
- **T3 — Spawns fantômes** : `g_expectInviewRefresh` consommé sur opcode 16 si count ≥ 8 (inview complet vs bande périphérique) → purge unités/sol hors liste ; `IsLocalPlayerMoveTarget` compare l'`unitId` joueur (ne avale plus les moves PNJ adjacents).
- **T6 — Anim marche** : préchargement des **9 frames** par angle (`T4CAppendUnitSpritePreload`) au lieu de 0–2 seulement.

Build : `cmake --build build/linux` OK.

---

## 2026-06-22 02:30:00 — DOC : sprint final parité client Linux↔Windows (`docs/SPRINT_FINAL.md`)

**Famille : Doc / Décision**

Préparation du sprint pour l'agent (Composer). Document `docs/SPRINT_FINAL.md` : 9 tâches ancrées dans le code réel (fichiers + fonctions), issues du diagnostic des sous-systèmes rendu/réseau/HUD.

Tâches : T1 sprites « carte verte » (résolution appearance→sprite, ex. Dark Fang dragon ; diagnostic d'abord) · T2 coffres rendus en portes (remap `_I`→type de base 18→41/19→42/20→238, parité `VisualObjectList.cpp`) · T3 spawns fantômes PNJ (purge inview non consommée, `IsLocalPlayerMoveTarget` par tuile) · T4 ombres incohérentes (résolution sprite/fallback puppet) · T5 chat PNJ : phrases tapées ignorées (`sendChatLine` envoie toujours opcode 27 ; doit router DirectedTalk 30+texte si `g_talkState.active`, miroir `main2.cpp`) · T6 anim marche 1–2 frames manquantes (préchargement 0–2 seulement) · T7 objets/inventaire incomplets · T8 HUD incomplet (framework `RootBoxUI`+widgets non porté) · T9 directive : porter Windows→Linux à l'identique (paliers A→E).

Confirmation : **oui**, il faut porter le reste du fonctionnement client Windows sur Linux à l'identique (le Linux ne partage aujourd'hui que la pile réseau).

---

## 2026-06-22 02:17:00 — FIX serveur Linux : sauvegarde du personnage à la déconnexion (position/XP/items)

**Famille : Serveur**

**Symptôme :** à chaque reconnexion, le personnage (`AZERTY`, compte `test`) réapparaissait au spawn (`2944,1059 w=0`) au lieu de sa position de sortie. Sauvegarde de déconnexion à **0 % de succès**. La BDD restait figée (niv 1, STR 18, XP 0) malgré la progression en jeu.

**Binaire serveur :** `/mnt/debian/home/tom/Public/FinalStEp0/T4Serv3/build/T4CServer` (compilé via CMake depuis `Exe Server/`, **pas** `tools/`).

### Cause racine

La save de déconnexion était accrochée au mauvais déclencheur. Sur Linux il existe **plusieurs chemins de sortie** distincts, et chaque test empruntait un chemin différent :
- `AcceptVoluntaryExitGame` (ExitGame volontaire) — **non appelé** quand l'antiplug refuse l'ExitGame (déco < ~13 s d'inactivité).
- Chemin de **suppression rapide** dans `AsyncDeletePlayer` (`!in_game && !registred && !boPreInGame && !boRerolling && keyCode==0`) : `delete` immédiat, court-circuitant la save.
- **`PurgeAccountSessionsExcept`** (déclenché par la reconnexion) : supprime l'ancienne session en jeu via `DeletePlayer(…, FALSE)` sans passer par la save.

Les saves qui « fonctionnaient » dans les logs (`ODBC requests took 0-1ms`) étaient des saves de **login** (`GetPersonnalPClist`, `bFORCE=TRUE`) → position encore au spawn.

### Tentatives infructueuses (régressions, finalement reverties)

1. **Save synchrone + `WaitForSaving()` dans `AcceptVoluntaryExitGame`** (thread UDP) → blocage du thread réseau : `[PKT] paquet 26 ignore — joueur NULL`, reconnexion instable.
2. **`SaveCharacter(TRUE, …, TRUE)` (boCallback=TRUE) + `WaitForSaving()` dans `AsyncDeletePlayer`** → `[DEADLOCK] Suspected stall in 'Player Deletion Thread'` (l'event `hCreationEvent` jamais signalé par le callback ODBC pendant la suppression).
3. **Flag `boSaveOnDelete` posé dans `AcceptVoluntaryExitGame`** → jamais lu, car la fonction n'est pas appelée quand l'antiplug refuse.
4. **Gate sur `boCanSave` dans le fast-path `AsyncDeletePlayer`** → ne couvre pas la purge de reconnexion.

Mécanisme de save (référence) :
- `SaveCharacter(bFORCE, who, boCallback)` : si `!boCanSave && !bFORCE` → retourne FALSE (rien). Construit le SQL (`UPDATE PlayingCharacters SET … wlX,wlY,wlWorld … WHERE UserID`), puis `ODBCCharAsyncSave.SendBatchRequest` (fire-and-forget vers le thread ODBC, qui commit).
- `boCallback=TRUE` → `SavingStart()` fait `ResetEvent` ; `WaitForSaving()` bloque à l'INFINI jusqu'au callback `DataSaveCallback`→`SavingStop()`. **Source du deadlock** si le callback ne vient pas.
- `boCallback=FALSE` → pas de `ResetEvent`, `WaitForSaving()` ne bloque pas.

### Correctif retenu (fonctionne, confirmé en jeu)

Sauvegarde dans **`CPlayerManager::DeletePlayer`** (`Exe Server/PlayerManager.cpp`), le **point de passage UNIQUE** de toutes les déconnexions (seule fonction qui `PostQueuedCompletionStatus(hDeletionIo,…)` → alimente `AsyncDeletePlayer`). Appelée par : maintenance, idle, `PurgeAccountSessionsExcept`, `DropFlaggedSessionAt`, `CommCenter`.

- Save effectuée **tôt**, avant tout `reset_character`/teardown → la position/XP en mémoire sont encore les vraies.
- `SaveCharacter(TRUE, "DeletePlayer", FALSE)` : `bFORCE=TRUE` (ne dépend pas de `boCanSave`, comme la save login qui marche) ; `boCallback=FALSE` (fire-and-forget, **aucun blocage**, pas de deadlock, pas d'impact reconnexion).
- Condition : `self != NULL && ( in_game || boCanSave )` — couvre purge/timeout (`in_game`) et ExitGame (`boCanSave`, posé à l'opcode 46 et conservé).
- Log diagnostic stderr : `[AUTH] DeletePlayer save compte='…' perso='…' ret=… UserID=… pos X,Y w=W in_game=… canSave=…`.

`AsyncDeletePlayer` **remis à son état d'origine** (les saves y étaient redondantes et risquaient d'écraser la bonne position par le spawn après `reset_character`).

> Note : `ret=0` dans le log est **normal** — `SaveCharacter` retourne `FALSE` à sa fin normale après l'envoi du batch ; ce n'est pas un échec.

### Vestiges laissés en place (sur demande, sans impact fonctionnel)

- Log `[AUTH] DeletePlayer save …` conservé (utile si futur bug de save).
- Champ `Players::boSaveOnDelete` (+ init `PLAYERS.CPP` + set dans `AcceptVoluntaryExitGame`) : **plus utilisé**, conservé pour l'instant.

### Fichiers touchés (état final)

- `Exe Server/PlayerManager.cpp` : save dans `DeletePlayer` ; `AsyncDeletePlayer` restauré.
- `Exe Server/Players.h`, `Exe Server/PLAYERS.CPP`, `Exe Server/TFCMessagesHandler.cpp` : `boSaveOnDelete` (vestige).
- Tests : `tests/test_save_guards.cpp`, `scripts/test_save_persist.sh` (non commités).

**Résultat : réapparition à la position de sortie confirmée en jeu, sur tous les chemins de déconnexion, sans régression connexion/reconnexion.**

---

## 2026-06-21 18:00:00 — FIX client Windows : crash / freeze a la mort du personnage

**Famille : Réseau / Rendu**

Symptôme : après mort en combat, le client Windows se fige ou plante ; dernière trace souvent `UDP recu nbuf=640` sans `DEDUP OK` suivant.

Cause : rafale serveur mort (statut mort, inventaire, megapack) traitée sur le **thread réseau** avec appels UI (`V3_LifeDlg`, `UpdateInventory`) qui entrent en conflit avec le thread de rendu (`RootBoxUI::threadLock`, guard singleton VS2022).

Correctifs :
- `V3_LifeDlg` / `V3_InvDlg` : guard `g_p*Instance` (pattern `V3_StatsDlg`) + pré-construction `V3_LifeDlg` à l'opcode 13.
- Mort : `QueueDeadStatus` / `QueueDeadInfo` depuis le thread réseau, application dans `V3_LifeDlg::Draw`.
- `ViewBackpack2` : données sac mises à jour côté réseau, `UpdateInventory` différé via `g_pendingInventoryRefresh` dans `RootBoxUI::Draw`.
- `NMPacketManager::AnalyzePacket` : borne megapack (évite dépassement buffer sur gros paquets mort).

---

## 2026-06-21 16:30:00 — FIX client Windows : delete/create auth menu + opcode 46/13

**Famille : Réseau / Client Windows**

Symptôme : après delete/create depuis le menu Windows, le serveur logue `paquet 25/26 ignore — joueur NULL` ; feuille de skills absente après création.

**Cause :** le client Windows envoyait opcode **15 + 26 dans le même tick** (sans attendre la réponse 15). Retry auth envoyait **99 + 15/25 ensemble** → session UDP détruite côté serveur.

**Client Windows (`TFCSocket.cpp`, `Packet.cpp`, `packethandling.cpp`)**
- Delete : opcode **15 seul** → réponse OK → **26** ; timeout 8 s × 4 ; re-auth **99 seul** puis 15 après réponse 99 (`g_pendingDeleteAfterAuth`).
- Create : idem pour **25** (`g_pendingCreateAfterAuth`).
- PutPlayerInGame : **ERR=7** → retry (~1 s, max 4), pas erreur fatale.
- FromPreInGameToInGame (46) : `g_Var.inGame=true` uniquement si **code=0** ; retry si code **1/7** (max 8).
- Logs debug `[AUTH]` via `AuthMenuTrace` / `EnterGameTrace`.

---

## 2026-06-21 15:00:00 — FIX opcode 46 picklock + client code=1 (régression entrée monde)

**Famille : Réseau / Serveur**

**Logs typiques :**
```
[PutPlayerInGame] load_character -> code 0
[FromPreInGameToInGame] async: UsePicklock refuse
[Session] joueur inconnu IP …
```

**Cause :** même bug picklock que opcode 13, mais sur **46** (async tentait `UsePicklock` sans que le sync l'ait pris). Le client traitait **code=1 comme succès** → envoi moves/60 alors que le serveur restait `preInGame=1`.

**Serveur**
- `RQFUNC_FromPreInGameToInGame` : `UsePicklock` avant enqueue async ; async ne re-acquiert plus.
- Picklock sync refusé → réponse **46 code=7** (busy).

**Client**
- Succès **46 uniquement si code=0** ; code 1/7 → retry (max 8).

Recompiler client **et** serveur.

---

## 2026-06-21 14:00:00 — FIX régression PutPlayerInGame erreur 7 (picklock Linux)

**Famille : Réseau / Serveur**

**Cause :** sur Linux, `RQFUNC_PutPlayerInGame` libérait le picklock juste après `Call(Async…)` alors que l'async tentait de le reprendre → **erreur 7** (busy). Le `ClearAllReceivedPacketIDs()` sur chaque 13 rendait cette erreur visible même pour un perso existant.

**Serveur**
- Ne plus `UseUnlock` après le `Call` async (aligné Windows) ; l'async libère via `AutoExit`.
- Suppression du second `UsePicklock` en tête de `AsyncRQFUNC_PutPlayerInGame`.

**Client**
- `ClearAllReceivedPacketIDs()` **uniquement** dans `TryAutoEnterWorldAfterCreateList` (flux create), pas depuis la sélection perso.
- Retry automatique si réponse **7** (picklock busy), max 4 fois.

Recompiler client **et** serveur.

---

## 2026-06-21 12:00:00 — Entrée après création + reroll stats : dedup 13, auth 13/26, dés serveur

**Famille : Réseau / Serveur**

**Client Linux (`T4CLoginSession.cpp`)**
- `ClearAllReceivedPacketIDs()` avant chaque envoi `RQ_PutPlayerInGame` (13) — aligné Windows `TFCSocket.cpp` ; évite le rejet de la réponse 13 par la dédup NM après la rafale 25/31/26.

**Serveur Linux (`TFCMessagesHandler.cpp`)**
- `RefreshAuthMenuRegistration()` au début des handlers **13** et **26** — si `key!=0` et menu auth, remet `registred=TRUE` avant `load_character` async.

**Serveur (`Character.cpp`)**
- `roll_stats()` : restauration formule de dés (`rnd(0,4)+6+bRoll*`) à la place du hardcode `SetSTR(20)` … `SetWIS(20)`.

Recompiler client **et** serveur.

---

## 2026-06-21 03:15:00 — FIX régression delete/create : gate registred + retry opcode 99

**Famille : Réseau / Serveur**

**Cause racine identifiée dans les logs :**
```
[AUTH] GetPersonnalPClist (26) registred=1
[PKT] paquet 15 ignore — registred=0   ← bloqué AVANT le handler
```
L'opcode **26** passait avec `bValidRegister=false`, pas le **15** (ni 25). De plus le retry client envoyait opcode **99 + 15 dans le même tick** ; la réponse 99 relançait une **liste persos** au lieu du delete → session détruite (`joueur NULL`).

**Serveur**
- `bValidRegister=false` aussi pour opcodes **15, 25, 31** (comme 26).
- Suppression de `GetPlayerResourceFctForAuthMenu` (migration IP dangereuse).
- `RefreshAuthMenuRegistration()` : si `key!=0` et pas in_game → remet `registred=TRUE`.

**Client**
- Retry delete/create : opcode 99 seul, puis **15/25 après réponse 99** (`g_pendingDeleteAfterAuth`).

**Build serveur** : fix compile — `RefreshAuthMenuRegistration` déplacée avant `RQFUNC_AuthenticateServerVersion` (C++ exige déclaration avant usage).

---

## 2026-06-21 03:00:00 — Suppression perso (opcode 15) : session UDP perdue + timeout client

**Famille : Réseau / Serveur**

Symptôme : `[PKT] paquet 15/26 ignore — joueur NULL (IP …:52672)` — le serveur ne trouvait pas la session auth (match IP **et** port strict `SAME_IP`), souvent après création async réussie côté serveur mais sans réponse client.

**Serveur**
- `GetPlayerResourceFctForAuthMenu` : opcodes menu auth (15/25/26/99/13/31…) — repli même IP si port UDP changé + migration `IPaddrO`.
- Logs + réponses erreur sur `RQ_DeletePlayer` (comme create).

**Client**
- Flux delete : opcode 15 seul, puis 26 après réponse OK (comme Windows).
- Timeout 8 s × 4 retries + re-auth 99 avant retry 2+.
- Log `[PHASE] ->/<- RQ_DeletePlayer (15)`.

---

## 2026-06-21 02:30:00 — Création perso bloquée (opcode 25) : logs serveur + timeout client

**Famille : Réseau / Serveur**

Symptôme : après le questionnaire, l'écran reste sur « attente opcode 25 » ; le client logue `[CharacterCreate] RQ_CreatePlayer` mais **aucune trace serveur** (le handler n'avait pas de `fprintf` — absence de log ≠ paquet absent).

**Client Linux (`T4CLoginSession.cpp`)**
- Log réseau `[PHASE] -> RQ_CreatePlayer (25)` à l'envoi (comme opcode 13/26).
- Timeout 8 s × 4 retries (comme PutPlayerInGame) ; message d'erreur UX si pas de réponse.

**Serveur Linux (`TFCMessagesHandler.cpp`, `PacketManager.cpp`)**
- Log stderr à la réception de opcode 25 + refus (`registred=0`, `in_game`, picklock occupé).
- Réponse opcode 25 avec code erreur (5/6) au lieu de silence.
- Log stderr si paquet 25/15 ignoré (`registred=0` ou joueur NULL).

Recompiler client **et** serveur pour tester contre `172.104.251.211:11677`.

---

## 2026-06-15 22:27:00 — ✅ CONFIRMÉ EN JEU : entrée des mondes réussie (serveur Linux)

**Famille : Réseau / Rendu / Build**

Validation utilisateur après désactivation de `/Zc:threadSafeInit-` : **« c parfait, on a enfin réussi »**. Le client Windows (1.72, build VS2022) entre désormais en jeu avec un personnage existant contre un serveur Linux — le handler opcode 13 construit la totalité des dialogues sans interblocage, jusqu'à l'affichage du monde.

La cause racine de toute la saga d'interblocages (« please wait while loading », `V2PalManager`, `V3_StatsDlg`, dialogues opcode 13, « bloqué au loading à l'entrée des mondes ») était **unique** : le guard d'initialisation des `static` locales thread-safe ajouté par VS2022. La désactivation du flag restaure la sémantique d'origine et résout l'ensemble d'un coup, sans toucher au comportement réseau ni casser la compatibilité serveur Windows.

Restent à traiter (mis de côté à la demande de l'utilisateur) : curseur inactif sur la fenêtre de login, feuille de stats après création de perso, suppression de personnage.

---

## 2026-06-15 22:05:00 — Correction RACINE : désactivation de /Zc:threadSafeInit (annule le wrap threadLock)

**Famille : Build / Réseau / Rendu**

### Le wrap threadLock (16:40) a empiré le symptôme — annulé

Prendre `threadLock` avant la construction a fait bloquer le client **encore plus tôt** : la trace s'arrête désormais sur `13: 46 envoye (proactif)`, c.-à-d. **à l'acquisition de `threadLock`** (avant `13: dlg DisplayHelp`). Donc un autre thread (principal L928 / rendu `RootBoxUI::Draw`) tient `threadLock` et ne le relâche jamais. Vérifications faites : `COMMCallBack` exécute le handler sous `packetLock` (`TFCSocket.cpp` L2792), mais `PacketCenter::SendPacket` ne prend **pas** `packetLock` → pas d'inversion `packetLock`↔`threadLock`. Le détenteur de `threadLock` reste donc coincé sur un **guard d'init statique thread-safe**. Wrap retiré.

### Cause racine commune à TOUTE la saga

`V2PalManager`, `V3_StatsDlg`, puis les ~19 dialogues de l'opcode 13 : à chaque fois, l'interblocage vient du **guard d'initialisation des statiques locales thread-safe** ajouté par VS2022 (`/Zc:threadSafeInit`, activé par défaut depuis VS2015). Ce code (ère VC6) suppose l'ancienne sémantique **non thread-safe** des `static` locales. Le guard crée des cycles guard↔`threadLock` impossibles à l'origine, et explique pourquoi le client fonctionne contre un serveur Windows uniquement par chance de timing.

### Correctif

`T4C Client.vcxproj` (Release **et** Debug) : ajout de `/Zc:threadSafeInit-` aux `AdditionalOptions`. Restaure la sémantique d'origine : plus aucun guard sur les statiques locales → plus aucun interblocage de guard, sur toute la base. Les guards à pointeur global posés sur `V2PalManager`/`V3_StatsDlg` deviennent inoffensifs (ils court-circuitent simplement). Aucun changement de comportement réseau ; impact serveur Windows nul.

---

## 2026-06-15 16:40:00 — Correctif (annulé) du deadlock d'entrée : threadLock pris AVANT la construction des dialogues (opcode 13)

**Famille : Réseau / Rendu**

### Observation décisive (trace utilisateur)

Avec le gating `g_UiInit` sur `RQ_HPchanged`/`RQ_ManaChanged`, l'opcode 13 est enfin reçu et traité (`CB recu type=13 state=2`). Le handler progresse jusqu'à `13: 46 envoye (proactif)` puis **se fige sur la première construction de dialogue** : la dernière trace est `13: dlg DisplayHelp`, juste avant `V3_DisplayHelpDlg::GetInstance()`. Ce n'est donc **pas** spécifique à un dialogue : c'est générique à « construire un dialogue sur le thread réseau pendant que le thread principal/rendu touche RootBoxUI ».

### Cycle d'interblocage identifié

- Le handler opcode 13 met `WantPreGame=false` **tôt** ; le thread principal (`TFCSocket.cpp` L928) entre alors dans le bloc `!WantPreGame` et prend `RootBoxUI::GetLock()` (`threadLock`, un `CRITICAL_SECTION`). Le thread de rendu prend aussi `threadLock` dans `RootBoxUI::Draw`.
- Thread réseau (handler 13) : `GetInstance()` → prise du **guard d'init statique thread-safe** (VS2022 `/Zc:threadSafeInit`) → puis `AddChild` qui **veut `threadLock`**.
- Thread principal/rendu : tient `threadLock` → appelle un `GetInstance()` qui **attend le guard**.
- → Étreinte guard ↔ threadLock.

### Correctif (`Packet.cpp`, handler opcode 13)

Le handler prend désormais `threadLock` **avant** de construire le moindre dialogue (`CAutoLock _entryUiLock( RootBoxUI::GetInstance()->GetLock() )`), pour tout le bloc de construction (dialogues + `ClientInitialize` + `UpdateCharacterSheet` + `SetSideMenuState`), puis le relâche **avant** la loadloop (qui ne doit pas bloquer le rendu). Ainsi, pendant la construction sur le thread réseau, les autres threads bloquent à l'**acquisition de `threadLock`** — sans détenir aucun guard — donc plus de cycle. Les `AddChild` internes reprennent `threadLock` en **récursif** (`CRITICAL_SECTION` réentrant sur le même thread).

Aucun impact serveur Windows : le bloc s'exécute déjà sur le thread réseau ; on ne fait qu'ordonner la prise de verrou pour casser l'inversion.

---

## 2026-06-15 05:06:00 — Correctif définitif : guard à pointeur global sur V3_StatsDlg (pas de pré-chauffage)

**Famille : Réseau / Rendu**

### Régression du pré-chauffage (04:49) — annulée

Pré-construire `V3_StatsDlg`/`V3_InvDlg`/`V3_MainBarDlg` depuis `TFCSocket::MainThread` (après `COMM.State=2`) a **déplacé** le deadlock plus tôt : le client restait bloqué **avant même** d'envoyer `PutPlayerInGame` (sélection de perso). Cause : le constructeur fait `RootBoxUI::AddChild(this)` (prend le verrou RootBoxUI) pendant que le **thread de rendu** tient ce verrou et appelle `V3_StatsDlg::GetInstance()` (HUD) → même étreinte guard-vs-lock, juste avancée. Pré-chauffage + includes retirés.

### Observation décisive

Le handler `RQ_PutPlayerInGame` (opcode 13, `Packet.cpp` L6285-6310) construit **déjà ~19 dialogues via `GetInstance()` sur le thread réseau** (`g_UiInit`) et fonctionne contre un serveur Windows. Donc « construire un dialogue sur le thread réseau » n'est pas le problème. Le problème est **spécifique à `V3_StatsDlg`** : le thread de rendu lit en continu ses stats (HUD), donc une course sur le guard d'init statique se produit dès que le `type=33` (Linux, reçu avant l'opcode 13) déclenche sa **première** construction sur le thread réseau.

### Correctif (calqué sur le fix `V2PalManager`)

`V3_StatsDlg.cpp` : pointeur global `g_pStatsDlgInstance` posé en **première ligne** du constructeur, et renvoyé par `GetInstance()` s'il est non nul :

```cpp
static V3_StatsDlg* g_pStatsDlgInstance = NULL;
// ... ctor : g_pStatsDlgInstance = this;  (1re ligne)
V3_StatsDlg *V3_StatsDlg::GetInstance(void) {
   if (g_pStatsDlgInstance) return g_pStatsDlgInstance;  // appel concurrent pendant la construction
   static V3_StatsDlg instance;
   return &instance;
}
```

Ainsi, pendant que le thread réseau construit l'instance, l'appel concurrent du thread de rendu récupère l'instance-en-cours et **ne bloque pas** sur le guard → le verrou RootBoxUI est relâché → la construction se termine. Aucun déplacement de la construction ; comportement post-construction identique. Binaire redéployé dans `runtime` (05:06).

Si un autre dialogue continûment référencé par le rendu déclenchait le même blocage, le même guard lui serait appliqué (le trace granulaire « 13: … » du handler opcode 13 localiserait précisément l'arrêt).

---

## 2026-06-15 04:49:00 — CAUSE RACINE : deadlock du thread réseau sur un singleton UI (V3_StatsDlg)

**Famille : Réseau / Rendu**

### Preuve (trace `DEDUP OK/DROP`)

La trace décisive a **innocenté la dédup** : l'opcode 13 (90 o) et le megapack (271 o) sont loggés `UDP recu` (thread socket) mais n'apparaissent **ni** en `DEDUP OK` **ni** en `DEDUP DROP`. Le dernier paquet réellement dépilé est `DEDUP OK ushID=10` (le `type=33`, HP). Après lui, le thread `UDPProcessPacketFct` **ne dépile plus rien** alors que le thread socket continue de recevoir.

⇒ Le thread réseau est **bloqué dans le traitement du `type=33`**, donc l'opcode 13 reste dans la file IOCP, jamais traité → blocage au loading.

### Cause racine

`COMMCallBack` appelle `HandlePacket` **directement sur le thread réseau** en state=2. Le handler `RQ_HPchanged` (33) fait `V3_StatsDlg::GetInstance()->IsShown()`.

`V3_StatsDlg::GetInstance()` est un **singleton Meyers** (`static V3_StatsDlg instance;`). Face à un serveur Linux, le `RQ_HPchanged` arrive **avant** la fin de l'entrée (opcode 13), donc la **toute première** construction de ce dialogue se fait **sur le thread réseau**, pendant que le thread principal tient un verrou UI. Le **guard d'initialisation statique thread-safe de VS2022** (`/Zc:threadSafeInit`, absent du build VC6/VC8 d'origine) inter-bloque avec ce verrou → deadlock permanent du thread réseau.

C'est la même famille de bug que le hang « please wait » de `V2PalManager` (corrigé précédemment), mais ici cross-thread plutôt que ré-entrant.

Contre un serveur **Windows**, l'opcode 13 (entrée) est traité **avant** qu'un `RQ_HPchanged` n'arrive : `V3_StatsDlg` est donc construit côté thread principal en premier, et le bug ne se déclenche pas.

### Correctif (client, sûr et non régressif)

`TFCSocket::MainThread` (thread principal), juste après `COMM.State = 2` et **avant** l'envoi de `PutPlayerInGame` (donc avant tout paquet in-game) : pré-construction des dialogues HUD dont les handlers in-game appellent `GetInstance()` :

```cpp
V3_StatsDlg::GetInstance();
V3_InvDlg::GetInstance();
V3_MainBarDlg::GetInstance();
```

Leur `GetInstance()` ultérieur depuis le thread réseau se contente alors de renvoyer l'instance déjà prête — aucune construction concurrente, aucun deadlock. Comportement identique face à un serveur Windows.

Includes ajoutés dans `TFCSocket.cpp` : `V3_StatDlg.h`, `V3_InvDlg.h`. Binaire redéployé dans `runtime` (04:49).

---

## 2026-06-15 04:16:00 — Diagnostic : l'opcode 13 est jeté par la dédup NM client (preuve définitive)

**Famille : Réseau**

### Constat

Malgré `ClearAllReceivedPacketIDs()` à l'envoi de `RQ_PutPlayerInGame` (binaire 03:41 bien déployé dans `runtime`), l'opcode 13 (et ses 5 retries, ID 16/18/20/22/24/26) arrivent physiquement (`UDP recu nbuf=90`) mais n'atteignent **jamais** `COMMCallBack` (aucun `CB recu type=13`).

### Élimination des fausses pistes

- L'opcode 13 `[10 10 …]` = `0x1010` → ushID=16, needAck=1, **pas** de compress/split → chemin normal (`else` → `PostReceivePacket`).
- `AnalyzePacket` appelle **toujours** le callback (pas de drop).
- `PostReceivePacket` ne drop jamais (incrément + IOCP).
- `COMMCallBack` logge **tous** les types sauf 10.
- ⇒ Le **seul** point de rejet possible est la dédup `AlreadyReceivedPacket` (ligne 452).
- Les retries ont des ID **neufs** après le reset, donc d'autres paquets in-game enregistrent ces ID juste avant chaque opcode 13.

### Conséquence

Le plan « megapack-off serveur » est invalidé : l'opcode 13 est **déjà** un datagramme direct (pas de megapack) et est quand même jeté. Le correctif doit viser la dédup côté client.

### Action

Ajout d'une trace décisive dans `UDPProcessPacketFct` (`NMPacketManager.cpp`) :
- `DEDUP OK ushID=… nbuf=…` pour chaque paquet accepté (≥ 12 o).
- `DEDUP DROP ushID=… nbuf=…` pour chaque paquet jeté comme doublon.

Cela révélera l'ID exact des paquets in-game qui « squattent » 16/18/20/… avant l'opcode 13, pour choisir le correctif dédup adéquat. Binaire redéployé dans `runtime` (04:16).

---

## 2026-06-15 00:31:00 — Récupération de perso : client Windows auto-adaptatif au serveur Linux

**Famille : Réseau**

### Contexte

La récupération d'un personnage existant fonctionne avec le client Linux mais échoue avec le client Windows quand le serveur est Linux (feuille de stat / sac incomplets). Le client Linux fonctionne car il **tire** lui-même tout son état post-entrée.

### Diagnostic

`AsyncRQFUNC_PutPlayerInGame` (serveur) diverge selon `#ifdef _WIN32` :

| Branche | Comportement après l'opcode 13 (stats de base) |
|---|---|
| `_WIN32` | **pousse** spontanément le sac (`StartViewBackpack2`), le statut (`PacketStatus`) et l'inview (`packet_inview_units`) |
| Linux (`#else`) | n'envoie **que** l'opcode 13, puis `BeginSession()` — rien d'autre poussé |

Or le client Windows ne tire de lui-même que `RQ_GetSkillList` (39) + `RQ_GetNearItems` (60) (+ handshake 46) et **attend** que le serveur pousse le statut (43) et le sac (154). Face au serveur Linux qui ne pousse rien, ces données n'arrivent jamais.

### Cause racine — interblocage sur l'opcode 46

Le vrai blocage (vu côté serveur : perso bloqué en `preInGame=1 / in_game=0`) est un **interblocage** sur `RQ_FromPreInGameToInGame` (46) :

- Côté **serveur**, c'est `Character::StartAsyncFromPregameToGame()` — déclenché par la **réception du 46** — qui bascule `in_game=TRUE` *puis* pousse sac + statut + **inview** (`packet_inview_units`, `Character.cpp` L15765).
- Côté **client Windows**, le 46 n'est émis que dans le handler `TFCAddObject`, donc **uniquement à réception du push inview**.

→ Le client attend l'inview pour envoyer le 46 ; le serveur attend le 46 pour pousser l'inview. Le serveur **Windows** brise le cycle en poussant l'inview tôt (branche `#ifdef _WIN32` de `AsyncRQFUNC_PutPlayerInGame`) ; le serveur **Linux** ne le fait pas → blocage définitif.

### Correction (côté client uniquement, `Packet.cpp`, handler `RQ_PutPlayerInGame`)

Après l'entrée (opcode 60), le client Windows envoie désormais **proactivement** :
- `RQ_FromPreInGameToInGame` (46) — **débloque la bascule `in_game`** ; le serveur enchaîne alors push sac + statut + inview → entrée en jeu effective.

Plus, en filet de sécurité (UDP non fiable, données déjà poussées par le serveur) :
- `RQ_GetStatus` (43) — statut / jauges
- `RQ_ViewInv` (156) — sac à dos

Requêtes identiques à celles déjà émises nativement par `V3_InvDlg`. Pas de détection d'OS : c'est le modèle « pull » du client Linux, appliqué par défaut.

### Pourquoi ça ne casse pas le serveur Windows

- **46 redondant** : un 46 reçu alors que le joueur est déjà `in_game` retombe sur la branche **no-op** du handler serveur (`TFCMessagesHandler.cpp` L1028 : `if(!boPreInGame || in_game)` → renvoie un 46 vide, ne fait rien).
- **Statut / sac** : ce sont des **instantanés d'état** ; leurs handlers font des affectations (`Player.Hp = …`), pas des cumuls. Reçus une 2e fois, ils réécrivent la même mémoire avec les mêmes valeurs (idempotent).

Aucun effet observable côté serveur Windows ; entrée en jeu + récupération réparées côté serveur Linux. Le client Linux (qui marche) n'est pas touché.

### Reste à faire (non couvert ici)

Curseur inactif sur la fenêtre de login, feuille de stat après création de perso, suppression de personnage.

---

## 2026-06-14 20:13:00 — Lancement build Microsoft Windows (client Vircom 1.72, VS2022)

**Famille : Build**

### Contexte

Reprise compilation native Windows (`client/T4C Client.sln`, toolset **v143**, VS 2022 Community). Échecs récents dus surtout à des **includes manquants/corrompus** et à des ajustements de compatibilité VS2022, pas à du code métier Linux.

### Diagnostic

| Symptôme | Cause |
|---|---|
| `TemplateQueue` inconnu / erreur de syntaxe avant `<` | `templatequeues.h` **corrompu** (auto-include `#include "templatequeues.h"` sans définition de classe) |
| `GetQueuedCompletionStatus` : `void**` vs `LPOVERLAID*` | Port Linux : 4ᵉ argument typé `void*` au lieu de `OVERLAPPED*` sous Windows |
| `std::replace` introuvable (V3_QuestBook*, V3_Chest*, etc.) | `<algorithm>` absent du PCH |
| `mapix.h` introuvable | `MailMan.h` incluait Extended MAPI inutile ; le code n'utilise que Simple MAPI (`mapi.h`) |
| `mapi.h` : `FAR` / `LHANDLE` invalides | `windows.h` manquant avant `mapi.h` dans `MailMan.h` |
| `C3892` assignation via itérateur `std::set` | VS2022 applique la constance des éléments du set (`V3_ChatDlg.cpp`) |
| `LNK1281` SAFESEH | `zlibstat.lib` + `dinput.lib` (DirectX8) pré-VS2012, sans handlers SAFESEH |

### Corrections (fichiers modifiés)

- **`client/T4C Client/templatequeues.h`** — restauration complète de `TemplateQueue` (FIFO Virtual Dreams : `AddToQueue`, `Retreive`, `NbObjects`).
- **`client/T4C Client/pch.h`** — `#include <algorithm>` (pour `std::replace` dans les headers V3).
- **`client/T4C Client/MailMan.h`** — `#include <windows.h>` + `#include <mapi.h>` uniquement (retrait `mapix.h` / `mapiutil.h` / `mapitags.h`).
- **`client/T4C Client/UDP/NMPacketManager.cpp`** — `OVERLAPPED*` sous Windows, `void*` conservé sous `LINUX_PORT`.
- **`client/T4C Client/NewInterface/V3_ChatDlg.cpp`** — mise à jour `std::set<RegChannel>` via erase/insert (2 sites).
- **`client/T4C Client/T4C Client.vcxproj`** — `<ImageHasSafeExceptionHandlers>false</ImageHasSafeExceptionHandlers>` (Release + Debug).

### Commande build

```bat
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x86
msbuild "client\client\T4C Client.sln" /p:Configuration=Release /p:Platform=Win32 /m
```

### Résultat

- **OK** — `client/client/T4C Client/Release/T4C Client.exe` (**3 057 664 octets**, 2026-06-14 20:13).
- Dépendances compilées : `T4CConfig.exe`, `Sound Engine.lib`.
- Aucune installation SDK/outil supplémentaire ; toolchain VS2022 Community existante uniquement.

---

## 2026-06-14 22:15:00 — Fix blocage au lancement (décompression musique SNMC)

**Famille : Build**

### Tests depuis `client/runtime/` (exe + assets)

| Observation | Détail |
|---|---|
| Processus actif | Fenêtre **« The 4th Coming »** atteinte en ~30 s si `T4CGameFile.VSB` déjà présent dans `Documents\Dialsoft\T4CDEV\Sounds\` |
| Cache VSB | Taille **99 744 622** octets = `Game Files\vsbinfo.txt` → pas de re-décompression |
| Blocage perçu | Boîte « décompression musique » : `DecriptVSB()` était appelé **2× d'affilée** (décodage MP3 complet à chaque fois, plusieurs minutes) |
| Risque crash | Si `SNMCI._` / ouverture VSB échoue, le code continuait avec des `FILE*` NULL |

### Correctifs

- **`Audio.cpp`** — `DecriptVSB` : création explicite `Documents\Dialsoft\T4CDEV\Sounds\`, **skip** si VSB existant = taille `VSBInfo.Txt`, `return FALSE` si fichiers SNMC/VSB inaccessibles.
- **`main2.cpp`** — thread `Decryption` : **un seul** appel `DecriptVSB` (plus de double passe).

---

## 2026-06-14 23:45:00 — Fix blocage écran « please wait while loading » (deadlock static VS2022)

**Famille : Build / Client Windows**

### Symptôme

Le client reste figé indéfiniment sur l'écran de chargement initial (`DrawFisrtLoadingText` / `g_LocalString[1]`), jamais de menu de connexion.

### Diagnostic

Traçage temporaire (fichier `startup_trace.log`) dans `FirstInitCreate` / `FirstInitObject` : le gel survient au **tout premier chargement de sprite** (`AttackCursorIcon.LoadSprite("StaticAttackCursor")`), juste après la création des `SoundFX`.

### Cause racine — régression de toolchain (VC6/VC8 → VS2022)

`CV2PalManager::GetInstance()` est un **singleton Meyers** (`static CV2PalManager pmInstance;`). Son **constructeur** appelle `m_plRefPal.LoadPalette("Bright1")` → `Sprite::LoadPalette` → **`CV2PalManager::GetInstance()` à nouveau** (ré-entrance, même thread).

- VC6/VC8 (toolchain d'origine) : **pas** d'initialisation thread-safe des statics locales → la ré-entrance renvoyait l'instance partiellement construite, sans blocage.
- VS2022 : `/Zc:threadSafeInit` **activé par défaut** → le garde d'init du static est détenu par le thread courant ; l'appel ré-entrant du **même thread** attend ce garde → **deadlock permanent**.

C'est le premier `LoadSprite` qui touche le gestionnaire de palettes, d'où le gel exactement à cet endroit.

### Correctif

- **`V2PalManager.cpp`** — l'instance est enregistrée dans un pointeur fichier (`g_pPalManagerInstance = this;`) **au tout début du constructeur** ; `GetInstance()` renvoie ce pointeur immédiatement si présent, **avant** d'atteindre le `static` local. L'appel ré-entrant pendant la construction ne touche donc plus le garde d'init → plus de deadlock. L'initialisation thread-safe reste **activée** pour tous les autres singletons (sûreté côté threads de jeu).
- **`main2.cpp`** — `FirstInitObject` appelé de façon **synchrone** dans `FirstInitCreate` (plus de second thread accédant au device DirectDraw en parallèle du flip) ; boucle d'animation jouée **après** le chargement.

Validé E2E : démarrage jusqu'au menu de connexion OK.

Binaire : `client/client/T4C Client/Release/T4C Client.exe` → déployé dans `client/runtime/T4C Client.exe`.

---

## 2026-06-11 08:00:00 — DIALOGUES PNJ FONCTIONNELS, clic droit = examiner, sous-sols (monde 1), mort/persistance, torche, masques curseurs

**Famille : Serveur / Réseau / Rendu / Gameplay**

### Dialogues PNJ : le texte arrive enfin au client (3 bugs serveur en chaîne)

- **Bug n°1 (LP64, stack corruption)** : `msg->Get((long*)&dword)` écrivait 8 octets sur 64-bit au lieu de 4 → la position `where` de `RQ_DirectedTalk` était écrasée, `FindNearUnit` échouait → `RQ_BreakConversation` au lieu du dialogue. Macro `GET_PACKET_U32` appliquée aux **22 sites** fautifs de `TFCMessagesHandler.cpp`.
- **Bug n°2 (shim CString)** : `GetBuffer(0)` retournait `NULL` pour une chaîne vide (MFC ne retourne **jamais** NULL) → double-clic PNJ (message vide) = `strlen(NULL)` / `CString(NULL)` → le talk mourait silencieusement dans la file async. `Portability.h` : `GetBuffer` retourne toujours un buffer valide + ctor `String(const char*)` accepte NULL.
- **Bug n°3 (override raté, LE bug racine)** : `Character::SendPrivateMessage(CString&)` ne **redéfinissait pas** le virtuel `Unit::SendPrivateMessage(const CString&)` (signature différente → masquage, pas override). Tout appel via `Unit*` (macro `EndTalk`, `InterpretKeyword` des PNJ DB) tombait sur le **corps vide** de `Unit` : la réponse privée du PNJ n'était jamais envoyée. Signature alignée (`const` + `override`).
- **Vérifié E2E** : sonde → marche vers Sublima → DirectedTalk → serveur `InterpretKeyword output=42 octets` → client reçoit `« Hello Traveler , do you want my "spells" ? » (locuteur « Sublima »)` — opcode 27, bulle + chat.
- Diagnostics conservés (`[DirectTalk]`, `[NPCTalk]` stderr, Linux uniquement) ; fix UB varargs `CString` passé à `%s` (noms tronqués/garbage dans les logs).

### Clic gauche = action, clic droit = examiner

- Clic droit sur unité/objet : **identification seule** (RQ_GetUnitName / nom objet 3 s) — plus d'attaque ni d'usage de porte (parité `MouseAction UseOb → Objects.Identify` Windows). Toutes les actions (attaque, parler, portes, ramassage) restent au clic gauche.

### Sous-sols temple LH / crypte cimetière (téléport monde 1)

- **Cause** : au `RQ_TeleportPlayer` vers un autre monde, seule `zoneMap_` était rechargée — le terrain dessiné restait celui du monde 0 (« endroit inconnu »). Désormais `v2Map_.OpenWorld(world)` + `tmiMap_.LoadWorld(world)` rechargées au changement de monde (`hasWorld` ajouté au teleport pour distinguer « monde 0 » de « pas de monde »).
- Monde ≠ 0 : obscurité permanente (donjon), musique donjon déjà OK.

### Mort, drops et persistance

- **HP au respawn = HP avant le coup fatal** (T4C 1.25) : `dwHPBeforeLastBlow` capturé dans `OnHit`, restauré dans `DeathNMS` (plafonné à MaxHP ; 0 → MaxHP). Drops or/XP/items déjà calculés par `DeathPenalties` (vérifié).
- **Déconnexion** : position (X, Y, monde) + stats sauvées par `SaveCharacter` à la déco, rechargées par `LoadCharacter` — le perso réapparaît où il a quitté (vérifié).

### Torche

- Spawn avec torche (StartupItem 40015 `T4CServer.ini`, vérifié). **Double-clic torche** (sac ou bouton Utiliser) : bascule `torchLit_` — halo de lumière élargi (300 → 480) la nuit et dans les donjons + message système.

### Restes session précédente confirmés dans cette passe

- **Carré noir bulle/flèches** : sprites `<name>Mask` décodés et appliqués comme canal alpha (`TransAlphaMask2` Windows) — gant/épée (color-key RGB) + bulle/flèches (masque) OK.
- **Chargement monde long** : suppression du préchargement des 45 frames d'attaque par base (`<Base>A<angle>-a..i`) — décodage **lazy** au 1er swing.
- **Noms PNJ** : requête auto `RQ_GetUnitName` au spawn de chaque unité + retry 3 s (les « PNJ »/« Warrio » restants = unités dont le serveur ne renvoie pas de nom ; les hommes en robe blanche à la banque sont des PNJ d'ambiance 1.72).

**Vérifié** : serveur + client rebuild OK, **93 checks régression OK**, E2E PASS (auth → monde → marche → dialogue PNJ reçu).

---

## 2026-06-11 04:50:00 — Curseurs sans carré noir, marche-vers-PNJ pour parler, vraies animations d'attaque, noms réels des unités

**Famille : Rendu / Réseau**

### Carré noir derrière les curseurs (gant/épée/flèche/bulle) — color-key RGB

- DirectDraw (`V2Sprite.cpp` Windows) pose le color-key sur la **valeur RGB** de `ushTransColor`, pas sur l'index de palette. Notre port ne masquait que l'index : un fond noir indexé 0 restait opaque quand `transIndex` = 255 (noir aussi) → carré noir derrière les sprites de curseur.
- `T4CV2SpriteAtlas` : tout pixel dont le RGB palette == RGB de la couleur transparente devient transparent, **uniquement si** `ushTransparency != 0` (flag header, fidèle à `DDBLT_KEYSRC`). Contour (outline) aligné sur la même règle.

### « You are too far ! » en parlant aux PNJ

- Le serveur refuse `DirectedTalk` si dist² ≥ 120 (~11 tuiles). Comme le client Windows, clic sur un PNJ lointain = **le perso marche vers lui** puis parle automatiquement (réutilise la mécanique pendingAttack, marge anti-désync dist² ≤ 36).

### Animations d'attaque enfin visibles (le son marchait, pas l'anim)

- **Cause racine** : les frames d'attaque générées s'appelaient `GoblinStAtt045-a` — **ce nom n'existe pas**. Le vrai nommage Icon3D (`LoadSprite3D`) est `<Base>A<angle>-<lettre>` (`GoblinA045-a` … `-i`, vérifié dans `V2DataI.did` : 394 bases avec frames A*). `StAtt` n'est que le *compte* de frames.
- `kT4CAttackAnimFrames` 6 → 9 (mode réel des créatures), durée du swing alignée (70 ms × 9).
- **Pose d'attaque puppet** : les 19 calques (`PupChainMailBodyA045-a`…) jouent l'anim d'attaque, repli pose debout par calque manquant.
- **Animation de swing du joueur local** : nouveaux états `playerAttacking_` + `T4CLoginSessionConsumeLocalAttack` — quand 10001/10002 désigne le joueur comme attaquant : whoosh + anim orientée vers le défenseur (avant : rien du tout pour ses propres coups).

### Nom réel des unités au clic (« Dark Fang » au lieu de « PNJ »)

- **RQ_GetUnitName (35)** implémenté : envoi `long id, short x, short y` au clic sur une unité (dédupliqué + cache), réponse parsée (nom, guilde, couleurs — format `Packet.cpp` Windows) dans `g_unitNames`.
- L'étiquette de cible affiche le nom serveur (« Dark Fang », « Mithrand », « Olin Haad guard »…), boîte élargie aux noms longs ; replis précédents conservés (Joueur #id / Pretre / Garde / base sprite).

**Vérifié** : build OK, **93 checks régression OK** (test `attack_sprite_frame_names` mis à jour vers le vrai nommage), **E2E PASS** (38 puppet spawn, 0 erreur serveur).

---

## 2026-06-11 04:20:00 — PNJ visibles (fix LP64 entêtes UDP), crash MD5 serveur, opcode 62, mode attaque Ctrl+C, curseurs contextuels

**Famille : Réseau / Serveur / Rendu**

### Cause racine PNJ invisibles : entêtes UDP « sur le fil » cassés en LP64

- `PacketHeaderSplit` (bitfield `unsigned long`) et `PacketHeaderComp` (2× `unsigned long`) sont **memcpy directement dans le datagramme**. En LP64 (Linux 64 bits) `unsigned long` = 8 octets : les entêtes passaient de 16 → 28 octets, donc tout fragment split = 1008 + 28 = **1036 octets > `PACKET_MAX_SIZE` (1024)** → silencieusement jeté à la réception (`DataLenght > PACKET_MAX_SIZE`). Résultat : tout inview compressé > ~1 Ko (125 unités après le fix spawn PNJ) n'arrivait jamais au client.
- **Fix (client ET serveur, `UDP/NMPacketManager.h`)** : champs forcés en 32 bits (`unsigned int`) = format Win32 d'origine, total entêtes 16 octets. Appels `uncompress()` adaptés (`uLongf` temporaire) des deux côtés.
- **Vérifié E2E** : `[FromPreInGameToInGame] inview push 125 unite(s)` → client reçoit `liste peripherique (127 entrées, 22 PNJ, 104 sol)` ; **Mithrand** (id 1051813), **Araknor** (id 1051794), Samaritans, marchands : `puppet spawn` confirmés côté client. 0 `ValidPacket FAIL`, 0 crash serveur, 92 checks régression OK.

### Serveur (`FinalStEp0/T4Serv3`)

- **Crash MD5 (`RotateLeft` assert `sizeof(x)==4`)** : `MD5Checksum.h/.cpp` portés en `std::uint32_t` (`md5_u32`) — le chat (`RQ_IndirectTalk` 27 → `AnalyseActionWorld`) ne tue plus le serveur.
- **Spawn PNJ Linux** : `MonsterStatSetup` enregistré explicitement dans `TFC_MAIN.cpp` (pas de `DllMain` en lien statique) ; `Unit::InitializeMessagesProcs()` ne purge plus les enregistrements statiques ; log `[BOOT] NPC spawn` (421 PNJ, dont Mithrand app=10011 @ 2961,1093).

### Client (`FinalStEp2/src`)

- **Opcode 62 (`RQ_SendSpellList`)** : envoi de l'octet `1` après l'opcode (aligné T4C.EXE) — plus de `PKT ValidPacket FAIL opcode=62`.
- **Opcode 123 ambigu** : `RQ_SafePlug` (court) vs `UpdateBackpackAfterUse` — garde `isEnd()` après le 1er octet, plus de `TFCPacketException`.
- **Mode attaque Ctrl+C (opcode 198)** : toggle envoyé au serveur (`NMCombatMode`), état resynchronisé par la réponse, bannière « MODE ATTAQUE » ; en mode attaque le clic gauche attaque **tout** PNJ (Olin Haad guard etc., qui ripostent alors).
- **Curseurs contextuels** : gant `glove0000` par défaut, `V3_TalkCursor` sur PNJ amical, **épée oscillante** (`sword0000`…`sword0022`) sur ennemi — ou sur toute unité en mode attaque.

---

## 2026-06-11 03:30:00 — GAMEPLAY INTERACTIF : économie, inventaire actif, chat, sorts/skills, jour/nuit, zones nommées, radar TMI

**Famille : Réseau / Rendu / Build**

Implémentation en une passe des priorités P1→P6 de l'audit #2 (ci-dessous). Le client passe de « lecture seule » à **jouable** : acheter/vendre/s'entraîner, équiper/utiliser/jeter, parler, lancer des sorts, ramasser au clic. Build OK, **92 checks régression OK**, **E2E auth+monde+move PASS** contre le vrai serveur (réception 39/53/62/123 vérifiée en session).

### Réseau (`T4CLoginSession.cpp/.h`) — formats alignés sur `Packet.cpp` / `V3_*Dlg.cpp` Windows

- **Sortants nouveaux** : 41 BuyItems / 56 SellItems / 40 TrainSkills / 55 TeachSkills (coords+ID TalkTo, listes id/qty) ; 21 EquipObject / 22 UnequipObject (mapping `T4CEquipSlot` → codes position `TFCPlayer.h` body=0…sleeves=15) / 12 DepositObject / 85 JunkItems / 23 UseObject(0,0,id) = utiliser un objet du sac sur soi ; 32 CastSpell (self ou cible) ; 42 UseSkill ; **chat** 27 IndirectTalk (avec sessionKey), 28 Shout, 29 Page ; 45 GetTime.
- **Entrants nouveaux** : 39 GetSkillList (vrai stockage `g_skillBook`, fini le stub vide), 62 SendSpellList (`g_spellBook` : mana, cible, coût, niveau, icône), 53 GoldChange (+ champ `gold` dans `T4CPlayerStatus`), 92 UpdateWeight, 123 UpdateBackpackAfterUse (maj qty/charges in place, purge qty=0, resync 156 si inconnu), 45 GetTime (`T4CGameTime`), 29 Page entrant, 63 ServerMessage, 102 InfoMessage.
- **File chat** thread-safe (`T4CChatMessage` Local/Shout/Page/System + couleurs COLORREF) drainée par la boucle monde ; messages système locaux (`PushSystemMessage`).

### UI / interactions (`GameWorldScreen`, `T4CCharacterWindow`, `T4CGameHud`)

- **Boutique interactive** : lignes cliquables avec survol (achat 1 / vente 1 / +1 point / apprendre), bouton X (= BreakConversation 36), affichage or ou points selon mode, refresh sac+statut (+39/62 après train) post-transaction, SFX `Gold sound`.
- **Fenêtre perso interactive** : clic objet du sac = sélection (surbrillance) → boutons **Equiper / Utiliser / Jeter / Détruire** ; clic slot paperdoll = déséquiper ; clic compétence (useMode 1 : méditer…) = 42 ; clic sort = 32 (sur cible sélectionnée sinon sur soi) ; or affiché dans le bandeau. SFX Equip / Generic drop item.
- **Chat in-game** : Entrée ouvre la saisie (SDL_StartTextInput), `texte` = parler, `:texte` = crier, `/Nom texte` = privé, Échap ferme ; historique 8 lignes (fade 14 s) au-dessus de la MainBar, couleurs serveur ; **bulles overhead** 5 s au-dessus des unités qui parlent (et de soi) ; panneau dialogue réservé aux PNJ ; écho local immédiat (dédup de l'écho serveur).
- **Pickup au clic** : clic gauche objet au sol = RQ_GetObject (11) direct (portes = use), SFX `Generic pickup item` ; clic droit = examiner (inchangé).
- **HUD** : **radar TMI réel** (texture 96×96 streaming, 1 px = 1 tuile, échantillonnage `T4CTmiWorldMap::SampleRgb`, cache par tuile joueur) ; **Home** = fiche perso, **Exit** = menu, **12 slots macro** = sorts 1-12 du grimoire ; or + poids live sur la MainBar ; clics sur fond HUD consommés (plus de marche sous les panneaux).
- **Dégâts flottants** : couleur serveur (rouge/vert COLORREF) + texte montant + ombre, sur les unités **et sur soi**.

### Rendu / ambiance

- **Jour/nuit** (LightMap simplifié) : opcode 45 demandé à l'entrée monde + toutes les 60 s ; obscurité selon l'heure T4C (jour 7-17 h, transitions 2 h, nuit 19-5 h) ; voile + **halo de lumière radial** autour du joueur (texture 256×256 générée, plateau 45 %).
- **Zones nommées** : nouveau module `T4CZoneMap` — lit `Game Files/Zone_Map.dat` (header 8 mondes + zlib 3072×3072 ids) et `zonemapinfo.zfn` (XOR `ZONE_key` 16 octets, format `%4d%4d%s`) ; bandeau « Vous entrez dans : … » 4 s + message système au changement de zone (reproduit `Global::ForceDisplayZone`/`ValidMapZonePosition`).

### Restes connus (inchangés depuis l'audit #2)

Trade joueur (111-121), party (78-82), guilde, banque/coffres (106-110), hôtel des ventes, quest book, météo 150, smooth tiles, animations objets de décor, ciblage sort/skill « au sol » (UseObject2 155 / UseSkillPosition), canaux de chat dédiés (48-51/74/75), options persistées, .elng.

---

## 2026-06-11 02:45:00 — AUDIT COMPLET #2 : écart restant client Linux vs client Windows (AUCUNE implémentation)

**Famille : Doc / Décision**

Audit croisé exhaustif (5 passes parallèles : réseau/opcodes, UI, rendu, interactions, audio/système) entre `client/T4C Client` (référence Windows) et `src/` (portage Linux SDL3). Remplace et met à jour la ROADMAP du 2026-06-09 — beaucoup de points y figurant sont **désormais faits** (combat 10001/10002/124/125, mort 47/200/201/202, dialogues PNJ 27/30/50/36, inventaire 19/154 lecture, HUD V3, fenêtre perso, outline alpha au survol, sons portes/combat).

> Constat global : le **socle est solide** (login→monde ~85 %, déplacement, combat de base, dialogue PNJ lecture, portes, mort/résurrection, feuille de perso en lecture). Les gros trous restants : **tout est en lecture seule** (shop, inventaire, sorts, skills) — le joueur ne peut ni acheter, ni équiper, ni lancer un sort — plus **zéro chat**, et les **couches d'ambiance** du rendu (lightmap, météo, smooth tiles, animations). Couverture opcodes : ~30 % complet / ~10 % partiel / ~60 % absent (sur ~130 RQ_* de `PacketTypes.h`).

---

### A. Réseau / opcodes (réf. `Packet.cpp`, `packethandling.cpp` vs `src/network/T4CLoginSession.cpp` switch L2357)

**Géré (complet)** : auth/persos (14/99/26/103/13/25/31/15/20), monde (46/57/16/60/68/69/70/10004), moves 1–8, objets 11/12, combat 10001/10002/124/125, stats 43/33/67/44/37, mort 47/200/201/202, dialogue 27 entrant + 30/50/36 sortants, listes shop/train 41/56/40/55 **entrants**, inventaire 19/154 entrants, use/get 23/11 sortants, attaque 24 sortant, résurrection 202.

**Absent — par sous-système (entrant ←, sortant →) :**

| Sous-système | Opcodes | Impact joueur |
|---|---|---|
| **Transactions PNJ** | → 41/56 confirm achat/vente, → 40/55 confirm train, → 58, 209 | Voit la boutique mais **ne peut ni acheter, ni vendre, ni s'entraîner** |
| **Inventaire actif** | → 21 Equip / 22 Unequip / 12 DepositObject (drop) / 85 JunkItems / 155 UseObject2 (manger/boire/lire/give) ; ← 123 UpdateBackpackAfterUse, 59/122 nom+info item | Inventaire **inutilisable** (lecture seule) |
| **Sorts** | → 32 CastSpell ; ← 64 SpellEffect | **Aucune magie** |
| **Skills actifs** | → 42 UseSkill (méditer, vol…) ; ← 52 points | Compétences **inutilisables** |
| **Chat** | → 27 IndirectTalk (parler !), 28 Shout, 29 Page, 48–51/74/75 canaux, 86/89, 159 smile | **Muet total** — réception 27 OK, émission zéro |
| **Or / poids live** | ← 53 GoldChange, 92 UpdateWeight | Or jamais affiché en continu |
| **Coffres / banque** | ← 106–110 / 220–222, → 107/108, 148/149 | Pas de banque |
| **Trade joueur** | 111–121 | Pas d'échange |
| **Party** | 78–82, 87–88 | Pas de groupe |
| **Guilde** | 126–138, 223–225 | — |
| **Hôtel des ventes** | 140–144 | — |
| **Quest book** | 182–185, 208 | — |
| **Météo** | ← 150 WeatherMsg | — |
| **Divers** | ← 9 GetPlayerPos (resync), 38 ReturnToMenu, 45 GetTime, 63/102/105 messages serveur, 66 MOTD, 83/84 buffs, 95/96 flèches, 151 GetStatus2, 198 AttackMode, 199/100, arène 226–232, GM 189–197 | — |

Note : l'entrée monde Windows demande aussi 75 `GetChatterChannelList` + 154 `ViewBackpack2` (`packethandling.cpp:399-421`) — Linux n'envoie que 39/43/62/156/19. `T4CLoginSessionPollItemNameRequests` est un stub vide (L3151) ; `GetSkillBook/GetSpellBook/GetBankChest` renvoient `{}` (L3101-3116).

---

### B. UI (réf. `NewInterface/` : 49 dialogues `V3_*` vs `src/gui` + `src/game`)

| Élément Windows | Linux | Manque |
|---|---|---|
| `V3_MainBarDlg` saisie chat + canaux CC/DD/WW/CB + boutons ChatLog/QuestBook/Smile/PVP | ❌ | Aucune saisie texte ; menu Esc affiche « Chats (bientot) » |
| `V3_ChatDlg` / `V3_ChatLogDlg` / `SysMsg` | ❌ | Pas d'historique, pas de messages système |
| 12 slots macro MainBar (12×9 pages, `MacroHandler`, `Macros.bin`) | 🟡 | **Rectangles décoratifs** — pas de données, ni clic, ni pagination |
| Boutons Home/Exit MainBar | 🟡 | Sprites rendus, **non cliquables** |
| `V3_TMIDlg` radar | 🟡 | Grille + point ; **vraie carte non rendue** (les données `T4CTmiWorldMap` existent déjà → brancher `SampleRgb`) |
| `V3_RTMapDlg` grande carte (touche W) + brouillard `RTMap.Bin` | ❌ | — |
| `V3_InvDlg` drag&drop sac↔équipement↔coffre | ❌ | Fenêtre perso = lecture seule (`handleClick` : onglets + fermer uniquement) |
| `V3_ItemInfoDlg` tooltip/examine objet | ❌ | Juste étiquette nom 3 s sur objets sol |
| `V3_BuyDlg`/`V3_TrainDlg` interactifs | 🟡 | Panneau texte lecture seule |
| `V3_SpellDlg` cast + ciblage | 🟡 | Liste affichée, pas de cast |
| `V3_OptionsDlg` + `T4CConfigDlg` (résolution, volumes, langue…) | ❌ | Seul toggle audio non persisté |
| `V3_QuestBookP1/P2/P3` (+ `QuestBook.dat` absent du runtime) | ❌ | — |
| `V3_TargetDlg` portrait + skins cible | 🟡 | Cadre + barre HP simplifiés |
| Bulles overhead (`DisplayTextBox` : dialogue au-dessus des têtes, hyperliens cliquables souris) | ❌ | Panel bas écran + touches 1–9 seulement |
| `V3_EffectStatusDlg` buffs/debuffs | ❌ | — |
| `V3_GroupeDlg`/`V3_GuildDlg`/`V3_TradeDlg`/`V3_AHDlg`/`V3_Arene*`/`V3_GMDlg`/`V3_SmileDlg`/`V3_ProfessionDlg`/`V3_ChestDlg` | ❌ | Tout l'écosystème social/économie |
| Portrait puppet + état mort dans `V3_LifeDlg` | ❌ | — |
| Login graphique `V3_LoginBackNew` multi-comptes | 🟡 | Formulaire texte fonctionnel |

---

### C. Rendu (réf. `VisualObjectList.cpp`, `LightMap.cpp`, `Tileset.cpp` vs `src/assets/map`, `src/game`)

**Acquis Linux** : atlas V2 complet (decode, ombre NCK, **outline alpha 1px** = FX_OUTLINE, miroir), carte iso + map2 + overlap, puppet 19 calques avec équipement visible en monde (si 68/70 reçu), survol souris avec contour, curseurs directionnels.

| Système | État | À porter |
|---|---|---|
| **Lightmap / nuit-jour / halo torche** (`LightMap.cpp`, `NTime.cpp`) | ❌ | Le plus gros écart d'ambiance : monde plein jour permanent, pas de halo torches |
| **SmoothTiles** (transitions terrain, `SmoothTiles.bin` — déjà dans runtime) | ❌ | Bords herbe/eau/route nets au lieu de fondus |
| **Animations objets sol** (torches, feux, drapeaux, portes : frames `Faces`) | ❌ | Sprites statiques ; portes = SFX sans changement visuel d'anim |
| **Effets visuels de sorts** (`Spell.cpp`, `VObject3D`) | ❌ | — |
| **Météo** (`weather.cpp` pluie/neige + opcode 150) | ❌ | — |
| **Attaque puppet** (calques `BodyOrderA/AR`) | 🟡 | Anim marche réutilisée pour l'attaque |
| **Variantes `_I` objets sol** (FX_LEFTRIGHTMIRROR, `VisualObjectList.cpp:18340+`) | 🟡 | OK map2, pas pour objets sol serveur |
| **Table objets sol** | 🟡 | ~458 / ~2000 objgroups (`T4CGroundObjectSpriteTable.inc`) |
| **Dégâts flottants** | 🟡 | Texte blanc fixe ; Windows : couleur réseau + montée animée + empilement |
| **Fondus d'écran** (`FadeLevel` au warp/téléport) | ❌ | — |
| **Couche map animée `CompiledViewA`** + fog | ❌ | — |
| **Fullscreen / sélecteur résolution** | ❌ | Fenêtré redimensionnable letterbox uniquement |
| **Intro vidéo** (`NMPlayVideo`) | ❌ | Remplacée par texte défilant (choix assumé ?) |
| Fallbacks visibles restants | — | rectangles 16×10 objets sans sprite, point 8×8 unités, cercles joueur, TMI factice |

---

### D. Interactions joueur (réf. `MouseAction.cpp` grille 0–6 + `Mouse.cpp` curseurs)

| Interaction | Linux | Note |
|---|---|---|
| Déplacement clavier + clic maintenu | ✅ | Pas le pathfinding `pf` Windows |
| Parler PNJ + liens dialogue | ✅ | Liens au clavier 1–9, pas cliquables souris |
| Attaquer (clic droit / A / double-clic) | ✅ | Pas de lock cible (`FreezeID`) ni mode combat auto (Ctrl+C → 198) |
| Use porte/objet (clic, U) | ✅ | — |
| Pickup | 🟡 | Touche **G** seulement ; Windows : clic + marche auto vers l'objet |
| Acheter / vendre / s'entraîner | ❌ | Affichage sans transaction |
| Équiper / déséquiper / drop / junk / use item sac / give | ❌ | — |
| Cast sort / use skill (méditer…) | ❌ | — |
| Chat local / cri / MP / canaux / emotes | ❌ | — |
| Trade joueur, party, guilde | ❌ | — |
| Curseurs contextuels (TALK bulle, porte, main GET) | ❌ | Curseur statique unique + 8 directions |
| Raccourcis Windows (S stats, L chat, G groupe, P sorts, M macros, T trade, O options, H screenshot, W carte) | ❌ | Seul **I** existe |
| Macros F1–F12 personnalisées | ❌ | — |
| Persistance par perso (`Grids.bin`, `Macros.bin`, `CCI.bin`, `RTMap.Bin`, `Options.ini`) | ❌ | Seul `client_config.ini` (ip/port/login) |

---

### E. Audio / système

| Sous-système | État | Note |
|---|---|---|
| Musique de zone | 🟡 | **Hardcodée** dans `T4CGameMusicZone.cpp` ; Windows lit `Zone_Map.dat` (présent dans runtime, non utilisé) |
| Ambiances 2e flux (`Cavern01`/`crypt01`, `dwAmbiantVol`) | ❌ | — |
| SFX combat | 🟡 | Sous-ensemble ; manquent dying/parry/arc/whoosh joueur/Female hit ; table Windows = ~250 `SoundFX[]` |
| Sons UI (open/close boîtes) | ❌ | — |
| Musiques saisonnières (Halloween/Noël — MP3 présents) | ❌ | — |
| Pause musique perte focus, fade in/out | ❌ | — |
| Localisation FR/EN (`.elng` présents dans runtime, non chargés) | ❌ | Textes FR hardcodés |
| Options persistées (volumes, brightness, langue) | ❌ | — |
| Fond launcher PCX (`start*.pcx` chargé via `SDL_LoadBMP` → échec probable) | 🟡 | À vérifier |
| Capture écran, auto-update WebPatch, gamma | ❌ | Basse priorité |
| Assets manquants runtime | — | `QuestBook.dat`, `WDA/` client |

---

### Ordre de bataille conseillé (impact joueur décroissant)

1. **Boucle objet + économie** (débloque le « jouer vraiment ») : transactions shop/train sortantes (41/56/40/55 — parsing entrant déjà fait), equip/unequip/drop/use-item (21/22/12/85/155 + 123/59/122), clics dans `T4CCharacterWindow` et `renderShopPanel`, pickup au clic + marche auto.
2. **Chat minimal** : saisie MainBar + envoi 27 (local), 28 (cri), 29 (MP) + historique/SysMsg — le client est aujourd'hui muet.
3. **Sorts + skills** : cast 32 + ciblage + effet 64, use skill 42 (méditer), liaison aux slots macro de la MainBar (qui deviennent fonctionnels au passage).
4. **Ambiance rendu** : lightmap + jour/nuit + torches, smooth tiles, animations objets sol, dégâts flottants colorés/animés, fondus.
5. **HUD finition** : TMI radar réel (données déjà chargées), or/poids live (53/92), Home/Exit cliquables, buffs (83/84), bulles overhead.
6. **Audio** : `Zone_Map.dat`, 2e flux ambiant, table SFX complète, sons UI.
7. **Systèmes sociaux** : trade, party, coffres/banque, guilde, quest book (+ `QuestBook.dat`), AH.
8. **Système** : options persistées (volumes/résolution/fullscreen/langue), localisation `.elng`, curseurs contextuels, raccourcis clavier complets, macros.

---

## 2026-06-11 00:15:00 — Fenêtres perso modales (stats / compétences / inventaire / équipement / sorts)

**Famille : Rendu**

Nouveau module `T4CCharacterWindow` (shell modal 608×455 centré, `g_OptionDlgW/H` Windows) calqué sur `V3_StatDlg` / `V3_InvDlg` / `V3_SpellDlg` (`client/T4C Client/NewInterface`). Trois onglets cliquables (Personnage / Inventaire / Sorts) + bouton fermer, voile arrière.

- **Personnage** : attributs FOR/END/AGI/INT/SAG (colonne gauche `m_statL`), dérivés CA/Vie/Mana/Poids (droite `m_statR`), XP totale + restant, **liste de compétences** (nom + points, depuis `RQ_GetSkillList` 39).
- **Inventaire + équipement** : **paperdoll** avec 13 emplacements aux rects exacts `V3_InvDlg` (tête, cou, brassards, bagues, corps, cape, armes G/D, gants, ceinture, jambes, pieds), **icônes d'objets** rendues via le sprite monde de l'objgroup (`T4CGroundObjectSpriteName`), grille de sac 8 colonnes avec quantités, bandeau stats + poids.
- **Sorts** : grimoire 10/page (2 colonnes 5+5, slot 222×40 comme `V3_SpellDlg`), nom + coût mana + niveau.

Fond V3 (`V3_StatBack8n`/`V3_InvBack`/`V3_SpellBack`) avec repli panneau plein. Données déjà fournies par le réseau (opcodes 43/39/62/19/154). Ouverture (`I` ou menu) déclenche `RQ_GetStatus` + skills + sorts + backpack + équipement ; clics d'onglets/fermeture consommés (pas de déplacement sous la fenêtre). Remplace l'ancien panneau inventaire textuel.

Limites connues : pas encore les icônes dédiées `InvItemIcons`/`SpellIcons` (on utilise le sprite monde de l'objet + cadre sort), pas de drag-équiper ni de scroll du sac, paperdoll 3D non rendu. Build + tests OK.

---

## 2026-06-10 23:00:00 — Full HUD in-game (calqué client Windows V3_*Dlg)

**Famille : Rendu**

Reproduction du HUD persistant Windows (overlays indépendants sur viewport plein écran, cf. `client/T4C Client/NewInterface`). `T4CGameHud` étendu de la seule barre vie à **trois panneaux** aux positions par défaut Windows (`Global.cpp`) :

- **Life (216×74, haut-droite)** — `V3_LifeBackF` + barres PV/PM (`V3_PVBar`/`V3_PMBar`, rects exacts (46,30)/(46,47) 100×13), nom (38,8), niveau (162,60), valeurs `hp/maxHp`, `mana/maxMana`.
- **MainBar (700×85, bas-centre)** — `V3_MainBarBack` + barre XP (`V3_MainXPBar`, (79,75) 543×7) + `%`, boutons Home/Exit, **12 slots macro** (y=40, x=134+col·36, 34×32).
- **TMI radar (148×152, bas-droite)** — `V3_TMIBack` + zone carte 96×96 + point joueur + coords `x,y,monde` (remonté au-dessus de la MainBar pour éviter le chevauchement).

Barres rendues par clip-largeur (drain gauche→droite, comme `DrawSingleStatusItem` Windows). Chaque sprite a un **fallback** (cadre/remplissage coloré) si absent de l'atlas. Suppression de l'ancienne barre de debug en haut (coords désormais dans le TMI). **Pas de barre d'endurance** sur le HUD (conforme Windows : `End` est un attribut, visible seulement dans la fenêtre Stats). XP en best-effort (`xp`/`xpToNextLevel`, faute de `ExpLastLevel`).

Build + tests OK.

---

## 2026-06-10 22:45:00 — SFX muets (WAV non chargés) + porte qui disparaît (opcode 16 racé)

**Famille : Audio / Réseau**

**1. Aucun SFX (portes, attaques…)** — log `SFX introuvable: Open Wooden Door (Unsupported block alignment)`. Les WAV de `FX Files/` sont du **PCM 8-bit mono 22050 Hz** mais avec un header non conforme (`block align=2` au lieu de 1, byte-rate faux) que `SDL_LoadWAV` (SDL3, strict) rejette. La musique a un header correct → elle marchait. Ajout d'un **chargeur WAV tolérant** (`LoadWavLenient`) qui parse RIFF/fmt/data en ignorant `block align`/`byte rate`, en fallback de `SDL_LoadWAV`, pour SFX **et** musique.

**2. Porte qui disparaît / impossible de ressortir** — cause : l'inview complet (réponse GetNearItems) **et** les bandes périphériques poussées à chaque déplacement (`Character.cpp` → `packet_peripheral_units`) arrivent toutes deux en **opcode 16** (== `RQ_SendPeriphericObjects` == `__EVENT_OBJECT_APPEARED_LIST`), indistinguables. Mon flag `g_expectInviewRefresh` était racé : une bande d'1 entrée consommait le flag et déclenchait un **purge complet du sol** → la porte (et tout) effacée. Correctif : opcode 16 **toujours additif** (jamais de purge) ; retraits via opcode 11 (`BCObjectRemoved`) + cull par distance au rendu + élagage réseau `PruneFarGroundMarkers` (>130 tuiles) pour borner la mémoire.

**Test** : ouvrir une porte → son + reste visible, s'ouvre/ferme sans disparaître ; engager un monstre → sons d'attaque/coup audibles (si l'opcode 10001/10002 arrive, cf. logs).

---

## 2026-06-10 15:30:00 — E2E : fantômes, inview complet, attaques (hit+miss), portes, Mithrand

**Famille : Réseau / Rendu / Serveur**

Cause racine des **sprites fantômes** : l'inview complet (réponse à `GetNearItems` 60, ex. *111 entrées, 7 PNJ, 103 sol*) **et** les bandes périphériques arrivent tous deux en **opcode 16**. Le client traitait tout en incrémental (`replaceGroundList=false`) → ni purge du sol ni retrait des unités disparues. Résultat : objets sol et unités s'accumulent (la « zone carré qui reste » au survol).

**Fix client (`T4CLoginSession.cpp`)**
- Flag `g_expectInviewRefresh` posé à l'envoi de `GetNearItems` (60). Le **prochain** opcode 16 est traité comme **rafraîchissement complet** (purge `g_groundMarkers` + retrait des unités hors inview) ; les bandes suivantes restent incrémentales.
- Réponse 60 **vide** → purge totale (sol + unités) pour ne rien laisser traîner.
- **Opcode 10002 `__EVENT_MISS`** désormais géré (`HandleAttackMiss`) : animation + whoosh de l'attaquant même quand l'attaque rate (très fréquent) — avant, toutes les esquives étaient muettes.
- Logs de diagnostic : `<- attaque (10001)`, `<- attaque ratee (10002)`, `<- retrait unite (11)`.

**Fix client (`GameWorldScreen.cpp`)**
- **Filet anti-fantôme par distance** : unités et objets sol au-delà de `kRemoteUnitCullRange=90` (serveur `_DEFAULT_RANGE=80`) masqués, au cas où un retrait (opcode 11) serait perdu en UDP.
- **Portes** : son `Open/Close Wooden Door` joué quand l'apparence d'un objet sol passe fermée↔ouverte (familles `__OBJGROUP_*_DOOR*`), mécanisme piloté serveur comme le client Windows.
- Boîte de survol/sélection sol alignée sur le marqueur (fini la « zone carré » décalée à côté).

**Fix serveur (`NPCstructure.cpp`)** : si `create_world_unit` échoue pour un PNJ à position fixe (Mithrand banquier derrière comptoir = tuile lue bloquante côté Linux), **retry en `ForceCreation=TRUE`** (log `[BOOT] NPC force-spawn …`). Sur le serveur Windows d'origine ces tuiles sont `__AREA_BUILDING`/`__SAFE_HAVEN`.

**Test** : rester immobile ~0,5 s près d'une foule → log `… N PNJ …` puis vérifier disparition des fantômes en s'éloignant ; engager un monstre → logs `<- attaque (10001/10002)` + sons ; Mithrand visible (sinon vérifier stderr serveur `[BOOT] NPC force-spawn "Mithrand"`).

---

## 2026-06-10 06:20:00 — Mithrand invisible : peripheral ≠ inview (diagnostic + fix)

**Famille : Doc / Réseau**

Logs banque @ ~2954,1090 : `liste peripherique (1 entree(s), 0 PNJ, 1 sol)` à **chaque move** — **normal** : le serveur n'envoie qu'une **bande directionnelle** (`packet_peripheral_units`, opcode 16), pas le rayon 80 autour du joueur. Mithrand @ 2961,1093 est **à côté** du couloir de marche → absent des bandes.

**Fix client** : `GetNearItems` (60) auto **500 ms après arrêt** du déplacement (= `packet_inview_units` rayon 80, comme entrée monde).

**Fix serveur** : log `[BOOT] NPC spawn FAILED` sur stderr si `create_world_unit` échoue au boot (ex. Mithrand sur case bloquée).

**Test** : rester immobile ~0,5 s à la banque → log attendu `[PHASE] -> RQ_GetNearItems (60)` puis `… X PNJ …` avec X ≥ 1 si Mithrand spawn OK.

---

## 2026-06-10 06:15:00 — Playtest escaliers #2 + opcode 57 + Mithrand (suite retours)

**Famille : Doc / Rendu / Réseau**

### Retours utilisateur (session 2026-06-10 ~06:08)

| Sujet | Observation |
|-------|-------------|
| **Esc** | OK — menu, plus de déconnexion directe |
| **Musique zone** | OK — change bien après déplacement |
| **Portes** | Clic G/D ouvre (opcode 47/UseObject) mais **sans anim ni son** — normal, pas encore portés |
| **Escaliers** | Coincé en haut (2987,1078…) puis téléport 2633,1457 ; acks répétés même case ; `GetNearItems vide` sporadique (double 60) |
| **Mithrand** | Toujours invisible à la banque Lighthaven malgré position serveur (2961,1093) |

### Correctifs appliqués

1. **Moves bloqués en haut d'escalier** — retrait du test `isMapTileBlocking` côté client avant envoi move (Windows : serveur seul arbitre).
2. **Ack move même position** — débloque `canMove_` + snap vue (équivalent `pfStopMovement` Windows).
3. **Opcode 57 `RQ_TeleportPlayer`** — handler ajouté (était stub) ; aligné `packethandling.cpp::TeleportPlayer` : break conv, clear unités, `GetNearItems`.
4. **Téléport \|Δ\|>10** — ferme UI + purge `remoteUnits_` avant refresh (comme `Objects.DeleteAll` Windows).
5. **Puppet PNJ sans opcode 70** — fallback `body=425` (WHITEROBE / sprite Cleric) pour banquiers type Mithrand.

### Encore ouvert

- Anim/son ouverture porte (FX + sprite porte map2)
- `GetNearItems (60) vide` en double après téléport (ACK vide harmless, à réduire)
- Vérifier en jeu à la banque : log `[PHASE] <- liste peripherique (… PNJ …)` — si 0 PNJ, problème serveur inview pas client

---

## 2026-06-09 19:30:00 — Retours playtest Lighthaven / Goblin Bridge + correctifs client (Esc, escaliers, musique)

**Famille : Doc / Rendu / Réseau / Serveur**

Session de test manuel compte `test`, perso `azzz` @ spawn Lighthaven (2944,1059). Référence Windows consultée : `V3_MainBarDlg::ManageESCGUIAll` (Esc = cycle HUD), `Packet.cpp` TFCMoveID (|Δ|>10 téléport, |Δ|>1 une tuile), `GameMusic::LoadNewSound` (musique après chaque move joueur).

### Corrigé dans cette passe (client)

| Problème | Cause | Fix |
|----------|-------|-----|
| **Esc quitte le jeu** | `GameWorldScreen` appelait `returnToLogin_` direct | Esc ouvre un **menu** (comme Windows) ; retour persos uniquement via bouton + confirmation ; Alt+F4 / fermeture fenêtre = quitter |
| **Escaliers = tapis roulant** | Client appliquait tout le Δ serveur en une frame | Aligné Windows : \|Δ\|>10 → snap + `GetNearItems` ; \|Δ\|>1 → une tuile max par ack |
| **Musique ne change pas de zone** | `LoadNewSound` seulement à l'entrée monde | Appel après chaque move serveur / téléport (comme `g_GameMusic.LoadNewSound("TFCMoveID")` Windows) |
| **Clic gauche porte = rien** | Seule la sélection, pas `RQ_UseObject` | Clic gauche sur objet sol → `T4CLoginSessionSendUseObject` (Windows : clic droit / touche U) |

### Bugs ouverts (à traiter)

1. **Sprites objets sol incorrects** — ex. bières sur tables affichées comme cartes à jouer. Table `gen_ground_object_sprite_table.py` / mapping `appearance` WDA incomplet ou mauvais ID map2.
2. **Mithrand absent** — serveur le place @ (2961,1093,w0) (`Dll Npcs/Mithrand.cpp`). Soit hors champ de vue, soit pas dans les 102–105 entrées inview, soit sprite PNJ non dessiné. **À confirmer** : tu étais bien dans la banque Lighthaven quand tu as cherché ?
3. **Portes** — peut nécessiter **clic droit** ou touche **U** (pas encore documenté in-game) ; handler `RQ_UseObject` côté serveur à valider.
4. **Pont Goblin — aggro masse sans anim d'attaque** — thread IA PNJ actif, dégâts serveur OK, client ne joue pas anim combat distante (handlers 10001/124 partiels).
5. **Crash serveur à la mort** — `SIGSEGV` dans `CT4CLog::SaveToLog` ← `Character::DeathNMS` (GDB : `strncpy` sur texte vide). Bloquant combat hors Lighthaven.
6. **Fantôme spawn Olin Haad guard** — image figée au point de spawn pendant que l'unité bouge (remote unit pas retirée de l'ancienne tuile ? double entrée inview ?). **À confirmer** : PNJ précis et zone exacte.

### Logs serveur utiles (session)

```
[FromPreInGameToInGame] inview push 102 unite(s) @ 2944,1059 w=0
[GetNearItems] enqueue async pour test
[Move] compte='test' dir=… (longue exploration jusqu'à ~2771,1096)
→ crash thread NPC @ DeathNMS lors du combat pont Goblin
```

### Questions pour toi (si mal expliqué)

- **Escaliers** : crypte de Lighthaven uniquement, ou toutes les maps avec escaliers ?
- **Mithrand** : tu ne le vois pas du tout, ou sprite invisible / mauvaise case ?
- **Portes** : clic gauche, droit, ou les deux testés ?
- **Olin Haad guard** : quel garde exactement (nom à l'écran) et à quel moment le fantôme apparaît ?

---

## 2026-06-09 05:05:00 — ROADMAP : reste à faire pour terminer le portage (audit complet, AUCUNE implémentation)

**Famille : Doc / Décision**

État des lieux factuel après stabilisation auth + déplacement du joueur local. But de cette entrée : recenser **précisément** ce qui manque et **comment** le faire, sans toucher au code. Chaque point cite les fichiers/lignes pertinents (client = `FinalStEp2/src`, serveur = `FinalStEp0/T4Serv3`, référence Windows = `FinalStEp2/client/T4C Client`).

> Repère majeur : le **client peut bouger**, mais quasiment toute la couche « monde vivant » (PNJ animés, dialogues, inventaire, combat, portes) est soit **désactivée côté serveur**, soit **non portée côté client**. Rien n'est cassé « par accident » : ce sont des morceaux pas encore activés/portés.

---

### 1. PNJ — présence & déplacement (cause racine identifiée)

**Présence (placement statique) : OK.** Les PNJ sont placés au boot via `NPCstructure::OnServerInitialisation()` (`Exe Server/NPCstructure.cpp:133-176`) → `WorldMap::create_world_unit(U_NPC, …, npc.InitialPos, …)`. Les classes C++ par région sont compilées en monolithe sur Linux (`CMakeLists.txt:109-120`, `LoadDLLList()` est un no-op `TFC_MAIN.cpp:1133-1138`).

**Déplacement / IA : DÉSACTIVÉ sur Linux.** Cause exacte :

```5426:5432:/mnt/debian/home/tom/Public/FinalStEp0/T4Serv3/Exe Server/main.cpp
         NPCMain::GetInstance();
#ifndef _WIN32
         /* Final_Step : pas de NPCMain::Create() — WDAInitNPC a déjà chargé les PNJ */
#else
         NPCMain::GetInstance().Create();
#endif
```

`NPCMain::Create()` (`NPC Thread.cpp:1158-1161`) est ce qui **démarre le thread IA** (`NPCThreadFunc`, `NPC Thread.cpp:180-315` : wander L766+, fighting L464+, `MoveUnit(...)` + broadcast L656-661). Sur Linux ce thread **ne tourne pas** → les PNJ sont placés mais **statiques**, ne se battent pas, ne parlent pas spontanément, ne sont pas dépop/repop.

**Comment faire (dans l'ordre, c'est le gros morceau) :**
1. Ré-activer le thread IA sur Linux : retirer le `#ifndef _WIN32` ci-dessus (ou écrire un driver de tick équivalent) puis **valider la sûreté threads**. C'est précisément ici que la dernière IA avait « tout cassé » : ce thread martèle la liste des unités/joueurs en parallèle du thread UDP. Les fix récents (`CRITICAL_SECTION` → `std::recursive_mutex`, garde `GetGodFlags`/`PacketPuppetInfo` NULL) sont des prérequis ; il faudra re-vérifier chaque verrou pris par `NPCThreadFunc` et `SubmitNearUnit`.
2. Vérifier que `MoveUnit()` (`Unit.cpp:987+`) émet bien le broadcast move (opcodes 1-8) que le client sait déjà appliquer aux unités distantes (cf. §2).
3. Couvrir par E2E : un PNJ « wander » connu (ex. un rat près du spawn 2944,1059) doit générer des opcodes move reçus par le probe → assert non-régression + pas de crash.

**PNJ nommés demandés — vérifiés dans l'arbre 1.72 :**
| PNJ | Présent ? | Référence |
|-----|-----------|-----------|
| Samaritain de Lighthaven | **Oui** | `Dll Npcs Arakas/LighthavenSamaritan.cpp` — `[2901]` @ (2931,1072, w0) |
| Marsac Cred | **Oui** | `Dll Npcs WindHowl/MarsacCred.cpp:23` — `[3016]` @ (1608,1253, w0) |
| Uranos | **Oui** | `Dll Npcs/Uranos.cpp:21` — `[3060]` @ (2975,950, w0) |
| Araknor | **Oui** | `Dll Npcs/Araknor.cpp:21` — `[3029]` |
| Lothan | **Oui** | `Dll Npcs/Lothan.cpp:21` — `[3047]` |
| Wellan, Herman, Valerius, Kinaata, Aspectra | **Introuvables** | aucun match `.cpp/.h/.lng` dans T4Serv3 |

→ Les 5 introuvables : soit propres à une **autre version** que cette 1.72, soit **renommés** (les noms FR/EN diffèrent comme tu le notes), soit définis **en WDA** (pas en C++) et donc absents tant que `NPCs.WDA` n'est pas chargé (cf. ci-dessous). **À faire :** confirmer leur source (WDA vs version) avant de conclure qu'ils manquent vraiment.

---

### 2. PNJ visibles côté client (`GetNearItems` revient vide)

**Symptôme observé** dans les logs : `[PHASE] RQ_GetNearItems (60) vide` → aucune unité distante instanciée client. Le **pipeline client existe pourtant** : `ParsePeripheralObjectList` (`src/network/T4CLoginSession.cpp:950-988`) instancie les unités d'`appearance` 10001-29999 via `QueueRemoteSpawn` → `remoteUnits_` → `updateRemoteUnitMotion` (`src/game/GameWorldScreen.cpp:915-939`) interpole le déplacement avec anim de marche. Les opcodes move distants (1-8) sont gérés par `HandleRemoteMove`.

**Donc le « vide » vient du serveur** : sans thread IA (§1) et avec un inview potentiellement incomplet, le serveur n'envoie pas les PNJ. **Bug client secondaire à corriger en parallèle :** `IsLocalPlayerMoveTarget` (`T4CLoginSession.cpp:430-438`, utilisé L1028-1033) traite **tout** move vers une case adjacente au joueur comme un ack local — un PNJ qui bouge à côté de soi peut être « avalé ». À durcir (comparer l'`unitId` au lieu de la seule position).

**Comment faire :** d'abord §1 (serveur envoie), puis vérifier le push opcode 16 (`RQ_SendPeriphericObjects`) à l'entrée monde, puis corriger `IsLocalPlayerMoveTarget`.

---

### 3. Quêtes — fonctionneront-elles ?

**Logique de dialogue/quête : compilée et présente**, deux moteurs :
- **DSL C++ (macros)** : `Exe Server/NPCmacroScriptLng.h` (`InitTalk/Begin/Conversation/GiveFlag/…`) expansé dans chaque `OnTalk()` (ex. `LighthavenSamaritan.cpp:35-48`). Compilé sur Linux (fix macro `.##X`→`.X` déjà fait). TODO résiduels header L67-69, L535 (`CurePlayer/BlessPlayer/CureDisease`).
- **Interpréteur WDA** (PNJ d'éditeur) : `SimpleNPC.cpp` (`InterpretContainer`, `OnTalk` L601+) + `Wda/Npc/*`. Dépend de `NPCs.WDA`/`DialsoftV2NPCs.WDA`.
- **Flags de quête** : `QuestFlagsListing.h` (ex. `__FLAG_MARSAC_CRED` L30, `__FLAG_ARAKNOR` L14).

**Bloquants Linux pour que les quêtes tournent vraiment :**
1. **Assets WDA manquants** dans `runtime/WDA/` : présents seulement `DialsoftV2Edit.WDA`, `DialsoftV2NPCs.WDA`. **Manquent** `NPCs.WDA`, `T4C Worlds.WDA`, `T4C Edit.WDA` (cf. `runtime/WDA/README.md`). Sans `T4C Worlds.WDA`, `create_world_unit/GetWorld()` peut échouer → placement PNJ KO.
2. **QuestBook (journal de quêtes client)** : `QuestBook.cpp:32` chemin codé en dur `WDA\\QuestBook.dat` (pas de `#ifndef _WIN32`), et fichier absent du runtime. Idem à vérifier dans `Professions.cpp`, `EventsMaster.cpp`.
3. **`CWDAUtils` désactivé sur Linux** (`TFCInit.cpp:1508-1511`, contournement crash LP64) → tables summon/skins/quêtes alternatives non chargées. À porter (LP64) ou remplacer.

**Comment faire :** (a) déployer le pack WDA LP64 complet ; (b) corriger les chemins `WDA\\` → séparateur portable ; (c) porter/rétablir `CWDAUtils` ; (d) seulement ensuite tester une quête bout-en-bout (ex. « tuer 15 rats » du Samaritain) — qui dépend aussi de §1 (rats mobiles) et §4 (combat).

---

### 4. Combat (« bastons »)

**Côté client : envoi seulement.** `RQ_Attack` (24) part bien (`T4CLoginSessionSendAttack`, `T4CLoginSession.cpp:2207-2219`), avec auto-approche jusqu'à portée melee (`GameWorldScreen.cpp:600-654`). Barre de vie cible via opcode 69.

**Manque côté client (retours de combat) :**
| Effet | Opcode Windows | État Linux |
|-------|----------------|------------|
| Résultat d'attaque / reposition | **10001** | non géré (tombe en `default`) |
| Dégâts affichés | **124** `RQ_DamageUnit` | non géré |
| Mort | **47** `RQ_YouDied` | non géré |
| Anim d'attaque distante | 10001 + `Objects.PlAttack` | `Attack` event existe (`T4CLoginSession.h:296`) mais **jamais émis** |
| Sorts | 32/64 | non implémenté |

**Comment faire :** ajouter au switch `T4CLoginSessionHandlePacket` (`T4CLoginSession.cpp:1429-1544`) les handlers 10001/124/47, émettre `T4CRemoteUnitEventKind::Attack` pour déclencher l'anim déjà câblée, et un écran/feedback de mort. Référence : `client/T4C Client/Packet.cpp`.

---

### 5. Objets au sol qui « ressemblent à des cartes à jouer » + portes

**Deux phénomènes distincts :**
1. **Vraies cartes = sprites `Misc`/`Misc1` authentiques** (objgroups 27-59, 126-129) — identiques au client Windows (`VisualObjectList.cpp:5186-5289`). Ce n'est PAS un bug de rendu, c'est le vrai sprite T4C de ces objets. Rien à « corriger » sauf si l'objet est mal classé.
2. **Rectangle de secours (fallback)** : quand l'`appearance` n'est pas dans la table client ou que le sprite manque, `drawGroundObjectMarker` (`src/game/GameWorldScreen.cpp:705-728`) dessine un **rectangle plein 16×10**. La table `T4CGroundObjectSpriteTable.inc` ne couvre que **458 / ~2000+** objgroups (`__OBJGROUP_LASTGROUP=2000`, `client/.../Apparence.h:1200`). **Les objgroups 1-26 (dont les portes `RockDoor*`) sont totalement absents** → portes/objets bas-ID rendus en rectangle.

**Portes — deux systèmes, aucun à parité :**
- **Portes map2 (tuiles murales)** : rendues (`T4CMapObjectTable.inc` contient `*Door*`), détectées `IsInteractiveMapObject` (`T4CMapObjectSprites.cpp:94-109`), **ouverture seulement au clic-DROIT** via `tryUseAtScreen` → `RQ_UseObject(23)` (`GameWorldScreen.cpp:427-447,537-541`). Pas d'anim d'ouverture, pas de refresh d'état serveur, picking par tuile rectangulaire (pas diamant iso) → on rate souvent la tuile.
- **Portes objets-unités serveur** : passent par le pipeline objets-sol → table incomplète (15/16/18-26 manquants ; 130/131 mal mappés sur des sprites de mur) → rendu faux ou rectangle.

**Comment faire (portes + objets sol) :**
1. **Régénérer `T4CGroundObjectSpriteTable.inc`** à partir de TOUTES les liaisons `CreateSprite`/`GetVSFObject` de `client/T4C Client/VisualObjectList.cpp` (incl. 1-26, familles portes 769+, jeu complet `64kItemGr*`). **Attention :** ne pas ingérer les lignes `CreateSprite` **commentées** (origine du mauvais mapping 131/132). Ajouter le script générateur au repo (`scripts/`, aujourd'hui absent).
2. **Porter les cas spéciaux de dessin** Windows (`VisualObjectList.cpp:18409-18450+`) : variantes miroir `_I` (`FX_LEFTRIGHTMIRROR`), frames animées (`Faces`), `Overlap` — pas seulement `DrawSpriteN` statique.
3. **Précharger** les sprites sol quand `RQ_GetNearItems` renvoie des marqueurs (le préchargement actuel ne couvre que les tuiles map du viewport, `GameWorldScreen.cpp:173-174`).
4. **Picking type `GridID`** (masque alpha du sprite ou pick diamant iso) au lieu du `screenToWorldTile` rectangulaire ; **clic-gauche** + curseur « USE » à parité `MouseAction.cpp:417-466`.
5. **Anim d'ouverture/fermeture** map2 + état serveur (CHANGELOG L571 « portes animées » déjà ouvert).

---

### 6. Aura de survol vs « sale carré »

**Actuel : rectangles `SDL_RenderRect` à taille fixe**, et **aucun survol souris** (`GameWorldScreen::HandleEvent` n'a pas de `SDL_EVENT_MOUSE_MOTION` ; sélection au clic seulement). Boîtes : objet sol 36×40 (`GameWorldScreen.cpp:1092-1096`), unité 40×68 (L1115-1119). C'est le « carré moche » — un stub explicite de `V3_TargetDlg`.

**Windows : contour par masque alpha (per-pixel), pas un rectangle.** Quand `Object->ID == TargetID`, `boOutline=TRUE` → `FX_OUTLINE` (`VisualObjectList.cpp:15826-15865`), rendu par `CV2Sprite::TransAlphaMaskOutline` (`V2Sprite.cpp:4192+`, `FX_OUTLINE` défini `V2Sprite.h:26`). `TargetID` est mis à jour à **chaque mouvement souris** (`MouseAction.cpp:417-423`).

**Comment faire :**
1. Ajouter `SDL_EVENT_MOUSE_MOTION` dans `HandleEvent` et tracker `hoverUnitId_`/`hoverGroundId_` via les `pickUnitAtScreen`/`pickGroundObjectAtScreen` existants.
2. Porter `TransAlphaMaskOutline` : le décodeur V2 expose déjà la transparence par pixel (`T4CV2SpriteAtlas.cpp:553-577`) ; dilater de 1px les pixels non-transparents en blanc et dessiner sous le sprite. Ajouter une méthode `TryDrawSpriteOutline(...)`.
3. Remplacer/atténuer les `SDL_RenderRect` (L1088-1119) par cette passe contour quand `id == sélection/survol`. Prévoir aussi le cas puppets 3D (`T4CPuppetTryDraw`).

---

### 7. Dialogues PNJ (« parler aux PNJ »)

**Entièrement absent côté client.** Aucune fenêtre dialogue/shop/train ; le clic sur un PNJ ne fait que **sélectionner**, et double-clic/clic-droit/`A` = **attaquer** (`GameWorldScreen.cpp:518-585`). Aucun envoi de `RQ_DirectedTalk(30)`/`RQ_IndirectTalk(27)`, aucun handler 27/30/36/40/41/56. `ConsumeNetworkSuccessDialog` est un stub `@deprecated` (`T4CLoginSession.h:345`, `.cpp:2372-2374`).

**Comment faire :** porter le flux Windows (`client/T4C Client/MouseAction.cpp` envoie `RQ_DirectedTalk` au clic PNJ ; `Packet.cpp` gère 27/30/36/40/41/56) : (a) clic PNJ → `RQ_DirectedTalk`, (b) handlers réseau des opcodes de conversation/boutique/entraînement, (c) UI dialogue (Talk/Buy/Sell/Train) — référence `NewInterface/V3_*Dlg`. **Prérequis quêtes** (§3) : sans dialogue, aucune quête ne se déclenche.

---

### 8. Inventaire / équipement / apparition initiale (torches + habits)

**Apparition perso (apparence) : partielle.** À l'entrée monde, `ApplyRaceToPuppetAppearance` (`T4CLoginSession.cpp:284-297`) + `RQ_PuppetInformation(68)` ; la réponse 68 (`HandlePuppetInformation` L1042-1107) habille le puppet. Sans 68, fallback **body=285 / legs=284** (`T4CPuppet.cpp:97-102`) = habits de base, **pas de torches**. Les torches viendraient de `weaponR/weaponL` du puppet 68 — **aucune logique torche côté client**.

**Inventaire : structures définies, parsing/état/rendu absents.** `T4CPlayerInventory.h` a tous les structs (sac, équipement, livres de skills/sorts) mais : `GetBackpack`/`GetEquipment` renvoient `{}` (`T4CLoginSession.cpp:2119-2147`), `ConsumeInventoryUpdate` toujours `false`, **aucune UI inventaire**. Opcodes entrants non gérés : **154** `RQ_ViewBackpack2`, **19** `RQ_ViewEquiped`, 106/109/110 (coffres/banque), 59 (nom d'item). `finishEnterWorld` ne demande **que** near-items + status (`GameWorldScreen.cpp:114-125`), pas ViewInv/ViewEquip.

**Côté serveur — kit de départ : code OK, config VIDE.** `CreateCharacter()` (`Character.cpp:2977+`) donne toujours la Gemme du Destin puis boucle sur `vStartupItems` lue depuis `T4CServer.ini [.../characters] StartupItem1..N` (`AutoConfigUpdate` `Character.cpp:216-240`). Or **`StartupItem1=` est vide** dans tous les `.ini` (`runtime/T4CServer.ini:9`, build, docker, HallRes). IDs dispo : `__OBJ_TORCH=40015` (`DynObjListing.h:49`), habits IDs 155-156 (`t4c_eng.lng`). → **aucun item de départ donné** = problème de config, pas de code.

**Comment faire :**
1. **Serveur :** renseigner `StartupItem1..N` dans `T4CServer.ini` avec les noms summon officiels (torche + habits de départ).
2. **Client :** appeler `RQ_ViewEquiped(19)` et `RQ_ViewInv(156)` dans `finishEnterWorld` ; parser 154/19 → remplir l'état session ; porter une UI inventaire/équipement (réf. `client/T4C Client/NewInterface/V3_InvDlg.cpp`) ; afficher la torche équipée (weaponR/L du puppet 68).

---

### 9. HUD

**Présent :** barres HP/mana + nom + niveau (`T4CGameHud.cpp:47-106`, stats via opcodes 33/43/44/37/67), barre debug coords, cadre cible + barre de vie cible (`GameWorldScreen.cpp:730-748`).
**Manque :** chat (opcodes 27/28/34/48/49/51), barres d'action/raccourcis (sorts/skills, `RQ_CastSpell 32`/`RQ_UseSkill 42`), barre XP / or / poids (XP parsé mais non rendu ; 53/92 non gérés), écran de mort.
**Comment faire :** compléter `T4CGameHud` + handlers réseau correspondants ; réf. `client/T4C Client/NewInterface/`.

---

### 10. Sauvegarde du personnage (déco : position, inventaire, etc.)

**Implémenté côté serveur (pas de stub).** `SaveCharacter()` (`Character.cpp:1010-1735`) persiste position (`PlayingCharacters.wlX/Y/World` ~L1591), inventaire (`PlayerItems` L1067-1274), flags/boosts/effets. Déclencheurs : déconnexion `AsyncDeletePlayer` → `SaveCharacter` (`PlayerManager.cpp:1613-1616`), autosave périodique `QueryNextSave` (`PLAYERS.CPP:1106-1117`), reroll. Garde-fou `boCanSave` (mis `TRUE` à l'entrée monde, `Character.cpp:15677`). ODBC porté (curseur `SQL_CUR_USE_DRIVER`, `ODBCMage.cpp:180-184`).
**À vérifier (pas réécrire) :** que la save async (`ODBCCharAsyncSave.SendBatchRequest`, `SaveCharacter` retourne toujours FALSE L1735 = pattern batch) aboutit bien sur déco Linux, DSN MariaDB configuré. **Test E2E à ajouter :** entrer, bouger, se déco proprement, vérifier que la reco recharge la **nouvelle** position (et non le spawn).

---

### Ordre de bataille conseillé (dépendances)

1. **Serveur §1** (ré-activer thread IA + sûreté threads) — débloque PNJ mobiles, combat, quêtes. **Le plus risqué** (régressions threading) → E2E obligatoire.
2. **Assets WDA §3** (déployer pack LP64 + fixer chemins) — débloque placement complet + quêtes WDA + journal.
3. **Client §2** (corriger `IsLocalPlayerMoveTarget`) + **§4** (handlers combat) — rendre PNJ/combat visibles.
4. **Client §7** (dialogues) + **serveur §8** (StartupItem) — quêtes jouables + kit de départ.
5. **Client §5/§6** (table sprites complète + aura contour) — finition visuelle (cartes/portes/aura).
6. **Client §8 inventaire / §9 HUD** — UI complète.
7. **§10** : E2E de persistance position/inventaire.

> Détail serveur complémentaire : voir `FinalStEp0/T4Serv3/PORT_LINUX_CHANGELOG.md` (même date) pour le volet serveur (thread IA, WDA, CWDAUtils, StartupItem, chemins `WDA\\`).

---

## 2026-06-09 04:50:00 — Déplacement RÉTABLI + crash reco corrigé (clé 32 bits LP64 + garde GetNearItems)

**Famille : Réseau / Serveur / Test**

Suite (et fin) de la chasse « le perso ne bouge plus ». Trois causes serveur successives, toutes côté `T4Serv3` (détails dans `PORT_LINUX_CHANGELOG`) :

1. **Deadlock** : `CRITICAL_SECTION` émulé par un `std::mutex` non-récursif (Win32 = récursif) → self-deadlock du verrou liste joueurs lors d'un nettoyage de session → thread de réception figé (plus aucun paquet traité). Fix : `std::recursive_mutex`.
2. **Joueur supprimé juste après l'entrée monde** : la **clé de validation** T4C est 32 bits sur le fil, mais le serveur la comparait en `long` **64 bits** (Linux LP64) avec bits hauts parasites (`0xaa3059fe297e38ff` vs `0x297e38ff` reçu) → `Invalid Validation Key` → `dwKickoutTime` armé → `IsIdle()` → `DeletePlayer` du joueur vivant → moves rejetés (`joueur inconnu`). Fix : bornage 32 bits dans `SetKeyCode`/`GetKeyCode`.
3. **Crash à la reconnexion** : `SIGSEGV` dans `GetNearItems`/`packet_inview_units` → `Unit::QueryInvisible::SendPacketTo` déréférençait le `Players*` (NULL) d'une unité PC orpheline. Fix : garde `pl != NULL`.

**Résultat** : connexion → monde → **le perso avance** ; logout → reconnexion → monde → **avance** sans crash.

**Garde anti-régression (E2E)** : le probe `t4c_e2e_auth_probe` envoie désormais `RQ_GetNearItems` (60) à chaque entrée monde, donc le scénario `T4C_E2E_WORLD_RECONNECT` + `T4C_E2E_MOVE_TEST` exerce explicitement le chemin du crash orphelin. À lancer via `scripts/run_reconnect_sim.sh`.

---

## 2026-06-09 02:55:00 — Vraie cause « perso n'avance plus » : CRASH serveur opcode 68 après reconnexion + e2e move

**Famille : Réseau / Serveur / Test**

**Symptôme** : `connexion → monde → quitter → revenir → monde` puis flèches : `[MOVE] -> dir=N (udp_open=1)` côté client (envoi OK, aucun `sendto FAIL`), mais **aucune** confirmation serveur, perso tourne sans avancer.

**Cause racine (côté serveur, voir `PORT_LINUX_CHANGELOG`)** : après la ré-entrée monde, le client envoie l'opcode **68 `RQ_PuppetInformation`**. `Character::PacketPuppetInfo` déréférence un `Players*` NULL (`GetPlayer()` NULL après reconnexion) → **segfault serveur**. Le serveur mort, les `sendto` move réussissent sur localhost mais ne sont jamais traités ⇒ illusion « envoyé mais jamais reçu ». Fix = garde `pl != NULL` côté serveur.

**Outil de repro/garde (ce qui a permis de trancher sans GUI)** : sonde headless `t4c_e2e_auth_probe` étendue avec :
- `T4C_E2E_MOVE_TEST=1` : en jeu, envoie des moves et **exige** une confirmation serveur (`T4CLoginSessionConsumeLocalPlayerMove`).
- `T4C_E2E_WORLD_RECONNECT=1` + `MOVE_TEST` : monde → move → retour login → reconnexion → monde → move (scénario qui reproduit le crash).
- Refactor en helpers (`WaitCharacterList`, `EnterWorld`, `MoveTest`) — comportements des flags existants préservés.

`scripts/run_reconnect_sim.sh` : 2 nouveaux cas (move simple + anti-crash opcode 68) + détection « serveur vivant » en fin de scénario. **Tous PASS.**

**Nettoyage anti-spam** : suppression des logs de diagnostic temporaires côté client (`[MOVE] … udp_open`, `[UDP] sendto FAIL`) et serveur (voir PORT_LINUX_CHANGELOG).

---

## 2026-06-09 01:55:00 — Keep-alive manquant : moves jetés après inactivité

**Famille : Réseau / Client**

**Symptôme** : entrée monde OK, `[MOVE] -> dir=N` côté client, mais **aucun** `[PKT] move rx` serveur, aucun ack opcode 1, perso tourne sans avancer — surtout après quelques secondes d'observation avant d'appuyer.

**Cause** : le thread keep-alive Windows (`COMM.StartSendAlive()` → `SendAliveDataThread`, opcode 10 toutes les ~3 s) n'était démarré **que** dans l'ancien chemin `TFCSocket.cpp`. Le client Linux (SDL3 / `T4CCommBridge`) ne l'appelait jamais. Sans keep-alive, la connexion UDP NM serveur (`PACKET_BACKLOG_TIMEOUT = 15 s`) **expire** pendant l'inactivité ; ensuite `AnalyzePacket` fait `GetConnection(...,false)` → NULL → paquet marqué « déjà reçu » et **jeté avant `PacketInterpret`** (donc pas de log move, pas d'ack). Les opcodes 39/46/60 passaient car envoyés dans la seconde suivant l'entrée monde.

**Fix client** (`T4CLoginSession.cpp`) : `SendPacket` mémorise `g_lastClientSendTick` ; `T4CLoginSessionPollBackgroundTasks` renvoie l'opcode **10 (`RQ_Ack`)** après ~3 s sans envoi tant que la session UDP est ouverte (hors logout). Reproduit `SendAliveDataThread` sans thread Win32 ni `TerminateThread`.

**Test attendu** : entrer monde, **attendre 20-30 s**, puis flèches → serveur `[PKT] move rx opcode=N` + `[Move] rx dir=N` + `[Move] compte='test' dir=N pos …` ; client `[PHASE] <- move ack @ x,y` et le perso avance.

---

## 2026-06-08 23:50:00 — Client stale + logs serveur Windows sur Linux

**Famille : Build / Serveur / Client**

**Symptôme** : `./scripts/run_client.sh` utilisait `build/linux/t4c_client` du **08/06 04:11** (pas le source à jour) — logs `[PHASE] -> 60 + skill list`, cooldown reconnexion **12 s**, double opcode 60 → stall UDP serveur, moves `[MOVE]` sans `[PKT] move rx`.

**Cause logs build** : `main.cpp` formatait le sous-dossier session avec `\\` sur Linux → répertoires littéraux `logs\2026_06_03__…\` et milliers de fichiers plats dans `build/logs/`.

**Fix serveur** : timestamp session `…/` sous Linux ; `EXTBD/` au lieu de `EXTBD\\` (`main.cpp`, `Character.cpp`). Nettoyage `build/logs`, `build/EXTBD`.

**Fix client** : `./scripts/build_client.sh` — binaire `build/linux/t4c_client` reconstruit (39 seul après 46, 60 dans `finishEnterWorld`, cooldown **3 s**).

**Test** : redémarrer `T4CServer`, `./scripts/run_client.sh` → flèches → serveur `[PKT] move rx` + `[Move]` ; logout monde → attendre ~3 s → reconnect OK.

---

## 2026-06-08 05:15:00 — DEADLOCK UDP : GetNearItems (60) async + simple 60 client

**Famille : Serveur / Client**

**Symptôme** (test ~04:55) : 46 OK, `in_game=1`, client `[MOVE] -> dir=N`, **aucun** `[PKT] move rx` serveur, `[DEADLOCK] Suspected stall in UDPReceivePacketThread` après opcode 60.

**Cause** : `packet_inview_units` exécuté **sur le thread UDP** (handler 60 sync, parfois ×2) → thread bloqué 10–30 s, paquets move jamais analysés.

**Fix serveur** :
- **`RQFUNC_GetNearItems`** si `in_game` : enqueue `AsyncRQFUNC_GetNearItems` (scan peripherique hors thread UDP).
- **46 déjà in_game** : plus de `packet_inview_units` sync sur UDP (code=1 seulement).

**Fix client** :
- Après 46 : envoi **39 seulement** ; le **60** reste dans `finishEnterWorld()` après preload (évite double 60).

**Test attendu** :
```
[GetNearItems] enqueue async pour test
[PKT] move rx opcode=7 …
[Move] rx dir=7 … in_game=1
[PHASE] <- move ack @ …
```
Pas de `[DEADLOCK]` pendant les flèches.

---

## 2026-06-08 05:00:00 — Reconnexion : 46 async + ExitGame + mot de passe

**Famille : Serveur / Client**

**Symptôme** : après retour login / logout monde, reconnexion impossible (Register code=1, `password_len=0`, session serveur fantôme `preInGame`).

**Causes** :
1. **Opcode 46 sync sur thread UDP** (régression) — bloque analyse UDP, ExitGame retardé, compte « déjà en ligne » en BDD.
2. **ExitGame** laissait `boPreInGame=1` sur logout pré-monde.
3. **Client** : champ mot de passe vide au re-clic Connect ; cooldown 12 s trop long ; UDP fermé avant drain serveur.

**Fix serveur** :
- **46 Linux** : `AsyncFuncQueue` (`AsyncRQFUNC_FromPreInGameToInGame`) — aligné `T4C_Server_Linux_Final_Step`.
- **ExitGame** : toujours `boPreInGame=0`, `DeleteOnlineUserSync` immédiat.
- **Register** : fast-path si `preInGame && key && !in_game` (reconnect même socket).
- **Moves** : routage 1–8 via `GetPlayerResourceFctForSession` (fix précédent conservé).

**Fix client** :
- Mot de passe mémorisé (`g_password`) si champ vide au Connect.
- LogoutWorker : `WaitForServerWorldLogoutDrain()` avant fermeture UDP ; cooldown **3 s**.

---

## 2026-06-08 04:30:00 — Mouvement : routage paquets 1–8 (46 OK, moves ignorés)

**Famille : Serveur / Client**

**Symptôme** (test ~04:25) : flux auth + opcode **46 code=0** OK, `[Main] entree monde OK`, client envoie `[MOVE] -> dir=N`, mais **aucun** `[PKT] move rx` / `[Move]` serveur — perso tourne sans avancer (`canMove_` jamais confirmé par le serveur).

**Cause** : opcodes **1–8** passaient par `GetPlayerResourceFctForMove` avec filtre `registred` + `in_game` **avant** PickLock, et `bValidRegister=true` exigeait encore `registred` — paquets droppés silencieusement alors que 46/60 passaient via `GetPlayerResourceFctForSession`.

**Fix serveur** (`PacketManager.cpp`, `TFCMessagesHandler.cpp`, `PlayerManager.cpp`) :
- `bValidRegister=false` pour opcodes **1–8** (et KB 211–218).
- Routage moves via **`GetPlayerResourceFctForSession`** (même chemin que 46/60/39).
- Log systématique `[Move] rx dir=…` dans `RQFUNC_PlayerMove`.
- PickLock session : **128** tentatives (au lieu de 64).

**Fix client** : log `[PHASE] <- move ack @ x,y` quand l'ACK `__EVENT_OBJECT_MOVED` (opcode 1) est reconnu comme joueur local.

**Test attendu** après redémarrage **`build/T4CServer`** + `./scripts/run_client.sh` :
```
[PKT] move rx opcode=7 …
[Move] rx dir=7 compte='test' in_game=1 …
[Move] compte='test' dir=7 pos …
```
côté client :
```
[MOVE] -> dir=7
[PHASE] <- move ack @ …
```

---

## 2026-06-08 04:00:00 — Serveur : flux Linux 13→46 corrigé (mouvement + stall UDP)

**Famille : Serveur**

**Symptôme** : `in_game immediat` dans les logs, opcode 46 en double (`code=0` puis `code=1`), `[DEADLOCK] UDPReceivePacketThread`, aucun `[PKT] move rx` / `[Move]` malgré `[MOVE] -> dir=N` côté client — le perso tourne sans avancer.

**Cause** : régression du fix 03:20 — `StartAsyncFromPregameToGame()` (PutPlayerInGame + `in_game=true`) était appelé dans l'async **13** au lieu de l'async **46**. Le client envoie 46 dès réception du 13 ; le serveur bloquait le thread UDP (`packet_inview_units` sync) et retenait les verrous pendant que les moves arrivaient.

**Fix** (aligné `T4C_Server_Linux_Final_Step`) :
- **Async 13 (Linux)** : envoie opcode 13 via `StartPutPlayerInGame()`, libère le picklock, retourne avec `preInGame=1` / `in_game=0` (log `picklock relache apres 13 (46/60)`).
- **Async 46** : `UsePicklock` → `ReleasePicklockEarly` → `StartAsyncFromPregameToGame()` (PutPlayerInGame réel + opcode 46).
- **UDP 46 déjà en jeu** : réponse `code=1` + `packet_inview_units` sans rappeler PutPlayerInGame.
- **Moves** : `GetPlayerResourceFctForMove` (fast path 32× PickLock) + logs `[PKT] move rx`, `[Move] pas in_game`, `[Move] PickLock timeout`.

**Test attendu** : après redémarrage du binaire `T4CServer` reconstruit — séquence serveur `13` → client `46` → serveur `[FromPreInGameToInGame] async termine … in_game=1` → flèches → `[PKT] move rx opcode=…` + `[Move] compte=…`.

---

## 2026-06-08 04:15:00 — Race preInGame : moves avant opcode 46

**Famille : Client / Serveur**

**Symptôme** : `[PKT] move rx opcode=7` puis `[AUTH] paquet 7 ignore — GetPlayerResourceFct NULL` alors que `preInGame=1 in_game=0` — aucun log `[FromPreInGameToInGame] enqueue`.

**Cause** : le client mettait `g_inGame=true` dans `SendPostEnterWorldPackets()` **avant** la réponse 46 → moves envoyés alors que le serveur n'a pas encore fait `PutPlayerInGame()`. Opcode 46 bloqué par `GetPlayerResourceFct` (5000× PickLock) pendant l'async 13.

**Fix client** : `g_inGame` + `pipelineStep=6` uniquement après `FromPreInGameToInGame code=0|1` ; `SendMove` exige `g_fromPreInGameResult >= 0` ; handlers 60/status acceptent `g_waitingFromPreInGame`.

**Fix serveur** : `GetPlayerResourceFctForSession` (64× PickLock, sans exiger `in_game`) pour opcodes 46/60/skill ; async 13 Linux envoie opcode 13 puis libère le picklock (backpack/status déplacés dans `StartAsyncFromPregameToGame`).

---

## 2026-06-08 02:30:00 — Serveur : crash AsyncDeletePlayer LP64 (SIGSEGV à l'entrée en jeu)

**Famille : Serveur**

**Symptôme** : segfault dans `CPlayerManager::AsyncDeletePlayer` juste après `PutPlayerInGame` (`picklock relache apres 13`), compte connecté OK, perso chargé (`load_character -> code 0`).

**Cause** : `GetQueuedCompletionStatus` lisait l'adresse joueur dans un `DWORD` (32 bits) alors que `PostQueuedCompletionStatus` poste un `std::uintptr_t` (64 bits sur Linux) — pointeur tronqué → accès mémoire invalide à `lpPlayer->self`.

**Fix** (`FinalStEp0/T4Serv3/Exe Server/PlayerManager.cpp`) : `dwPlayerAddress` passé en `std::uintptr_t` (même pattern que `AsyncFuncQueue.cpp`).

---

## 2026-06-08 03:20:00 — Serveur : in_game réel après PutPlayerInGame (mouvement bloqué)

**Famille : Serveur**

**Symptôme** : connexion OK, perso chargé, mais déplacement impossible (rotation flèches seulement). Régression liée au split Linux `preInGame` / opcode 46 : le serveur restait `in_game=0`, les moves étaient ignorés.

**Fix** (`TFCMessagesHandler.cpp`) :
- Fin de `AsyncRQFUNC_PutPlayerInGame` (Linux) : `StartAsyncFromPregameToGame()` appelé immédiatement après libération du picklock (crée l'unité monde, `in_game=true`, envoie opcode 46).
- `AsyncRQFUNC_FromPreInGameToInGame` : conserve le pointeur `user` du paquet (plus `PeekPlayerAtEndpoint` qui pouvait rater la session après reconnexion).

**Préservé** : auth, reconnexion, purge sync RegisterAccount — pas de changement client.

---

## 2026-06-08 03:35:00 — Mouvement : ack serveur + reconnaissance local client

**Famille : Serveur / Réseau**

**Symptôme** : `in_game=1` OK, opcode 46 code=0, mais le perso pivote sans avancer (pas de confirmation move appliquée).

**Fix serveur** (`Character::StartMove`) : envoi systématique de `__EVENT_OBJECT_MOVED` (opcode 1) au joueur — succès **et** collision bloquée (position inchangée, pour débloquer `canMove_`).

**Fix client** (`T4CLoginSession.cpp`) : comparaison `unitId` en `uint32_t` ; fallback « move local » si la tuile cible est à ≤1 pas de la position active (ack serveur même si `unitId` désync).

**Debug serveur** : log `[Move] compte=… dir=…` quand un paquet 1–8 est traité.

---

## BASELINE IMMUABLE — déplacement joueur (validé 2026-06-04)

> **NE PAS MODIFIER** ce pipeline sans demande explicite et relecture Windows (`Tileset.cpp` `MoveToPosition`, `Packet.cpp` TFCMoveID).  
> C’est la **base du moteur in-game** : direction, animation, scroll, collisions. Toute « simplification » ou « optimisation » a déjà causé 3 régressions (traversée murs, dos permanent, snap/ même jambe).

### Architecture (deux positions + trois couches)

```
                    SERVEUR                          CLIENT VISUEL
                    ───────                          ─────────────
Position tuile  →  playerX_ / playerY_        (autorité — collision, move, attaque)
Centre caméra   →  playerDisplayX_ / Y_      (float — rendu map + PNJ + souris)
Sprite joueur   →  écran centre fixe          (direction + frame animation)
```

| Couche | Variable / fonction | Rôle |
|--------|---------------------|------|
| **Logique** | `playerX_`, `playerY_` | Tuile serveur ; mise à jour **uniquement** dans `applyServerPlayerMove`, popup, teleport |
| **Visuel carte** | `playerDisplayX_`, `playerDisplayY_` | Centre flottant ; lerp vers serveur dans `updatePlayerScroll()` |
| **Direction** | `playerDirection_` | Opcode **envoyé** (immédiat) + delta **confirmé** (serveur) |
| **Animation** | `playerAnimFrame_`, `playerMoving_` | Frames 0–7 ; continuité entre pas |

### Pipeline exact (ordre d’exécution)

1. **`tryMovePlayer(opcode)`** — si `!canMove_` **ou** `isPlayerScrolling()` → abort  
   - Pré-check map2 `isMapTileBlocking(nx, ny)`  
   - `T4CLoginSessionSendMove(opcode)` — **sans** bouger `playerX_`  
   - Direction : `T4CDirectionFromMoveOpcode(opcode_envoye)`  
   - Anim : reset frame/tick **seulement** si `!playerMoving_` ou direction changée  
   - `canMove_ = false` jusqu’à réponse (timeout 450 ms)

2. **Serveur** — confirme avec **`__EVENT_OBJECT_MOVED` = opcode 1** + `(x,y)` — **pas** la direction du pas

3. **`T4CLoginSessionConsumeLocalPlayerMove` → `applyServerPlayerMove(x,y,_)`**  
   - Met à jour `playerX_`, `playerY_` (serveur)  
   - Direction : `T4CDirectionFromWorldDelta(dx, dy)` — **jamais** `moveOpcode` du paquet entrant  
   - Anim : reset tick **seulement** si direction changée  
   - **Ne pas** appeler `snapPlayerViewToServer()` ici — le scroll lerp gère l’écart visuel  
   - `canMove_ = true`

4. **`updatePlayerScroll()`** (chaque frame)  
   - Lerp display → serveur : **8 pas par tuile** (`kPlayerScrollSteps = 8`, = Windows `g_DONE`)  
   - Équivalent pixels : 32/8 = 4 px X, 16/8 = 2 px Y par frame (`g_MOVX`, `g_MOVY`)

5. **Rendu** — `RenderIsoViewport(playerDisplayX_, playerDisplayY_, …)` ; `worldToScreen*` utilise display, pas `playerX_`

6. **Exception snap** — `snapPlayerViewToServer()` **uniquement** : spawn, popup correction, teleport

### Constantes (ne pas changer arbitrairement)

| Constante | Valeur | Équivalent Windows |
|-----------|--------|-------------------|
| `kT4CPlayerScrollSteps` | **8** | `g_DONE` |
| `kMoveServerTimeoutMs` | **450** | attente ACK move |
| `kMoveCooldownMs` | **100** | cadence envoi |
| Anim frame tick | **90 ms** | ~`nbSprite4Move` |
| `playerAnimUntil_` extension | **450 ms** + flèche enfoncée | marche continue |

### Fichiers concernés (toucher = relire toute la baseline)

| Fichier | Symboles |
|---------|----------|
| **`src/game/T4CMovementBaseline.cpp`** | **Source unique** — direction, opcodes, scroll, politique anim |
| **`src/tests/t4c_regression_tests.cpp`** | **Tests non-regression** — un cas par invariant |
| `src/game/GameWorldScreen.cpp` | `tryMovePlayer`, `applyServerPlayerMove`, `updatePlayerScroll`, … |
| `src/assets/map/T4CV2WorldMap.cpp` | `RenderIsoViewport(float centerX, …)` |
| `src/network/T4CLoginSession.cpp` | `HandleRemoteMove`, `QueueLocalPlayerMove` |

### Tests automatiques (obligatoire avant chaque avancée)

```bash
./scripts/run_regression_tests.sh     # aussi lance a la fin de build_client.sh
T4C_RUN_E2E=1 ./scripts/run_regression_tests.sh   # + E2E reseau si serveur dispo
ctest --test-dir build/linux          # equivalent CMake
```

**Règle :** échec = **stop**. Nouveau comportement validé → ajouter `T4C_TEST(...)` dans `t4c_regression_tests.cpp`.

### Interdit (régressions déjà vues)

| Ne jamais faire | Symptôme |
|-----------------|----------|
| `applyLocalMoveDelta` avant confirmation serveur | Traverse les murs |
| Direction depuis opcode paquet **entrant** (= 1) | Perso **toujours de dos** |
| Reset `playerAnimTick_` à chaque pas (try + apply) | **Même jambe** / frame 0 en boucle |
| Mettre à jour `playerDisplayX/Y` = serveur dans `applyServerPlayerMove` | **Snap** tuile, plus de scroll |
| Supprimer `isPlayerScrolling()` gate dans `tryMovePlayer` | Pas qui se chevauchent / saccades |
| Revenir à `RenderIsoViewport(uint centerX, …)` entier only | Perte du scroll sub-tuile |

### Réseau — rappel opcode (définitif)

- **Client → serveur** : opcodes **1–8** = direction **demandée**
- **Serveur → client** : **`__EVENT_OBJECT_MOVED` = 1** toujours — direction = **delta position**

---

## Règles générales de fonctionnement du moteur (référence permanente)

**Objectif :** ne pas réinventer, ne pas régresser. Avant toute modification gameplay/rendu/réseau, vérifier **BASELINE IMMUABLE** (ci-dessus) + le client Windows (`client/T4C Client/`).

### 1. Principe de port

| Règle | Détail |
|-------|--------|
| Source de vérité | Client Windows **1.72** + serveur T4Serv3 **1.72** + assets `runtime/` |
| Méthode | Trouver opcode/fonction Windows → parser identique → brancher rendu existant (atlas V2, puppet) |
| Test minimal | Spawn **Lighthaven (2944, 1059)** avant d'élargir |
| Interdit | Réinventer un moteur parallèle (TnC `.dec`, moteur 1.68, prédiction position qui traverse les murs) |

### 2. Autorité serveur vs prédiction visuelle

| Donnée | Autorité | Client Linux |
|--------|----------|--------------|
| **Position tuile** joueur | Serveur uniquement | Mise à jour dans `applyServerPlayerMove` ou popup — **jamais** `applyLocalMoveDelta` avant confirmation |
| **Direction sprite** | Delta position (confirmé) + opcode envoyé (immédiat) | Voir section mouvement ci-dessous |
| **Animation marche** | Client (visuel) | Frames 0–7, **ne pas reset** à chaque pas si même direction |
| **Collision murs** | Serveur (+ pré-check map2 client) | Bloquer l'envoi si tuile map2 `IsBlockingMapObject` |

Le client **peut** et **doit** prédire visuellement (direction, animation) — il **ne doit pas** prédire la position tuile.

### 3. Boucle de déplacement joueur (Windows vs Linux)

> **Implémentation de référence :** section **BASELINE IMMUABLE** en tête de changelog — ne pas dupliquer ni raccourcir ce pipeline.

**Windows (`Packet.cpp` → `MoveToPosition`) :**

1. Client envoie opcode **1–8** (direction demandée)
2. Serveur valide → renvoie **`__EVENT_OBJECT_MOVED` (opcode 1)** + `(x,y)` nouvelle tuile
3. `MoveToPosition` : calcule direction depuis **delta** `(NewX-Player.xPos, NewY-Player.yPos)`
4. Lance **`Done = g_DONE`** (8 frames) : scroll carte + incrément `SpriteNumber` **sans reset** à chaque frame

**Linux (`GameWorldScreen`) :**

| Étape | Fonction | Règle |
|-------|----------|-------|
| Flèche enfoncée | `tryMovePlayer` | Envoie opcode ; direction immédiate depuis opcode **envoyé** ; anim continue si même direction |
| Confirmation | `applyServerPlayerMove` | Position `(x,y)` ; direction depuis **delta** ; **ne pas** reset `playerAnimTick_` si même direction |
| Attente | `canMove_ = false` | Jusqu'à réponse serveur ou timeout 450 ms |
| Scroll visuel | `Done = g_DONE` (8 frames) | `playerDisplayX/Y` → lerp 8 pas/tuile ; pas de pas suivant tant que scroll actif |

→ Voir aussi **Référence définitive — mouvement & direction** (opcodes client vs serveur).

### 4. Animation marche (sprites 3D)

| Règle | Windows | Linux |
|-------|---------|-------|
| Frames | `SpriteNumber` 1…N, ping-pong | `playerAnimFrame_` 0–7 (~90 ms) |
| Reset frame 0 | Uniquement arrêt ou changement direction | `!playerMoving_` ou `dirChanged` — **pas** à chaque pas serveur |
| Continuité | `SpriteNumber` avance pendant tout `Done` | `playerAnimUntil_` prolongé tant que flèche enfoncée |
| Symptôme régression | — | **Même jambe / frame 0** si `playerAnimTick_` reset dans `tryMovePlayer` **et** `applyServerPlayerMove` |

### 5. Rendu monde

| Sujet | Règle |
|-------|-------|
| Projection | **Rectangulaire** T4C : `screenX += 32*worldX`, `screenY += 16*worldY` (pas iso 45° mathématique) |
| Sol | `v2_*map.map1` + atlas V2 NMS |
| Objets | `map2` + `T4CMapObjectSprites` (~2210 IDs auto-gen) |
| Humains | Apparence **10011/10012** → puppet **19 calques** + opcode **68** obligatoire |
| Créatures | Table apparence → sprite 3D (`gen_creature_appearance_table.py`) |
| Joueur | Toujours **centré écran** ; le monde scroll via **`playerDisplayX/Y`** (lerp 8 pas/tuile, comme `SepX/SepY` Windows) |

### 6. Réseau (paquets)

| Règle | Détail |
|-------|--------|
| Skip opcode | Toujours `SkipOpcode(msg)` avant lecture champs (fix LP64) |
| Ordre champs | Identique `Packet.cpp` / `TFCPacket` Windows |
| Move entrant | Opcode paquet = **1** (`__EVENT_OBJECT_MOVED`) — **pas** la direction |
| Puppet | Opcode **68** : body, legs, helm, weapons… par `unitId` |

---

## 2026-06-06 21:47:00 — Reconnexion après monde : socket conservé + reuse slot serveur (FIGÉ — E2E ALL PASS)

**Problème** : après `PutPlayerInGame`, retour login → Connect → timeout Register/99. Cause : fermeture UDP (nouveau port) + session `in_game` fantôme + blocages ODBC/maintenance.

**Client** (`T4CLoginSessionDisconnectInGame`) :
- ExitGame fire-and-forget (3×) + poll court
- **Conserve le socket UDP** (même stratégie que Esc écran persos)
- Plus de `T4CCommBridgeStop()` au retour login depuis le monde

**Serveur** (`T4Serv3`) :
- `ResetFlaggedSessionForReauth` : réutilise le slot UDP après ExitGame (sans delete async / ODBC)
- `PurgeAccountSessionsExcept` : purge `in_game` même port + autres ports, sans `cMaintenanceLock`
- `CreatePlayer` : sans `cMaintenanceLock` ; recycle les ghosts `IsDeleteFlags()` sur même IP/port
- `LoadPlayer` : accepte `lpReusePlayer` pour re-auth immédiat

**Tests** : `./scripts/run_reconnect_sim.sh` → **ALL PASS** (auth, double Connect, Esc persos, monde→login→reconnect).

---

## 2026-06-06 19:04:26 — Auth reconnexion : re-clic Connect + retry 99/26 (FIGÉ — validé E2E)

**Famille : Réseau / GUI / Build**

> **État validé** : `./scripts/run_reconnect_sim.sh 127.0.0.1 11677` → **ALL PASS** (auth simple, double Connect 1 socket, Esc persos → reconnect).  
> **Ne pas réintroduire** : cooldown reconnect UI, « Connect ignore » pendant auth, `T4CCommBridgeStop()` sur timeout Register, `ExitGame` systématique au retour écran persos.

### Symptôme corrigé

| Scénario | Avant | Après |
|----------|-------|-------|
| 1er Connect → 14 puis 99, pas de 26 | UI « Connexion en cours… » indéfiniment | Retry auto 99/26 + message d’erreur si échec |
| 2e clic Connect pendant auth | `[AUTH] Connect ignore — auth deja en cours` — **aucune action** | **Renvoi immédiat** du paquet de l’étape en cours (**même socket UDP**) |
| Version refusée (99 long=0) | `T4CCommBridgeStop()` sans reset `pipelineStep` → UI bloquée | `FailAuthPhase()` → erreur visible, `step=0`, socket conservé |
| Esc écran persos → reconnect | Parfois nouveau port / session fantôme | Socket UDP **conservé** ; Register → resend serveur `(char)0 + key` → 99 → 26 en ~1 s (local) |

### Cause racine (client)

1. **Double-clic / re-clic** : destruction du socket en cours (`AbortLogin` + `T4CCommBridgeStop`) ou **ignorance** du 2e Connect → le serveur ne recevait jamais le Register NM complet sur le nouveau port.
2. **Blocage post-99** : seul Register (step 1) avait un timeout/retry ; après envoi du **99** (step 2) ou **26** (step 3), aucun retry ni erreur → attente infinie.
3. **Chemin d’échec 99** : version refusée ou `TFCPacketException` fermait le bridge sans remettre `g_pipelineStep` à 0 ni remplir `g_authError`.

### Correctifs client (`FinalStEp2`)

#### `src/network/T4CLoginSession.cpp`

- **`T4CLoginSessionStart()`** : si auth en cours (step 1–3) **et** même IP/port (`SameLoginEndpoint` + `T4CCommBridgeIsOpen()`), **renvoie** :
  - step 1 → `SendRegister` (14)
  - step 2 → `SendAuthVersion` (99)
  - step 3 → `SendCharacterListRequest` (26)
  - **Pas** de nouveau socket, **pas** d’ignore silencieux.
- **`T4CLoginSessionPollBackgroundTasks()`** :
  - step 1 : retry Register toutes les **8 s** (max 3) — inchangé, mais timeout via `FailAuthPhase()` **sans** `T4CCommBridgeStop()`.
  - step 2 / 3 : retry **99** ou **26** toutes les **5 s** (max 6), puis message *« Pas de reponse serveur (99/26). »*.
- **`FailAuthPhase(message)`** : remplit `g_authError`, `g_pipelineStep=0`, reset compteurs — **socket UDP laissé ouvert** pour re-clic immédiat.
- **`HandleAuthVersionReply()`** : log `[PHASE] <- RQ_AuthenticateServerVersion (99) valid=N` ; refus → `FailAuthPhase` (plus de `T4CCommBridgeStop()`).
- **`HandleRegisterReply()`** : reset `g_authPhaseRetryCount` avant envoi 99.
- **`ParseCharacterList()`** : reset `g_authPhaseSentAt` / retry à réception opcode 26.
- **`T4CLoginSessionAbortLogin()`** : `T4CCommBridgeStop()` **uniquement si `g_inGame`** ; écran login/persos → reset état session, **socket conservé**.
- **`SendAuthVersion()` / `SendCharacterListRequest()`** : horodatage `g_authPhaseSentAt` pour les timeouts.

#### `src/network/T4CCommBridge.cpp`

- **`T4CCommBridgeStart()`** : si même host/port (`SameEndpoint`) → **réutilise** le socket existant (pas de `COMM.Close()` + `Create()`).
- **`T4CCommBridgeStop()`** : ne touche **pas** à `g_boQuitApp` (fix antérieur conservé).

#### `src/gui/LoginScreen.cpp`

- Suppression du early-return *« Auth en cours — Connect ignore »* : le 2e clic appelle `T4CLoginSessionStart()` qui relance l’étape.
- **`Update()`** : affiche l’erreur auth quand `step=0` ; pendant auth : *« Connexion en cours… (re-cliquer Connect relance l'etape en cours) »*.

#### `client/T4C Client/Comm.cpp`

- Linux : `RQ_RegisterAccount` → `dwMaxAck=0`, `dwMaxDelay=0` (évite fragments NM bloqués en reconnect).

#### `client/T4C Client/UDP/NMPacketManager.cpp` / `Comm.cpp`

- `PacketCenter::Create()` : retourne `FALSE` si `NMPacketManager::Init()` échoue (avant : succès silencieux, envois ignorés).

#### `src/main.cpp`

- Esc écran persos : `T4CLoginSessionAbortLogin()` + `ResetAfterReturnToLogin()` — **pas** d’`ExitGame` (session serveur auth-only conservée sur le même port).

#### Outils / tests

- **`src/tools/e2e_auth_probe.cpp`** : sonde headless 14→99→26 ; modes `T4C_E2E_DOUBLE_CONNECT=1`, `T4C_E2E_RECONNECT=1`.
- **`scripts/run_reconnect_sim.sh`** : 3 cas (auth simple, double Connect, Esc→reconnect) avec restart serveur entre cas.

### Correctifs serveur associés (`FinalStEp0/T4Serv3` — requis sur **192.168.1.76**)

| Fichier | Changement |
|---------|------------|
| `TFCMessagesHandler.cpp` | Resend Register si session déjà auth même IP/port ; `FlagDeleteByAccount` sync ; ghost → `SetDeletePlayerFlags` (pas purge sync UDP) |
| `PlayerManager.cpp` | `VerifyPlayerUnique` : ignore ghost auth-only autre port ; `GetPlayerResourceFct` maxLoop **500** |
| `PacketManager.cpp` | Log stderr si paquet 99/26/14 ignoré (`GetPlayerResourceFct NULL`) |

Sans rebuild serveur distant, le client peut envoyer 99 indéfiniment sans réponse (ghost / PickLock).

### Pipeline auth figé (client 1.72)

```
Connect → 14 Register → (réponse 14) → 99 AuthenticateServerVersion → (valid=1) → 26 GetPersonnalPClist → écran persos
         ↑ re-clic Connect renvoie l'étape courante (même UDP)
         ↑ retry auto si pas de réponse (8s/14, 5s/99, 5s/26)
```

### Rebuild / test

```bash
./scripts/build_client.sh
T4C_RUNTIME=./runtime ./scripts/run_client.sh

# E2E local (serveur T4Serv3 build requis)
./scripts/run_reconnect_sim.sh 127.0.0.1 11677

# Sonde seule
T4C_E2E_RECONNECT=1 ./build/linux/t4c_e2e_auth_probe 127.0.0.1 11677 test test
```

**Serveur distant :** `./scripts/build_server.sh` puis redéployer `T4CServer` sur **192.168.1.76:11677**.

### Fichiers touchés (liste exacte)

- `src/network/T4CLoginSession.cpp`
- `src/network/T4CCommBridge.cpp` / `.h`
- `src/gui/LoginScreen.cpp`
- `src/main.cpp`
- `client/T4C Client/Comm.cpp`
- `client/T4C Client/UDP/NMPacketManager.cpp`
- `src/tools/e2e_auth_probe.cpp`
- `scripts/run_reconnect_sim.sh` (nouveau)

---

## 2026-06-04 — Suite tests non-regression (`t4c_regression_tests`)

**Famille : Build / Doc**

### Objectif

Vérifier **automatiquement** qu'aucune régression ne casse la baseline (direction, scroll, opcodes) avant chaque nouvelle feature.

### Contenu

- **`src/game/T4CMovementBaseline.cpp`** — logique pure extraite (source unique testée + runtime)
- **`src/tests/t4c_regression_tests.cpp`** — 12 tests baseline (direction Windows, opcode serveur ≠ direction, scroll 8 pas, anim policy, …)
- **`scripts/run_regression_tests.sh`** — lance les tests ; échec = exit 1
- **`build_client.sh`** — exécute les tests après chaque build
- **`ctest --test-dir build/linux`** — intégration CMake

### Usage

```bash
./scripts/build_client.sh              # build + tests
./scripts/run_regression_tests.sh      # tests seuls
```

**Règle équipe :** tout comportement validé manuellement → ajouter un `T4C_TEST` ici.

---

## 2026-06-04 — Matrice parité Windows → Linux (référence méthodique)

**Famille : Doc / Décision**

Source de vérité : `client/T4C Client/` (Packet.cpp, VisualObjectList.cpp, Puppet.cpp, V3_*Dlg).  
Objectif : **reproduire le comportement réseau + rendu**, pas réinventer.

| Domaine | Client Windows (1.72) | Linux aujourd'hui | Prochaine étape |
|---------|-------------------------|-------------------|-----------------|
| **Auth / login** | 14→99→26→103→153→13 | ✅ OK | — |
| **Entrée monde** | PutPlayerInGame + 60 + 46 | ✅ OK | — |
| **Carte sol V2** | TileSet + V2Database | ✅ NMS 5–131, preload | Lissage SmoothTiles |
| **Objets map2** | DrawTileSet 2210 IDs | ✅ ~2210 auto-gen | Portails / portes animées |
| **Joueur local** | Puppet + MoveToPosition + Done | ✅ **baseline immuable** (puppet 3D, scroll 8f, direction delta, anim continue) | anim attaque, variantes couleur |
| **PNJ puppet** | opcode 68 + KnownPuppet + DrawPuppet | ✅ idem (Kilhiam, gardes, commerçants…) | direction/frame anim serveur |
| **PNJ fixes** | appearance → Set 3D | ✅ table 160 créatures | Merchants, Darkfang, etc. = puppets ou IDs manquants |
| **Mouvement PNJ** | broadcast `__EVENT_OBJECT_MOVED`(1) + delta position | ✅ direction delta + frame + interpolation | MovingQueue multi-pas Windows |
| **Clic / cible** | souris → TargetID → curseur ATTACK | ✅ ciblage + attaque opcode 24 | curseur ATTACK, mode combat (198) |
| **Objets sol** | opcode 16/60, cliitmpd | ✅ markers + use/get | sprites items V2 |
| **Portails** | objets map + scripts | ⚠️ RQ_UseObject map2 + sol | scripts serveur |
| **HUD in-game** | V3_LifeDlg HP/MP | ✅ panneau V3_LifeBackF bas-droite | inventaire V3_InvDlg, barre XP |
| **Inventaire** | RQ_ViewInv 156, drag&drop | ❌ | T4CPlayerInventory existant |
| **Commerce NPC** | OnTalk + fenêtre shop | ❌ | après ciblage |
| **Stats / XP** | opcode 43/44/37 | ✅ parse + barres HUD | feuille stats V3_StatsDlg |
| **Musique zone** | GameMusic zones | ✅ WAV zone 0 | — |

### Règle de port (ne plus régresser)

1. **Trouver l'opcode / la fonction Windows** (`grep RQ_* Packet.cpp`)
2. **Parser identique** (mêmes champs, même ordre)
3. **Brancher le rendu** existant (atlas V2, pas nouveau système)
4. **Tester au spawn Lighthaven** (2944,1059) avant d'élargir

### Référence définitive — mouvement & direction (T4C 1.72)

**À conserver — ne pas régresser.** Distinction fondamentale entre opcodes **client→serveur** et paquets **serveur→client**.

| Sens | Valeur | Rôle |
|------|--------|------|
| **Client → serveur** | opcodes **1–8** | Demande de déplacement : `RQ_MoveNorth`(1) … `RQ_MoveNorthWest`(8) — la **direction demandée** |
| **Serveur → client** | **`__EVENT_OBJECT_MOVED` = 1** | Confirmation de déplacement réussi (joueur local + broadcast PNJ) — **toujours opcode 1**, quelle que soit la direction réelle du pas |

**Sources serveur (T4Serv3) :** `GameDefs.h` (`__EVENT_OBJECT_MOVED`), `Character.cpp` (`SendPlayerMessage` après move), `WorldMap.cpp` (`BCast` après `move_unit`).

**Sources client Windows :** `Packet.cpp` (case TFCMoveID → `World.MoveToPosition(X,Y)`), `Tileset.cpp` `MoveToPosition` (l.18849–18864) — retourne la direction depuis **`MovX = NewX - Player.xPos`**, **`MovY = NewY - Player.yPos`**, **pas** depuis l'opcode du paquet entrant.

**Règle de rendu (Linux = Windows) :**

| Étape | Calcul direction |
|-------|------------------|
| Envoi move (clavier) | `T4CDirectionFromMoveOpcode(opcode_envoye)` — prédiction visuelle immédiate dans `tryMovePlayer` |
| Confirmation serveur | `T4CDirectionFromWorldDelta(newX - oldX, newY - oldY)` dans `applyServerPlayerMove` |
| PNJ distants (opcodes 1–8 reçus) | **Delta position prioritaire** — l'opcode paquet vaut 1 (`__EVENT_OBJECT_MOVED`), pas la direction |

**Table direction numpad (identique `MoveToPosition`) :**

| Delta monde | Direction |
|-------------|-----------|
| `(0, +1)` sud | **2** (face caméra) |
| `(0, -1)` nord | **8** (dos) |
| `(+1, 0)` est | **6** |
| `(-1, 0)` ouest | **4** |
| `(+1, +1)` | **3** |
| `(+1, -1)` | **9** |
| `(-1, +1)` | **1** |
| `(-1, -1)` | **7** |

**Symptôme si oubli :** perso **toujours de dos** (direction 8) à chaque pas — régression observée après Phase 4c quand la direction était lue depuis l'opcode paquet serveur (= 1 = nord) au lieu du delta.

**Fichiers Linux :** `T4CDirectionFromWorldDelta`, `T4CDirectionFromMoveOpcode` (`T4CLoginSession.cpp`), `GameWorldScreen::tryMovePlayer`, `applyServerPlayerMove`, `syncRemoteUnitsFromNetwork`.

### Ce qui bloque « voir tous les PNJ » sans les nommer un par un

- **~80 % des PNJ Lighthaven** = `MOB_NPC_APPEARANCE(PUPPET)` → apparence **10011/10012** + **opcode 68 obligatoire**
- **Commerçants, prêtres, Darkfang** = puppets habillés — pas des sprites 200xx
- **Gardes Olin Haad** = Brigand 20006 (sprite Warrio) — **patrouille = opcodes move serveur** (à vérifier)
- **Tour des pages / portails** = objets map2 + scripts — pas encore rendus/interactifs

---

## 2026-06-04 — Phase 3a : puppet joueur + PNJ (opcode 68)

**Famille : Réseau / Rendu**

### Symptômes utilisateur

- Perso « à poil » / guerrier nu alors que l'inventaire a habit + torche
- PNJ temple (Kilhiam, Moonrock, Frère Kiran) invisibles ou étiquetés « Joueur »
- Seuls les Brigands (gardes 20006) visibles

### Cause (client Windows)

- En jeu, **tous les humains** (joueur + PNJ habillés) utilisent apparence **10011/10012** (puppet)
- Le serveur envoie **`RQ_PuppetInformation` (68)** avec les slots d'équipement (`__OBJGROUP_*`)
- Windows : `Objects.SetPuppet()` → `Puppet::SetPuppet()` — **sans opcode 68, rien ne se dessine** pour les puppets

### Correctifs Linux

- Handler **opcode 68** : stocke `body` par unitId + joueur local (`g_activePlayer.puppetBody`)
- **Requête auto** opcode 68 à l'entrée monde + à chaque spawn puppet
- **Race → puppet** : race 10001–10004 → apparence 10011, 15001–15004 → 10012
- ~~**Sprite dérivé**~~ (Phase 3a temporaire) : remplacé par rendu 19 calques en **Phase 3b**
- Label sélection : plus « Joueur » sur les puppets PNJ ; fond noir lisible

### Limite connue (Phase 3a — réseau uniquement)

- Le rendu utilisait encore une **approximation** (sprite corps unique Cleric/Guard/Paysan) — **corrigé en Phase 3b** ci-dessous
- HUD / inventaire / attaque / portails : **Phase 4+** (voir matrice ci-dessus)

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Phase 3b : migration Puppet.cpp (tables auto-générées)

**Famille : Rendu / Build**

### Contexte

La Phase 3a branchait l'opcode **68** mais dessinait un seul sprite dérivé de l'équipement (`WHITEROBE→Cleric`, `CHAIN→GuardModel1`, etc.). Sur Windows, `Puppet.cpp` + `VisualObjectList.cpp::Puppetize()` composent **19 calques** (corps, jambes, armes, cape…) avec un ordre Z (`BodyOrderR`) par direction.

### Approche : générateur Python (pas de copie manuelle des ~700 variantes)

Script **`scripts/gen_puppet_tables.py`** — parse les sources Windows 1.72 :

| Source Windows | Extrait | Fichier généré Linux |
|----------------|---------|----------------------|
| `Puppet.cpp` → `SetPuppet()` | PUPEQ × slot × genre → sprite 3D | `src/game/T4CPuppetResolve.inc` |
| `VisualObjectList.cpp` → `Puppetize()` | `__OBJGROUP_*` → slot + PUPEQ | `src/game/T4CPuppetize.inc` |
| `Puppet.cpp` → init `BodyOrderR` | ordre de dessin 8 directions × 19 slots | `src/game/T4CPuppetBodyOrder.inc` |
| `Puppet.h` + `Apparence.h` | constantes PUPEQ / OBJGROUP | (inclus dans le parse) |

**Volumes générés (2026-06-04) :**

- **1695** entrées resolve (1486 lignes actives après dédup slot/genre)
- **705** lignes Puppetize (OBJGROUP → équipement puppet)
- **293** noms de sprites 3D uniques (`PupWhiteRobe`, `PupBattleSword`, …)
- **341** constantes PUPEQ, **1117** OBJGROUP

Les `.inc` sont **régénérés à chaque build** via `scripts/build_client.sh` (appel `python3 scripts/gen_puppet_tables.py` avant cmake).

### Pipeline runtime Linux (`src/game/T4CPuppet.cpp`)

1. **Opcode 68** → `T4CPuppetDress` (body, legs, helm, weapons, cape…)
2. **`T4CPuppetizeFromDress`** — lookup `kPuppetizeRows[]` → remplit `T4CPuppetInfo.slot[19]` (PUPEQ par slot)
3. **`T4CPuppetResolveLookup`** — PUPEQ + slot + genre → nom sprite (`Pup*`)
4. **`drawPuppetInternal`** — boucle 19 calques selon `kPuppetBodyOrderR[direction][layer]`
5. **`GameWorldScreen::drawUnitSprite`** — apparences **10011/10012** → `T4CPuppetTryDraw` (plus de fallback Cleric/Guard)
6. **Preload** — `T4CPuppetCollectPreloadBases()` charge les 293 bases + frames directionnelles

### Spot-checks Lighthaven (alignés Windows)

| Équipement OBJGROUP | PUPEQ | Sprite |
|---------------------|-------|--------|
| WHITEROBE (425) | PUPEQ_WHITEROBE | `PupWhiteRobe` |
| BODY_CLOTH1 (285) | PUPEQ_SET1 | `PupBodyClothSet1` |
| BATTLESWORD | PUPEQ_BATTLESWORD | `PupBattleSword` |
| ROMANSHIELD | PUPEQ_ROMANSHIELD | `PupRomanShield` |

PNJ attendus au temple : Kilhiam / Moonrock (femme 10012 + robe blanche), Frère Kiran (10011 + robe), gardes (10011 + chain + cape rouge + épée/bouclier).

### Fichiers touchés

- `scripts/gen_puppet_tables.py` — générateur
- `src/game/T4CPuppet.cpp`, `T4CPuppetDraw.h`, `T4CPuppetTypes.h` — runtime
- `src/game/T4CPuppetResolve.inc`, `T4CPuppetize.inc`, `T4CPuppetBodyOrder.inc` — **ne pas éditer à la main**
- `src/game/GameWorldScreen.cpp` — rendu + preload puppet
- `scripts/build_client.sh` — regen automatique

### Limites restantes (parité Windows incomplète)

| Sujet | Windows | Linux aujourd'hui |
|-------|---------|-------------------|
| **Variant index** (`LoadSprite3D(..., N)`) | couleurs plate / teintes armure | champ `variant` parsé mais **non appliqué** au draw |
| **BodyOrderA / BodyOrderAR** | ordre Z attaque / distance | seul **BodyOrderR** (debout) |
| **Slot HAIR** | Puppetize HAIR | `T4CPuppetDress` sans champ cheveux |
| **Slots MASK (17) / WEAPON2 (18)** | switches PuppetInfo | inclus dans resolve ; à valider en jeu |
| **Animation frame** | frame marche / attaque par direction | frames **0–7** en marche (Phase animation PNJ) |
| **Direction PNJ** | delta position (`MoveToPosition`) | delta position — voir référence définitive (pas opcode paquet serveur) |

### Test

```bash
./scripts/build_client.sh
T4C_RUNTIME=./runtime ./build/linux/t4c_client 192.168.1.76 11677
# Spawn Lighthaven ~2944,1059 — vérifier puppets temple + gardes + perso habillé
```

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Animation + interpolation PNJ distants (opcodes 1–8)

**Famille : Rendu / Réseau**

### Symptôme

PNJ / gardes visibles mais **figés** — pas de marche ni glissement entre tuiles.

### Cause

- Opcodes **1–8** (TFCMoveID) mettaient à jour la tuile serveur mais le rendu utilisait **direction 2 + frame 0** pour tous.
- Pas d'**interpolation** entre positions (Windows : `MoveObject` + `MovingQueue`).

### Correctifs

- Direction depuis **delta position** `(dx,dy)` — pas depuis l'opcode paquet serveur (voir référence définitive ci-dessus).
- **Frames marche** (0–7) pendant le déplacement.
- **Glissement** `displayX/Y` → `serverX/Y` (~6 frames/tuile).
- Puppet + creatures utilisent direction/frame au draw.

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Phase 4a : HUD vie/mana (V3_LifeDlg)

**Famille : Rendu / Réseau**

### Référence Windows

`NewInterface/V3_LifeDlg.cpp` — panneau **216×74** bas-droite (`V3_LifeBackF`, barres `V3_PVBar` / `V3_PMBar`, clip `DrawSingleStatusItem`).

### Correctifs Linux

- **`T4CGameHud`** — rendu panneau + barres HP/MP (clip gauche = % rempli, comme Windows)
- Opcodes branchés : **43** (`RQ_GetStatus`), **33** (`RQ_HPchanged`), **67** (`RQ_ManaChanged`), **44** (`RQ_XPchanged`), **37** (`RQ_LevelUp`)
- `T4CLoginSessionRequestPlayerStatus()` à l'entrée monde
- Preload sprites HUD dans le pipeline atlas V2
- Police launcher (`T4CUiFont` via `LauncherChrome`) pour nom / niveau / libellés PV PM

### Fichiers

- `src/game/T4CGameHud.cpp`, `T4CGameHud.h`
- `src/network/T4CLoginSession.cpp` — handlers + mutex `g_statusMutex`
- `src/game/GameWorldScreen.cpp` — remplace le placeholder debug par le panneau
- `CMakeLists.txt` — source HUD

### Hors scope (Phase 4b+)

- `V3_MainBarDlg` (barre XP), `V3_InvDlg`, état mort (`V3_LifeBackFD`), portrait puppet clip, drag du panneau

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Ciblage + attaque (RQ_Attack 24)

**Famille : Réseau / Gameplay**

### Référence Windows

`MouseAction.cpp::Combat()` — clic sur grille COMBAT (type 2) → `RQ_Attack` avec tuile cible + `unitId`. Mêlée : pathfind puis attaque à portée.

### Linux

- **`T4CLoginSessionSendAttack(x, y, unitId)`** — opcode **24** identique Windows
- **Ciblage** : clic gauche (sélection + cadre + barre HP %)
- **Attaque** : clic droit, double-clic gauche, ou touche **A** sur la cible
- **Poursuite mêlée** : marche auto vers la cible (opcodes 1–8) puis `RQ_Attack` à 1 tuile
- HP distant stocké depuis spawn/update (opcode 16/69)

### Hors scope

- Mode combat toggle (`RQ_AttackMode` 198, touche C)
- Attaque à distance immédiate (`Player.rangedAttack`)
- Anim attaque serveur (opcode 10001)

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Scroll fluide joueur (g_DONE = 8 frames)

**Famille : Rendu**

### Symptôme

Après autorité serveur (Phase 4c), le perso **saute d'une tuile** à l'autre — plus de défilement fluide.

### Cause

`playerX_/Y_` (serveur) mettait à jour le centre de la carte **instantanément**. Windows garde la tuile logique à jour mais **scroll visuellement** sur 8 frames (`Done = g_DONE`, `g_MOVX=4`, `g_MOVY=2` px/frame).

### Correctif

- **`playerDisplayX_/Y_`** (float) : centre visuel de la carte, lerp vers `playerX_/Y_` en **8 pas/tuile**
- Rendu map + PNJ + picking utilisent `playerDisplayX/Y` ; logique collision/move reste sur `playerX_/Y_`
- Pas suivant bloqué tant que `isPlayerScrolling()` (comme Windows `Done > 0`)
- Teleport/popup : snap immédiat display = serveur

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Fix animation marche (régression direction Phase 4c)

**Famille : Rendu**

### Symptôme

Après le fix direction, le perso avance **par accoups** et l'animation repart toujours sur la **première frame** (même jambe).

### Cause

Le fix direction avait remis direction + animation dans `tryMovePlayer` **et** `applyServerPlayerMove`, en resetant **`playerAnimTick_`** (et parfois `playerAnimUntil_` = 320 ms) à **chaque pas** :

1. `tryMovePlayer` → reset tick
2. ~50–150 ms plus tard, confirmation serveur → **second reset** tick
3. Timer 320 ms expirait entre deux pas → `playerMoving_ = false` → frame 0

Windows : `SpriteNumber` **continue** pendant les 8 frames `Done` — pas de reset par pas.

### Correctif

- Reset `playerAnimFrame_` / `playerAnimTick_` **uniquement** si arrêt ou **changement de direction**
- `applyServerPlayerMove` : ne plus toucher le tick si même direction
- Prolonger `playerAnimUntil_` tant que la flèche est enfoncée

**Accoup position** : corrigé via scroll `playerDisplayX/Y` (voir entrée scroll fluide).

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Fix direction joueur en mouvement (régression Phase 4c)

**Famille : Rendu / Réseau**

### Symptôme

Après Phase 4c (collisions / autorité serveur), le joueur restait **toujours de dos** pendant la marche, quelle que soit la flèche.

### Cause

Phase 4c avait retiré la mise à jour direction/animation de `tryMovePlayer` et ne la faisait plus que dans `applyServerPlayerMove` via `T4CDirectionFromMoveOpcode(moveOpcode)` — en lisant l'**opcode du paquet serveur**.

Or le serveur confirme **toujours** avec `__EVENT_OBJECT_MOVED` = **opcode 1** (move nord), pas avec l'opcode 1–8 envoyé par le client. Opcode 1 → direction **8** (dos) à chaque pas confirmé.

Avant Phase 4c, la direction venait de l'opcode **envoyé** dans `tryMovePlayer` (correct) ; le bug n'apparaissait qu'après le passage autorité serveur.

### Correctif (aligné Windows `MoveToPosition`)

- **`tryMovePlayer`** : direction + animation immédiates depuis l'opcode **envoyé** (prédiction visuelle)
- **`applyServerPlayerMove`** : direction depuis `T4CDirectionFromWorldDelta(dx, dy)` — pas `moveOpcode` paquet
- **PNJ distants** : delta position prioritaire sur `ev.moveOpcode`

Voir **Référence définitive — mouvement & direction** (section matrice parité).

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Collision murs + objets sol + portes (Phase 4c)

**Famille : Réseau / Gameplay / Rendu**

### Pourquoi on traversait les murs

Sur Windows, le client **n’applique pas** le déplacement localement : il envoie l’opcode 1–8 et attend la **réponse serveur** (`Packet.cpp` TFCMoveID → `World.MoveToPosition`).  
Linux appliquait le delta **avant** le serveur → le perso glissait à travers les murs jusqu’à correction (souvent invisible).

### Correctifs

- **Mouvement autorité serveur** : envoi opcode 1–8 seul ; position confirmée par paquet `__EVENT_OBJECT_MOVED`(1) + coords `(x,y)` — **pas** par l'opcode reçu pour la direction (voir référence définitive)
- **Pré-check map2** : tuiles mur (`IsBlockingMapObject`) bloquent l’envoi client
- **Objets au sol** (apparence &lt; 10001, opcode 16/60) : markers + sélection
- **Interaction** : `RQ_UseObject` (23) portes/portails map2 + objets sol ; `RQ_GetObject` (11) ramassage
- Contrôles : **clic droit** sur porte/objet, **U** utiliser, **G** ramasser

### Régression connue (corrigée le même jour)

Retirer la direction de `tryMovePlayer` sans passer au delta position → perso toujours de dos. **Ne pas** utiliser `T4CDirectionFromMoveOpcode` sur l'opcode paquet **entrant** serveur.

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — Fix gardes Olin Haad + preload creatures auto

**Famille : Rendu / Build** (`20006` → `Warrio` Windows) disparaissaient ; puppets temple OK.

### Cause

Budget decode/frame saturé (19 calques × N puppets) ; sprites creatures non préchargés ; fallback `10011→Warrio` retiré.

### Correctifs

- `T4CCollectCreaturePreloadBases()` — **160** sprites auto (`gen_creature_appearance_table.py`)
- Preload unifié classes + creatures + puppets
- Budget decode : 512 → **2048**
- Fallback puppet → `GuardModel1` / `Cleric` / `Warrio` si draw 3D échoue
- `gen_creature_appearance_table.py` dans `build_client.sh`

**Pas de mapping manuel par PNJ** — deux générateurs depuis `VisualObjectList.cpp` :

| Type | Script | Volume |
|------|--------|--------|
| Puppet habillé | `gen_puppet_tables.py` | ~705 mappings |
| Creature / monstre | `gen_creature_appearance_table.py` | ~160 apparences |

**Rebuild :** `./scripts/build_client.sh`

---



**Famille : Rendu**

### map2 (objets / batiments)

- **`T4CV2WorldMap`** charge `v2_*map.map2` (meme index RLE que map1)
- **`GetObjectTileId`** + 2e passe de rendu iso (apres le sol)
- **`T4CMapObjectSprites`** : mapping ID → nom sprite V2 (chateau Lighthaven 5678–5792, tapis, bancs)
- Preload etendu aux sprites objets visibles au spawn

### Sprite joueur / unites (2e)

- **`TryDrawSpriteByName`** sur l'atlas V2
- Joueur local : **`T4CPlayerSpriteNpcName`** (Warrio/Wizard/Cleric/Thief) — fallback point jaune si decode echoue
- Unites distantes : **`T4CSpriteNameFromAppearance`** (NPC + monstres courants)
- Preload des 4 sprites de classe au chargement

### Limite connue

- Mapping objets partiel (~100 IDs chateau + decor proche spawn) — pas les 16384 tuiles `DrawTileSet`
- Sprites 3D puppet : **19 calques** via tables Puppet.cpp (Phase 3b) — frame statique direction sud par défaut

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 — CHECKPOINT BASELINE (ne pas régresser)

**Famille : Décision / Doc**

**État de référence validé en jeu** (Lighthaven, compte perso, serveur `192.168.1.76:11677`) :

| Flux | Attendu |
|------|---------|
| Auth | `14 → 99 → 26 → 103 → 153` sans `TFCPacketException` |
| Sélection perso | Liste 3 persos + équipement (opcode 153) |
| Entrée monde | `PutPlayerInGame OK @ 2944,1059 w=0` puis `60 + 46` |
| Chargement | **Écran « Chargement du monde… X% »** puis carte **immédiate** |
| Rendu sol | **Sprites V2 terrain** + **objets map2** (chateau Lighthaven si IDs connus) |
| Joueur | Sprite classe V2 si decode OK, sinon point jaune (fallback) |
| Musique | Sadness (sélection) → Outdoors (monde) |
| Réseau en jeu | Opcode 16 (~101 unités), 43/44 stats, 169 keep-alive — normal |

**Régression interdite à partir de ce point :** auth bloquée sur 26, minute de losanges avant sol, lookup DID linéaire 130k, absence d’écran de chargement.

**Test rapide après tout changement rendu/réseau :**

```bash
./scripts/build_client.sh && ./scripts/run_client.sh
# 1) Login → liste persos  2) Entrer « tomy »  3) Barre chargement < ~15 s  4) Terrain sprite visible
```

**Prochaine session :** map2 / objets / bâtiments, sprite joueur (plus point jaune), opcodes HUD non gérés — **sans casser** ce baseline.

---

## 2026-06-04 03:15:00 — Fix chargement monde : terrain instantané + écran loading

**Famille : Rendu / GUI**

### Symptôme utilisateur

- Auth et entrée monde OK (logs `PutPlayerInGame OK`, atlas V2 chargé)
- **~1 minute** avant apparition carte ; sol **pixelisé / losanges** comme si rien n’était décodé
- Musique Outdoors dès la sélection perso, écran vide ou placeholder entre-temps

### Causes racines

1. **`findEntry()` linéaire** sur **130 131 entrées** `.did` — par tuile visible → CPU énorme (~minutes)
2. **Budget 64 décodes/frame** — sprites non décodés → fallback losanges TMI colorés
3. **Pas d’écran de chargement** — preload invisible, impression de freeze

### Correctifs

- **Index hash** `didByName_` — lookup sprite O(1) (`T4CV2SpriteAtlas`)
- **Preload viewport** au spawn : collecte noms uniques (`CollectViewportSpriteNames`), preload banques `.dda` (`PreloadBanksForNames`), decode par lots (`PreloadSprites`)
- **Phase `WorldLoading`** dans `main.cpp` — barre progression + fond launcher (`GameWorldScreen::RenderLoading`)
- **`BeginLoad` / `PollLoad`** — monde affiché seulement quand sprites visibles prêts
- Budget runtime **512/frame** pour tuiles hors cache au déplacement

### Résultat validé

- Chargement **immédiat** après la barre ; **terrain sprite V2** de nouveau visible (Lighthaven)

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 02:50:00 — Fix auth bloquée : opcode 26 liste persos

**Famille : Réseau**

### Symptôme

```
[PHASE] -> RQ_GetPersonnalPClist (26)
[NET] opcode 103 (0 octets)
[NET] TFCPacketException opcode=26 pos=35 size=39 cause=1
[NET] opcode 153 (0 octets)
```

Connexion coupée (`T4CCommBridgeStop`) — écran login bloqué.

### Cause

Parse opcode **26** : le serveur envoie par perso un champ **`shTmp` (short=0)** après race/level (`BuildAccountPlayerList` T4Serv3). Le client Windows lit toujours ce short. Notre code :

```cpp
if (!msg->isEnd()) { msg->Get(&extra); }  // BUG
```

`isEnd()` = « position encore dans le buffer » (pas « fin de paquet ») → **`!isEnd()` faux quand il reste des octets** → `shTmp` non lu → parseur désynchronisé → underrun pos 35/39.

### Correctifs

- **`ParseCharacterList`** : lecture systématique de `shTmp` (aligné `TFCSocket.cpp` Win)
- Handlers **103** (`RQ_MaxCharactersPerAccountInfo`) et **153** (`RQ_GetPersonnalPClistEquitSkin`)

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 02:35:00 — Mapping sol NMS complet + ancrage sprites

**Famille : Rendu / Décision**

### Comment le mapping fonctionne

1. `v2_worldmap.map1` → **ID tuile** par case (ex. 95, 65, 62)
2. Table recopiée du client Windows (`TFCLanddef.h` + `Tileset.cpp`) → **nom sprite** (ex. 95 → `NewTempleblack01`, 65 → `NMRPF_Grass2High`)
3. Variante 16×16 : `"Nom (x%16+1, y%16+1)"` dans `v2nmsdatai.did`
4. Décode ZIP/NCK → palette par nom → `SDL_Texture` → blit iso 32×16

### Pourquoi pas des le debut (95–97 seulement)

- Approche **incrementale** : 2a TMI → 2b carte iso → 2c sprites avec **3 tuiles** pour valider le pipeline (probe `tuile 95 OK`)
- Lighthaven = mix de tuiles (**~44 %** tuile 95, **~30 %** herbe 65, roche 62, bois 87–91) — losanges TMI dominaient tant que le mapping etait partiel
- Bugs decouverts apres : format **ZIP (9)** (pas NCK), **casse .dda** Linux, **ancrage** sprite (centre iso)

### Correctifs

- **`TileSpriteBase`** : IDs sol NMS **5–131** (herbe, roche, bois, temple, donjon, marécage)
- Ancrage sprite : centre tuile iso + offsets `shOffX1/Y1`
- Fallback losange : remplissage plat sans contour wireframe

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 02:10:00 — Fix sprites V2 : casse fichiers .dda (Linux)

**Famille : Rendu**

- **Cause pixelisation** : banques `v2nmsdata05.dda` introuvables alors que `V2NMSData05.dda` existe (Linux sensible a la casse) → decode sprite echouait → fallback losanges TMI colores
- **`T4CResolveGameFilesPath`** : resolution insensible a la casse sous `Game Files/`
- **`T4CV2SpriteAtlas`** : charge `.did/.dda/.dpd` via ce resolver ; log banques non-vides

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 02:20:00 — Fix decode sprites tuiles : format ZIP (comp=9)

**Famille : Rendu**

- **Cause reelle pixelisation** : tuiles Lighthaven (`NewTempleblack01`) sont en **COMP_ZIP (9)**, pas NCK — le decodeur les rejetait → fallback losanges TMI
- Support **ZIP + NCK + DD** ; palettes par nom sprite (`GetPal` style Windows)
- Probe au demarrage : log `probe tuile 95 OK` ou `ECHEC`

**Rebuild :** `./scripts/build_client.sh`

---

## 2026-06-04 01:25:00 — Phase 2b/2c : remplissage tuiles + debut sprites V2

**Famille : Rendu**

- **Fix wireframe** : `SDL_FColor` en 0-1 ; remplissage par scanlines
- **Couleurs TMI** sous les tuiles iso (herbe/pierre via `TMI_Map.dat`)
- **`T4CV2SpriteAtlas`** : charge `.did/.dda/.dpd` NMS, decode NCK → `SDL_Texture`
- Tuiles **95-97** (`NewTempleblack01-03`) — mapping minimal Lighthaven (etendu ensuite en 02:35)

**Rebuild :** `./scripts/build_client.sh`

---

**Famille : Rendu**

- `T4CMapRle` — decompression RLE (LASTONE=16384 pour map1/map2)
- `T4CV2WorldMap` — lecteur `v2_*map.map1`, cache chunks 128x128, IDs tuile sol
- `GameWorldScreen` — rendu **isometrique** 32x16 (losanges colores par ID tuile)
- Priorite : V2 map > TMI > grille
- **Phase 2c** suivante : sprites V2 (.did/.dda) a la place des losanges

**Rebuild :** `./scripts/build_client.sh`

**Note reconnexion :** `./scripts/build_server.sh` puis redemarrer `T4CServer` sur 192.168.1.76. Correctif serveur : `ForceReconnectPurge` (IP/port) + `ForceReconnectPurgeByAccount` (nouveau port UDP / compte fantome).

---

## 2026-06-04 01:00:00 — Reconnexion apres deconnexion (serveur + client)

**Famille : Réseau / Serveur**

### Symptome

- Apres retour login ou Ctrl+C : client log `[AUTH] -> RQ_RegisterAccount (14)` **sans reponse** (pas de 99/26)
- Serveur : parfois `[DEADLOCK] UDPReceivePacketThread` (bruit connu, non fatal)

### Cause racine serveur

`RQFUNC_RegisterAccount` : si `IsPlayerResourceExist(sockAddrO)` (session fantome meme IP/port), le handler **retournait sans envoyer de paquet** (commentaire « Resending authentication agreement » jamais implemente).

### Correctif serveur (T4Serv3)

- `CPlayerManager::ForceReconnectPurge()` — purge synchrone session fantome (meme IP/port UDP)
- `CPlayerManager::ForceReconnectPurgeByAccount()` — purge par login (client relance, nouveau port)
- `RQFUNC_RegisterAccount` : purge IP/port + purge compte avant auth async
- `Players::AccountLogged` : purge in-memory synchrone au lieu de `FlagDeleteByAccount` async

### Correctif client

- `T4CLoginSessionDisconnectInGame()` : poll ExitGame + **ferme UDP** (`T4CCommBridgeStop`)
- `T4CLoginSessionStart()` : `g_boQuitApp = false` explicite
- `Comm.cpp` : log stderr si paquet jete par `g_boQuitApp`

**Rebuild :** `./scripts/build_client.sh && ./scripts/build_server.sh`  
**Test :** `T4C_E2E_RECONNECT=1 ./build/linux/t4c_e2e_auth_probe 127.0.0.1 11677 test test`

---

## 2026-06-04 00:10:00 — Phase 2a demarree : carte TMI en jeu

**Famille : Rendu**

- Nouveau module `src/assets/map/T4CTmiWorldMap.cpp` — lit `Game Files/TMI_Map.dat` (zlib, 3072×3072 + palette)
- `GameWorldScreen` affiche la **vraie couleur de sol** autour du joueur (minimap serveur) au lieu de la grille verte si TMI charge
- Fallback grille si `TMI_Map.dat` absent
- **Pas encore** : tuiles isometriques 32×16, sprites V2 (`.did/.dda`) — etapes 2b–2d

**Rebuild :** `./scripts/build_client.sh`

---

**Famille : Réseau / GUI / Serveur**

### Symptome

- 1re session OK ; **Ctrl+C** puis relance → auth bloquee sur opcode **14** seul (pas de 99/26)
- Obligation de **redemarrer le serveur** parfois

### Cause racine client (bug **specifique port Linux**)

`T4CCommBridgeStop()` mettait `g_boQuitApp = true` sans jamais le remettre a `false`.

Dans `Comm.cpp`, si `g_boQuitApp` : **`SendPacket` ignore tout sauf opcode 20** (`RQ_ExitGame`).

→ Apres la 1re fermeture UDP, le paquet **14 RegisterAccount** etait **silencieusement jete** alors que le log affichait quand meme `[AUTH] -> RQ_RegisterAccount`.

**Correctif :**

- `T4CCommBridgeStart()` : `g_boQuitApp = false`
- `T4CCommBridgeStop()` : ne touche plus a `g_boQuitApp`
- `T4CLoginSessionShutdown()` : envoie **RQ_ExitGame (20)** si deja authentifie (`pipelineStep >= 2`), poll UDP, puis `g_boQuitApp = true` (quit app seulement)
- **SIGINT/SIGTERM** dans `main.cpp` → boucle propre + shutdown reseau
- Message d'erreur auth visible sur l'ecran login (`T4CLoginSessionGetLastAuthError`)

### Cause serveur (existe aussi sur Windows, aggrave par quit brutal)

Session fantome : `OnlineUsers` + objet `Players` en memoire si pas de `RQ_ExitGame`.

**Correctifs serveur deja en place / renforces :**

- `AccountLogged()` : DELETE stale + retry INSERT
- **Nouveau** : `CPlayerManager::FlagDeleteByAccount()` — marque les sessions fantome pour suppression quand INSERT echoue
- Rebuild serveur sur la machine **192.168.1.76** requis

### Ecran monde « fond noir quadrille vert + point blanc »

**Comportement normal Phase 1** — ce n'est pas un bug reseau :

| Element | Signification |
|---------|----------------|
| Fond sombre + grille verte | `GameWorldScreen` placeholder |
| Point central (jaune/blanc) | Votre personnage @ 2944,1059 |
| Points bleus | Autres unites (opcode 16, ~101 recues) |
| Musique « Outdoors Music » | Entree monde OK |
| HUD | `Phase 1 — grille placeholder (carte V2 = Phase 2)` |

La vraie carte Lighthaven = **Phase 2** (decodeur `.map*` + sprites V2).

### Linux vs Windows — synthese

| Probleme | Linux-only ? |
|----------|----------------|
| `g_boQuitApp` bloque reconnexion | **Oui** (bug port) |
| Ghost session sans ExitGame | **Non** (Win aussi si kill brutal) |
| Grille placeholder | **Oui — Linux seulement** (Win = tuiles V2 ; TMI 2a en cours) |
| `TFCPacket Get(long)` 4 octets LP64 | **Oui** |
| ODBC MariaDB curseur | **Oui** (serveur Linux) |
| Questionnaire 5/4, RenderPresent monde | **Oui** (GUI SDL3 port) |
| Cle session PutPlayerInGame | **Oui** (oubli port initial) |

---

## 2026-06-03 23:25:00 — Entree monde GUI : affichage bloque apres opcode 13 OK

**Famille : GUI / Rendu / Réseau**

### Symptome utilisateur

- Logs reseau OK : `PutPlayerInGame OK @ 2944,1059`, puis `60 + skill list + 46`, `FromPreInGameToInGame (46) code=0`
- Fenetre **retrecit** (800×600 → 1280×720 logical) mais l'utilisateur reste visuellement sur l'ecran **creation perso** (ou frame figee)
- Opcode **169** (`RQ_RPStatus`) toutes les 5 s = keep-alive serveur, **normal** en jeu

### Causes racines (client `src/main.cpp`)

| Bug | Effet |
|-----|--------|
| Phase **World** : `world.Render()` sans **`SDL_RenderPresent()`** | Aucune frame monde affichee — derniere frame creation perso reste a l'ecran |
| Phase **CharacterCreate** : dessin creation **meme apres** `phase = World` | Fenetre agrandie/retrecie mais UI creation toujours dessinee par-dessus |

### Correctifs

- Helper `TryTransitionToWorld()` : resize fenetre, `GameWorldScreen::Init`, musique zone (`T4CGameMusic::LoadNewSound`), log `[Main] entree monde OK`
- Garde `if (phase == CharacterCreate)` avant le rendu creation
- **`SDL_RenderPresent(renderer)`** dans la boucle World

### Correctifs session precedente (meme flux entree monde)

- **`SendPutPlayerInGame`** : ajout `g_sessionKey` (long) — manquant vs client Windows (`Player.lKey`)
- Questionnaire : **4 questions** (`kQuestionsAsked = 4`), fix ecran vide « Question 5/4 » apres la 4e reponse

### Ce que l'utilisateur doit voir apres rebuild

- Fond **vert fonce** + grille, cercle jaune (joueur), points bleus (unites distantes opcode 16)
- HUD : `Monde 0 @ X,Y (fleches=deplacer, Esc=login)`
- **Pas** encore la carte Lighthaven reelle — `GameWorldScreen` reste un **placeholder Phase 1** (Phase 2 = decodeur V2 + tuiles)

### Test headless

```bash
T4C_E2E_ENTER_WORLD=1 ./build/linux/t4c_e2e_auth_probe 127.0.0.1 11677 test test
# Attendu : [E2E] OK PutPlayerInGame @ ...
```

---

## 2026-06-03 22:15:00 — Reconnexion auth (ghost OnlineUsers)

**Famille : Serveur / Réseau**

- Symptôme : 2ᵉ login → `[AUTH] reject: AccountLogged('test')` après quit client sans déconnexion propre
- Fix serveur `T4Serv3/PLAYERS.CPP` : purge ligne `OnlineUsers` stale + retry INSERT
- Rebuild serveur requis : `./scripts/build_server.sh`

---

## 2026-06-03 22:15:00 — Plan Phase 2 rendu + parité visuelle

**Famille : Rendu / Décision / Doc**

### Objectif

Remplacer `GameWorldScreen` (grille SDL placeholder) par le **pipeline graphique Vircom 1.72 natif** : mêmes fichiers `runtime/Game Files/`, décodés en mémoire, dessinés via **SDL3** (textures, pas DirectDraw).

### Ce qu'il faudra décoder (sources = `client/T4C Client/` + assets)

| Couche | Fichiers runtime | Code référence Windows | Module Linux cible | Sortie en mémoire |
|--------|------------------|----------------------|--------------------|-------------------|
| **Sprites V2** | `V2DataI.did`, `V2Data*.dda`, `V2ColorI.dpd` | `V2Database.cpp`, `V2PalManager.cpp` | `src/assets/v2/V2Database.cpp` | Index sprite → pixels RGBA + palette |
| **Sprites NM** | `V2NMSDataI.did`, `v2nmsdata*.dda`, `v2nmscolori.dpd` | idem | idem | Variantes jour/nuit / effets |
| **Cartes tuiles** | `v2_worldmap.map*`, `mapa/mapm/mapl/mapx`, … | `TileSet.cpp`, `MapInterface` | `src/assets/map/MapLoader.cpp` | Tuile (x,y) → id sol + flags collision |
| **Métadonnées carte** | `SmoothTiles.bin`, `Map.ID*`, `Zone_Map.dat`, `maps.dat` | `TileSet`, `GameMusic` zones | `src/assets/map/MapMeta.cpp` | Lissage, zones musique, transitions |
| **Objets décor** | `cliitmpd.bin`, lumières `Light*.dat` | `VisualObjectList`, lights | `src/render/` (phase 3) | Sprites au sol, halos |
| **UI launcher** | `Images/*.pcx`, `Fonts/*.ttf` | `Launcher`, PCX loader | `src/gui/` (existant) | Surfaces SDL (partiel) |
| **Textes** | `*.elng` | `IntlText` / GUI strings | futur `src/assets/elng/` | Chaînes UTF-8 |
| **Audio** | `Music Files/*.wav`, `FX Files/*.wav` | `GameMusic.cpp` | `src/audio/` ✅ musiques zone | SDL3 `AudioStream` |

**Pas de conversion TnC** : pas de `.dec`, `.rmap`, mestoph — lecture directe des binaires Vircom.

### Pipeline cible (ordre d'implémentation)

```text
1. V2Database     — ouvrir .did/.dda/.dpd, GetSprite(id) → SDL_Surface/RGBA
2. MapLoader      — charger map1/map2/… pour monde 0, GetTile(x,y)
3. MapRenderer    — viewport caméra, blit tuiles + calques (SDL3 textures)
4. UnitRenderer   — sprites perso/NPC (opcodes 69/70/16 déjà reçus réseau)
5. UI in-game     — inventaire, barres stats (réutiliser `.elng` + sprites UI V2)
6. Effets         — sorts, météo, lumières dynamiques
```

**Critères intermédiaires :**

- **2a** : tuiles Lighthaven visibles (~2880, 1083), caméra fixe
- **2b** : 1 sprite joueur animé (apparence réseau)
- **2c** : scroll + collision map
- **2d** : NPC/objets peripheriques (opcode 16)

### Parité « 100 % » — réponse honnête

| Aspect | Parité réaliste | Commentaire |
|--------|-----------------|-------------|
| **Tuiles + sprites V2** | **~95–100 %** | Mêmes assets, même algo de décodage → pixels identiques si palette/blit corrects |
| **Cartes / collision** | **~95 %** | Même `map*` ; risque d'écarts sur cas limites (extensions, LeoWorld) |
| **UI menus / HUD** | **~80–90 %** | SDL3 ≠ DirectDraw pixel-perfect ; polices TTF vs raster possible |
| **Effets sorts / météo** | **~70 %** | Beaucoup de cas dans `VisualObjectList` — port progressif |
| **Performance** | Variable | SDL3 software/GPU vs DirectDraw ; optimiser plus tard |
| **Audio SFX combat** | **~90 %** | WAV OK ; 143 fichiers dans `FX Files/` à brancher |

**Conclusion :** un rendu **visuellement équivalent** (même carte, mêmes sprites, même gameplay visible) est **atteignable** en portant les décodeurs Vircom + SDL3. Une parité **bit-à-bit / pixel-perfect Windows** sur tous les effets edge-case n'est **pas garantie** sans longue phase de polish — but raisonnable : **Lighthaven jouable indistinguishable du client Win** pour un joueur normal.

### Hors scope Phase 2 (Phase 3+)

- Inventaire complet, skills, commerce NPC
- Opcodes HUD restants (37, 39, 44, 53, 68, 151, 154, …) — **103/153/26 auth gérés** (2026-06-04)
- Client Windows `T4C.EXE` comme test de régression visuelle

---

## 2026-06-03 22:15:00 — Sons WAV finalstep + scripts CRLF

**Famille : Assets / Build**

- `scripts/copy_sons_wav.sh` : 7 musiques → `Music Files/`, 143 SFX → `FX Files/` (source `finalstep/.../data/sons`)
- `scripts/*.sh` : normalisation fin de ligne LF (`bash\r` corrigé)
- SDL3 lit **WAV** natif (pas MP3 sans bibliothèque additionnelle)

---

## 2026-06-03 19:37:18 — Auth E2E PASS (14 → 99 → 26)

**Famille : Réseau / Serveur / E2E**

- **`scripts/run_e2e_test.sh`** : **PASS** compte `test/test`, port **11677** (session `20260603_193706`)
- **Serveur** : curseur ODBC `SQL_CUR_USE_DRIVER` (MariaDB) ; `Version=1720` ; lecture auth via `ODBCUsers`
- **Client** : `TFCPacket::Get(long)` wire **4 octets** sur LP64 (fix crash opcode 14) ; catch `TFCPacketException`
- Sonde : `build/linux/t4c_e2e_auth_probe`

---

## 2026-06-03 05:56:30 — Serveur : boot WDA LP64 débloqué

**Famille : Serveur / E2E**

- Port loader WDA LP64 appliqué côté **`FinalStEp0/T4Serv3`** (spells, objects, creatures, `WDAFile::Seek/Tell`)
- Crash session 9 (`WDASpells` assert bool) **corrigé** — désync format Win32 vs WDA LP64
- Serveur atteint le port **11677** après init WDA (~2 min en dev)
- **`T4C_SKIP_CREATURES`** : variable optionnelle serveur (dev perf) — **non** utilisée par `run_e2e_test.sh`
- Boot E2E complet (créatures incluses) : prévoir **`T4C_E2E_SERVER_WAIT=600`** (lecture WDA lente)

---

## 2026-06-02 — Phase 1 Linux : client testable de bout en bout

**Famille : Build / Réseau / GUI**

- **Build vert** : `build/linux/t4c_client` compile et lie sans erreur
- **Auth 1.72** : `14 → 99 → 26 → 13 → 60/39/46` via stack `COMM` + `NMPacketManager` (`CLT_VERSION=1720`)
- **Parsing paquets corrigé** : skip opcode systématique (`SkipOpcode`) sur toutes les réponses serveur
- **Création perso** : `RQ_CreatePlayer (25)`, reroll `(31)`, validation → liste `26` → `13`
- **Suppression perso** : `RQ_DeletePlayer (15)` + refresh `26`
- **Monde** : déplacement opcodes `1–8`, unités distantes (`16`, `60`, `69`, `70`, `10004`), popup joueur local
- **UI SDL3** : login, sélection/création perso, questionnaire, écran reroll, monde grille (placeholder)
- **Scripts** : `scripts/run_client.sh`, `scripts/build_client.sh`
- **Lancement** :
  ```bash
  ./scripts/build_client.sh
  ./scripts/run_client.sh [ip_serveur] [port]
  ```
  Variable d'environnement : `T4C_RUNTIME=./runtime` (défaut)

**Prochaine étape (Phase 2)** : décodeur assets V2 natifs + rendu tuiles/sprites SDL3 (remplace la grille).

---

## 2026-06-02 — Phase 0/1 Linux : build + stack réseau 1.72

**Famille : Build / Réseau**

- `CMakeLists.txt` — binaire `t4c_client` (SDL3 + zlib + stack Vircom 1.72)
- `scripts/build_client.sh` — configure + compile dans `build/linux/`
- `client/platform/linux/` — IOCP émulé, threads/events, sockets POSIX, sync Win32
- Patches `#ifdef LINUX_PORT` : `Comm.cpp`, `TFCPacket.cpp`, `UDP/*` (pas CommCenter 1.68)
- `src/main.cpp` — fenêtre SDL3 smoke + `COMM.Create()` (NMPacketManager)
- `src/network/T4CClientStubs.cpp` — `g_boQuitApp`
- Critère atteint : **compile + lance fenêtre + init UDP**

---

**Famille : Assets**

- Création de `runtime/` : copie intégrale (**cp -a**, aucune suppression à la source) depuis Original Client 1.72 :
  - `Game Files/` (110 fichiers, ~325 Mo)
  - `Fonts/`, `Music Files/`, `FX Files/`, `Images/`
  - `english.elng`, `EnglishGUI.elng`, `french.elng`, `FrenchGUI.elng`
  - `help/`, `iplist.txt`, `license.txt`
- Total local : ~**401 Mo**
- Script reproductible : `scripts/copy_runtime_assets.sh`
- Guide : `docs/RUNTIME_ASSETS.md`, trace source : `runtime/SOURCE.txt`

---

## 2026-06-02 05:01:40 — Structure projet + doc architecture

**Famille : Doc**

- `README.md` racine — vue d'ensemble
- `docs/ARCHITECTURE.md` — **pas de moteur TnC séparé** ; tout dans le client
- Arborescence cible : `client/` (Win), `src/` (Linux futur), `runtime/`, `docs/`, `scripts/`

---

## 2026-06-02 (session planification) — Décisions architecture validées

**Famille : Décision**

Résumé des conclusions de la session de planification :

| Sujet | Décision |
|---|---|
| Version cible | **Client 1.72** + serveur **2008 v1r72** + assets Original Client 1.72 |
| Abandon TnC / 1.68 comme cible finale | Le port 1.68 + `t4c-world-engine_sdl3` reste prototype ; pas de `.dec`/`.rmap` |
| Assets | **Natifs V2** — pas de conversion mestoph |
| Moteur séparé | **Non** — décodage + rendu dans le client (modules `src/`) |
| Approche Linux | **Sans shim** DirectDraw : interfaces propres (`IAssetStore`, `IMapStore`, `IRenderBackend`) |
| Dual-build | Windows = `.sln` intact ; Linux = CMake + `src/` ; builds **indépendants** |
| Problème assets 1.68 | Résolu en changeant de stack (1.72 + install Vircom propre) |

---

## 2026-06-02 (session planification) — Pourquoi TnC produisait .dec / .rmap

**Famille : Doc**

- TnC (mestoph 2005) = reverse-engineering **sans source Vircom**
- `.dec` = VSF déchiffré ; `.rmap` = carte aplatie pour `MapInterface`
- Le client officiel décode **en runtime** ; la conversion était un raccourci TnC, pas une loi T4C
- Avec source 1.72 + assets natifs : **inutile** pour FinalStEp2

---

## Antérieur (projet finalstep / 1.68) — Contexte hérité

**Famille : Réseau / Rendu** — *prototype, non repris tel quel*

Travail accompli sur `finalstep/client` (1.68 RC14h) — réutilisable comme **modèle**, pas comme binaire :

| Élément | Statut prototype 1.68 |
|---|---|
| Auth 14 → 99 → 26 → 13 | Validé |
| `T4CLoginSession`, écrans SDL3 login/persos | Validé |
| Rendu TnC + `$T4C_DATA` `.dec`/`.rmap` | Validé (carte, mobs partiels) |
| Assets | Bricolés multi-sources — **cause du pivot 1.72** |
| HUD inventaire / skills / musique | Partiel |

→ À **réadapter** pour opcodes et version **1.72**, pas copier-coller.

---

## BACKLOG — À venir (ordre recommandé)

### Phase 0 — Build ✓ partiel

- [x] `runtime/` assets copiés + guide
- [x] Doc architecture (no TnC)
- [x] `CMakeLists.txt` Linux (smoke : fenêtre SDL3 + version 1720)
- [x] Script `build_client.sh`
- [ ] Windows : valider compile VS2022 depuis `client/T4C Client.sln`

### Phase 1 — Réseau Linux (démarré)

- [x] Couche `client/platform/linux/` — IOCP, threads, sockets, sync Win32
- [x] Stack 1.72 `#ifdef LINUX_PORT` : `NMPacketManager`, `PacketCenter`, `TFCPacket`, UDP
- [x] `src/main.cpp` — fenêtre SDL3 + init `COMM.Create()`
- [x] `src/network/` — session auth 1720 (14 → 99 → 26) ; sonde headless `t4c_e2e_auth_probe`
- [x] **E2E PASS** `scripts/run_e2e_test.sh` (2026-06-03, compte test/test, port 11677)
- [ ] Écrans SDL3 : login, liste persos, création
- [ ] Critère : auth complète avec logs (GUI)

### Phase 2 — Assets + rendu (sans shim)

Voir entrée **2026-06-03 22:15:00 — Plan Phase 2 rendu** (décodeurs, pipeline, parité %).

- [x] **2a** TMI + couleurs sol (`T4CTmiWorldMap`)
- [x] **2b** `v2_worldmap.map1` + RLE + viewport iso (`T4CV2WorldMap`)
- [x] **2c** sprites sol V2 (`.did/.dda/.dpd`, ZIP/NCK, mapping 5–131, preload + loading screen) — **baseline 2026-06-04**
- [x] **2d** `map2` + objets / batiments (debut — chateau, tapis, bancs Lighthaven)
- [x] **2e** sprite joueur / NPC (debut — Warrio+classe, fallback point)
- [ ] **2f** UI in-game (inventaire, stats) — sprites UI V2 + `.elng`
- [ ] Critère **2a** : carte Lighthaven (~2944, 1059) sans grille placeholder — **✓ baseline**
- [ ] Critère **2b** : parité visuelle vs `T4C.EXE` Windows (capture comparée)

### Phase 3 — Gameplay réseau

- [ ] Mouvement 1–8, unités 69/70, objets 16
- [ ] Inventaire, stats, audio MP3
- [ ] Parité progressive vs `T4C.EXE` Windows

### Phase 4 — Serveur (hors repo client, coordonné)

- [x] Port Linux serveur T4Serv3 (~124 Mo)
- [x] `dwVersion = 1720`, ODBC MariaDB, boot `[BOOT] SERVER_READY`
- [x] Test E2E : sonde headless 1.72 → serveur Linux (11677)
- [ ] Test : Windows `T4C.EXE` + assets `runtime/` → serveur Linux

---

## 2026-06-10 (suite) — Mort → temple, SFX, contour, menu Esc, E2E combat

**Famille :** gameplay réseau / client / audio / E2E

### Mort NMS → temple Lighthaven (~2948,1041)

- Handlers **200** `RQ_NM_DeathStatus`, **201** `RQ_NM_DeathProgress`, envoi auto **202** `RQ_NM_DeathResurect` quand `canRes=1` (PvM).
- Overlay mort + timer ; clear `playerDead` sur status 0 / teleport temple / opcode 57 zone temple.
- **Cause** : sans 202 le serveur laisse le fantôme sur place (57 @ coords mort) — pas de `NMResurect`.

### Audio SFX

- Conversion WAV 8-bit → S16 avant lecture SDL (`Unsupported block alignment` corrigé).

### Rendu / UX

- Contour survol : frame sprite réelle (`Goblin090-a` / `StAtt`) via `TryDrawSpriteOutline` + miroir direction.
- Contour objets sol au survol.
- Portes : `Open Wooden Door.wav` au clic.
- Curseur gant `StaticAttackCursor` (comme T4C.EXE).
- Menu **Esc** : sac, stats/équipement, chats (stub), toggle musique/SFX, retour persos.

### PNJ / inview

- `GetNearItems` : retire les unités distantes absentes du snapshot (fix doublon garde Olin Haad figé).
- Log `[PHASE] puppet spawn` pour debug Mithrand (app 10011).

### E2E

- `./scripts/run_combat_e2e.sh` — build + régression + auth/monde + checklist mort/combat.

---

## 2026-06-10 — Combat gobs : crash mort serveur + anim/SFX attaque mobs

**Famille :** gameplay réseau / serveur / audio

### Serveur (`FinalStEp0/T4Serv3`)

- **Fix SIGSEGV mort par gobs (`DeathNMS` → `SaveToLog:368`)** :
  - `SaveToLog` : ignore chaîne vide/null ; plus de `CString::GetBuffer`/`strstr` (crash Linux) ; filtre spam en `std::string`.
  - `DeathNMS`/`DeathOld` : `_LOG_DEATH` via `"%s"` + garde `!csText.IsEmpty()` (perso niv.1 ou flush inventaire vide).
  - **Obligatoire** : recompiler `T4CServer` et redémarrer le binaire (`build/T4CServer`).

### Client (`FinalStEp2/src`)

- **Opcode 10001** : animation attaque distante (`GoblinStAtt045-a` …), direction vers la cible, ~480 ms.
- **SFX combat** : `T4CGameMusic::PlaySfx` (FX Files) — gobelin : `Whooshh 2` / `Whooshm 6` à l'attaque, `Goblin Hit` / `Male Hit` quand touché.
- Test régression `attack_sprite_frame_names`.

---

## 2026-06-09 — Mithrand, clic maintenu, doublon garde

### Client (`FinalStEp2/src`)

- **Téléport (57)** : resync opcode **46** + GetNearItems différé (serveur repasse `boPreInGame`).
- **Clic gauche maintenu** : curseurs `V3_North Cursor` … `V3_North-West Cursor` + déplacement directionnel (comme `MovePl` / `Pf.cpp` Windows).
- **Doublon PNJ au spawn** : snap `displayX/Y` si re-spawn peripheral à distance > 2 tuiles.
- Message mort **VOUS ETES MORT** déjà présent à l’overlay.

### Serveur (`FinalStEp0/T4Serv3`)

- **GetNearItems (60)** Linux : plus de réponse vide en `preInGame` — async inview comme en `in_game` (aligné Windows).

---

## Notes permanentes

- **Ne pas** réintroduire `convert2`, `.dec`, `.rmap` dans ce projet
- **Ne pas** supprimer les assets source sur disque dur — copies locales uniquement
- **Windows** reste référence comportementale jusqu'à parité Linux
- Chaque entrée future : horodater + tagger la **famille**
- **Baseline 2026-06-04** : auth 14→26 OK, écran chargement, terrain V2 au spawn — **toute PR doit préserver ce flux** (test `./scripts/run_client.sh`)
