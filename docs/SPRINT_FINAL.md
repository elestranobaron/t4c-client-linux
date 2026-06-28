# Sprint final — Parité client Linux ↔ Windows (T4C 1.72)

> Document de travail destiné à l'agent (Composer). Chaque tâche est autonome : symptôme, cause **ancrée dans le code réel**, correctif proposé (fichiers + fonctions), critères d'acceptation.
>
> **Directive générale (confirmée) :** le client Linux (`src/`) doit reproduire **à l'identique** le comportement du client Windows de référence (`client/T4C Client/`). En cas de doute sur un comportement, la **source de vérité est le code Windows**. Ne pas inventer de logique : porter/mapper celle de Windows.

## Repères architecture

- **Client Linux** : `src/` — `src/network/` (protocole, `T4CLoginSession.cpp`), `src/game/` (monde + HUD : `GameWorldScreen.cpp`, `T4CGameHud.cpp`, `T4CCharacterWindow.cpp`, `T4CPuppet.cpp`), `src/assets/map/` (sprites/tuiles : `T4CV2SpriteAtlas.cpp`, `T4CV2WorldMap.cpp`), `src/gui/` (écrans hors-jeu), `src/audio/`.
- **Client Windows (référence 1:1)** : `client/T4C Client/` — protocole/handlers `Packet.cpp`, `packethandling.cpp`, `TFCSocket.cpp` ; rendu `Disp.cpp`, `Icon3D.cpp`, `V2Sprite.cpp`, `VisualObjectList.cpp`, `Puppet.cpp` ; UI `NewInterface/RootBoxUI.cpp` + `V3_*Dlg`.
- **Transport réseau partagé** (déjà commun) : `Comm.cpp`, `TFCPacket.cpp`, `UDP/*` via shims `client/platform/linux/`.
- **Tables auto-générées** : `scripts/gen_*.py` → `*.inc` (creature appearance, ground objects, puppet). Régénérer plutôt qu'éditer à la main quand une table est en cause.

## Méthode imposée

1. **Une tâche = un commit** (ou une PR). Build après chaque tâche : `cmake --build build/linux -j$(nproc)`.
2. Avant de coder un correctif visuel, **ajouter le log de diagnostic** quand il est demandé : ça évite de patcher le mauvais chemin (cf. historique save serveur).
3. Comparer systématiquement au fichier Windows cité. Si le comportement Linux diverge, l'aligner.
4. Ne pas régresser la connexion/reconnexion ni l'entrée monde (opcodes 13/46) — déjà stabilisés.

---

## T1 — PNJ « carte verte » : sprite non résolu (ex. Dark Fang = dragon)

**Famille : Rendu**

**Symptôme :** certains PNJ/créatures s'affichent comme un bloc/placeholder au lieu de leur sprite réel. Ex. « Dark Fang » (un dragon) apparaît en carte verte.

**Cause (à confirmer par log) :** il n'existe **pas** de « carte verte » littérale dans le code. Les fallbacks visibles sont : rect 16×10 objets sol, point 8×8 unités (`drawUnitSprite` retourne `false`), losanges iso de tuile, ou **décodage/palette erroné** d'un sprite valide. Chaînes en cause (`src/game/GameWorldScreen.cpp`, `src/assets/map/T4CV2SpriteAtlas.cpp`) :
- appearance absente de `src/network/T4CCreatureAppearanceTable.inc` → `T4CSpriteNameFromAppearance` renvoie `nullptr` → point 8×8 ;
- nom absent de l'index DID, banque `.dda` manquante (casse), ou **`V2DataI.did` dont l'échec de chargement est silencieusement ignoré** (`T4CV2SpriteAtlas::Init`, retour ORI ignoré) ;
- budget de décodage par frame épuisé (`kDecodeBudgetPerFrame`).

**Correctif :**
1. **Diagnostic d'abord** : dans `T4CV2SpriteAtlas::TryDrawSpriteByName` et `decodeAndCache`, logger sur échec : nom du sprite, banque attendue, raison (index absent / banque absente / décode KO / budget). Dans `GameWorldScreen::drawUnitSprite`, logger l'appearance + nom résolu quand le rendu échoue.
2. Vérifier/contrôler le chargement de `V2DataI.did` (actuellement ignoré) — le rendre obligatoire ou au moins loggué.
3. Avec le log, identifier le vrai cas pour Dark Fang : appearance manquante dans la table (→ régénérer/compléter via `scripts/gen_creature_appearance_table.py`) **ou** palette/banque (→ corriger résolution dans `paletteForSprite` / `ensureBankLoaded`).
4. Comparer le mapping appearance→sprite à Windows `VisualObjectList.cpp` / `Icon3D.cpp`.

**Acceptation :** Dark Fang (et autres dragons listés dans `T4CCreatureAppearanceTable.inc` : 20095, 20112–20116, 20130–20136) s'affichent comme dragon. Le log liste 0 échec de résolution sur une zone de spawn standard.

---

## T2 — Coffres affichés comme des portes

**Famille : Rendu**

**Symptôme :** les coffres apparaissent en portes.

**Cause (identifiée) :** bug Linux-only. Le serveur envoie les variantes `_I` de coffre (appearance **18/19/20**). Le client Linux fait un lookup **par ID brut** dans `src/game/T4CGroundObjectSpriteTable.inc`, où 18→`RockDoor2`, 19→`RockDoor3`, 20→`RockDoor4`. Windows applique un **remap `_I` → type canonique** avant de dessiner (`VisualObjectList.cpp` ~L18376) :

| appearance `_I` | Linux dessine (faux) | Windows dessine (correct) |
|---|---|---|
| 18 `CHEST_I` | RockDoor2 | `__OBJGROUP_CHEST` (41) = « Misc 2 - All 1 » |
| 19 `CHEST_OPEN_I` | RockDoor3 | `__OBJGROUP_CHEST_OPEN` (42) = « Misc 2 - All 2 » |
| 20 `CHEST2_I` | RockDoor4 | `__OBJGROUP_CHEST2` (238) = « Chest » |

**Correctif :** ajouter le remap `_I → type de base` **avant** `T4CGroundObjectSpriteName` (dans `src/game/T4CGroundObjectSprites.cpp` ou dans `drawGroundObjectMarker`), en reprenant la table de remap complète de Windows (`VisualObjectList.cpp` draw switch + `SetMouseCursor` L156–168 : couvre coffres, portes, chaises, vaults). Idéalement, intégrer le remap dans le générateur `scripts/gen_ground_object_sprite_table.py` pour rester aligné.

**Acceptation :** coffres (ouverts/fermés, chest2) s'affichent comme coffres ; aucune régression sur les vraies portes (RockDoor) ni le SFX porte (`isDoorAppearance`).

---

## T3 — PNJ qui « sortent de nulle part » (spawns fantômes)

**Famille : Réseau / Rendu**

**Symptôme :** des PNJ apparaissent brutalement à des positions incohérentes, ou des fantômes persistent.

**Cause (identifiée) :** dans `src/network/T4CLoginSession.cpp` :
- l'opcode 16 (`RQ_SendPeriphericObjects`) est traité **uniquement en additif** (`HandleObjectAppearedList`), y compris les bandes périphériques directionnelles à 1 entrée ;
- `g_expectInviewRefresh` est posé à l'envoi de GetNearItems mais **jamais consommé** → le bloc de purge des unités obsolètes (~L2306–2321) est **mort** ;
- côté rendu, un ré-spawn périphérique avec gros delta **snap** la position (`GameWorldScreen::syncRemoteUnitsFromNetwork` ~L783) au lieu d'un déplacement ;
- `IsLocalPlayerMoveTarget` route les move-acks **par tuile uniquement** → peut avaler un move PNJ adjacent au joueur.

**Correctif (aligner sur Windows `TFCAddObject` / `VisualObjectList`) :**
1. Consommer `g_expectInviewRefresh` : quand une vraie liste inview complète arrive (réponse GetNearItems opcode 60→16), purger les unités hors liste ; ne **pas** purger sur les bandes périphériques additives.
2. Durcir `IsLocalPlayerMoveTarget` : comparer l'`unitId` du joueur local, pas seulement la tuile.
3. Vérifier la couverture de la suppression (opcode 11) + cull distance ; conserver le snap uniquement sur (re)spawn réel, pas sur ré-annonce périphérique proche.

**Acceptation :** sur une zone peuplée (place de Lighthaven), pas d'apparition téléportée d'un PNJ déjà visible ; pas de fantôme après qu'un PNJ s'éloigne ; les moves PNJ adjacents au joueur ne sont plus avalés.

---

## T4 — Ombre de PNJ ne correspondant pas à l'apparence

**Famille : Rendu**

**Symptôme :** l'ombre d'un PNJ ne colle pas à sa silhouette.

**Cause (identifiée) :** l'ombre n'est **pas** un système séparé — elle est rasterisée dans la texture du sprite (NCK `shadowMask`, `T4CV2SpriteAtlas::createTextureFromDecoded`). Toute divergence vient donc d'une **mauvaise résolution du nom de sprite** :
- avant l'opcode 68 (`HandlePuppetInformation`), un puppet est rendu via un fallback (silhouette Cleric/WHITEROBE) → l'ombre suit le fallback ;
- divergence **outline vs corps** : `drawUnitOutline` peut utiliser `T4CPuppetFallbackSpriteName` (un seul sprite) tandis que `drawUnitSprite` compose 19 calques puppet → ombres incohérentes ;
- entrée erronée/absente dans `T4CCreatureAppearanceTable.inc`.

**Correctif :** garantir que **corps et outline utilisent le même chemin de résolution** une fois l'opcode 68 reçu ; réduire la fenêtre de fallback (demander/attendre le dress puppet avant de figer une silhouette). S'aligner sur Windows `Puppet.cpp::SetPuppet` + ordre des calques `T4CPuppetBodyOrder.inc` vs `BodyOrderR` Windows. Dépend de T1 (résolution correcte des sprites).

**Acceptation :** un PNJ habillé (puppet) a une ombre cohérente avec sa silhouette finale ; pas d'ombre « Cleric » résiduelle après réception du dress.

---

## T5 — Discuter avec les PNJ : phrases tapées ignorées

**Famille : Réseau**

**Symptôme :** le PNJ « n'écoute pas » les phrases écrites (mots-clés type `acheter`, `oui`…). Côté serveur, aucun `[DirectTalk]` n'est loggué pour le texte tapé.

**Cause (identifiée) :** dans `src/network/T4CLoginSession.cpp` / `src/game/GameWorldScreen.cpp` :
- `tryTalkUnit` envoie bien un `RQ_DirectedTalk` (opcode 30) **vide** au clic PNJ (ouvre le dialogue) ;
- mais `sendChatLine` (Entrée) envoie **toujours** `RQ_IndirectTalk` (opcode 27), **sans jamais vérifier `g_talkState.active`** → le texte n'est pas routé vers le PNJ.

Windows (`client/T4C Client/main2.cpp` ~L2475–2537) : si une conversation PNJ est active (`TalkToX/Y && Objects.FoundID(TalkToID)`), le texte tapé part en **`RQ_DirectedTalk` (30) + chaîne** ; sinon `RQ_IndirectTalk` (27).

**Correctif :** dans `sendChatLine`, si `g_talkState.active` (conversation PNJ en cours), envoyer `RQ_DirectedTalk` (30) **avec le texte** (étendre `T4CLoginSessionSendDirectedTalk` qui termine actuellement par `short 0`) en miroir de `main2.cpp`. Conserver `:` → Shout (28), `/Nom` → Page (29), et 27 hors conversation. Réception déjà OK (`HandleIndirectTalk` + `renderDialoguePanel`).

**Acceptation :** en conversation avec un marchand, taper `acheter` puis `oui` déclenche les réponses serveur ; le serveur logge `[DirectTalk] '...' -> 'Npc' ... msg='acheter'`.

---

## T6 — Animation de déplacement du perso : 1–2 frames manquantes

**Famille : Rendu**

**Symptôme :** le cycle de marche du personnage principal saute des images.

**Cause (identifiée) :** dans `src/game/` :
- le préchargement (`T4CAppendUnitSpritePreload`) ne couvre que les frames **0,1,2** par angle ; les frames 3–8 se décodent à la volée → échec transitoire (budget/banque) → `tryDrawNpcBase` retombe sur le nom de base sans suffixe (pose figée) ;
- distinction marche/idle imparfaite : à l'arrêt, Linux force frame 0 alors que Windows `ST_STANDING` joue un cycle idle (`View[StMov]`).

Référence Windows : `Icon3D::DrawSprite3D` (`ST_MOVING` utilise `View045[Frame]`, cycle 9 frames `a..i`).

**Correctif :** précharger les **9 frames** par angle des bases jouables (ou précharger à la première apparition de l'unité) ; vérifier `T4CUnitSpriteFrameName` (cycle a..i complet) et le tick d'animation (`updatePlayerScroll`, `kT4CWalkAnimFrames=9`) ; aligner idle vs marche sur Windows. Tests existants à conserver/étendre : `src/tests/t4c_regression_tests.cpp` (`walk_anim_nine_frame_cycle`, `walk_sprite_frame_names`).

**Acceptation :** marche fluide 9 frames dans les 8 directions, sans frame manquante ni pose figée ; comportement idle conforme à Windows.

---

## T7 — Objets/inventaire incomplets

**Famille : Rendu / UI**

**Symptôme :** « il manque des bons objets à compléter » (objets sol/inventaire incomplets).

**Cause :** `src/game/T4CGroundObjectSprites.cpp` (+ `.inc`, 1308 lignes) couvre l'essentiel mais : pas d'animation d'objets sol (cycle `Faces`), pas de variantes miroir `_I` pour objets serveur, icônes d'inventaire réutilisant les sprites sol avec fallback brun 18×18 (`T4CCharacterWindow::drawItemIcon`). Données inventaire présentes (`T4CPlayerInventory.h`) mais UI partielle.

**Correctif :** compléter via le générateur `scripts/gen_ground_object_sprite_table.py` (re-balayer `VisualObjectList.cpp`/`Apparence.h`) ; ajouter le support des frames animées d'objets sol (parité Windows) ; vérifier les icônes inventaire manquantes. Lié à T2 (remap `_I`) et T9 (UI inventaire).

**Acceptation :** les objets au sol et icônes d'inventaire d'une zone test s'affichent tous correctement (0 fallback brun/rect sur objets connus).

---

## T8 — HUD incomplet / moins instinctif

**Famille : UI**

**Symptôme :** HUD incomplet et moins intuitif que l'original.

**Cause (identifiée) :** Linux n'a **pas** porté le framework UI Windows `NewInterface/RootBoxUI` + widgets ; le HUD actuel (`T4CGameHud`, `T4CCharacterWindow`, panneaux inline de `GameWorldScreen`) couvre ~15–20 % de la parité visuelle/comportementale. Manques majeurs : barre macro (cases vides, pas de drag-drop), saisie chat sur la barre + système de canaux, panneau cible/effets/groupe, popups objet, confirm/ask-value, carte monde, options.

**Correctif (gros morceau — découper en sous-tâches, Tier A en priorité) :**
1. Porter/adapter SDL la base UI : `NewInterface/RootBoxUI` + widgets (`BoxUI`, `ButtonUI`, `GridUI`, `DropZoneUI`, `EditUI`, `ListUI`, `TextPageUI`).
2. Re-loger le dessin existant (`T4CGameHud`/`T4CCharacterWindow`) dans des classes de dialogue.
3. Compléter `V3_LifeDlg` (portrait, panneau mort, bouton restore), `V3_MainBarDlg` (saisie chat + canaux + barre macro), `V3_TMIDlg`, `V3_ChatLogDlg`+`V3_ChatDlg`, `V3_TargetDlg`, `V3_StatDlg`/`V3_InvDlg`/`V3_SpellDlg` (drag-drop, icônes, règles de ciblage), `V3_MacroDlg`, `V3_ItemInfoDlg`.
4. Câbler les handlers d'opcodes (`T4CLoginSession`) vers les `Update*` des dialogues, en miroir de `Packet.cpp` → `V3_*Dlg`.

**Acceptation :** HUD visuellement et ergonomiquement aligné sur Windows pour le gameplay quotidien (vie/mort, barre principale + macros, chat + canaux, cible, inventaire drag-drop, sorts). Voir la liste complète des 49 `V3_*Dlg` et leur statut dans la cartographie ci-dessous.

---

## T9 — Directive de fond : porter Windows → Linux à l'identique

**Famille : Décision / Tous domaines**

**Confirmé :** oui, il faut porter le reste du fonctionnement client Windows sur Linux **à l'identique**. Le client Linux ne partage aujourd'hui que la pile réseau ; tout le reste (`NewInterface/`, `Disp.cpp`, `Icon3D.cpp`, `Puppet.cpp` Windows, handlers de `Packet.cpp`/`packethandling.cpp`, audio, input) est réimplémenté partiellement.

**Plan de portage par paliers :**
- **Tier A — Parité HUD cœur** : `RootBoxUI` + widgets, `V3_LifeDlg`, `V3_MainBarDlg`, `V3_TMIDlg`, `V3_ChatLogDlg`/`V3_ChatDlg`, `V3_TargetDlg`, `V3_StatDlg`/`V3_InvDlg`/`V3_SpellDlg`, `V3_MacroDlg`, `V3_ItemInfoDlg`. (cf. T8)
- **Tier B — Dialogues gameplay courants** : `V3_BuyDlg`/`V3_BoutiqueDlg`/`V3_TrainDlg`, `V3_ChestDlg`, `V3_TradeDlg`, `V3_ConfirmDlg`/`V3_AskValueDlg`/`V3_ComboDlg`, `V3_OptionsDlg`, `V3_EffectStatusDlg`, `V3_GroupOSDlg`/`V3_GroupeDlg`, `V3_RTMapDlg`, `V3_QuestBookP1/2/3`.
- **Tier C — Fidélité rendu monde** : `SmoothTile/*`, `LightMap.cpp`, `weather.cpp`, alignement `Disp.cpp`/`Icon3D.cpp`/`VisualObjectList.cpp`.
- **Tier D — Input / logique** : `MouseAction.cpp`/`Mouse.cpp`/`VKey.cpp`, `MacroHandler.cpp`, `Spell.cpp`, handlers restants de `Packet.cpp`/`packethandling.cpp`.
- **Tier E — Secondaire** : guilde, hôtel des ventes, arène, GM, profession, RP/PVP, MP3 panel, mail, vidéo, sauvegarde réglages.

**Acceptation :** chaque palier livré rapproche le comportement de Windows ; aucune divergence fonctionnelle introduite sur les opcodes déjà gérés.

---

## Ordre d'exécution recommandé

1. **T1** (diagnostic sprites + Dark Fang) — débloque T4 et révèle l'ampleur des cartes vertes.
2. **T2** (coffres↔portes) — correctif net et localisé.
3. **T5** (chat PNJ) — petit changement, fort impact gameplay.
4. **T3** (spawns fantômes) — réseau, qualité de jeu.
5. **T6** (anim marche) — préchargement frames.
6. **T7** (objets/inventaire) — complétude tables.
7. **T4** (ombres) — dépend de T1.
8. **T8 / T9** (HUD + portage Windows complet) — gros chantier par paliers (A→E).

## Garde-fous

- Toujours rebuild + lancer le client après chaque tâche ; vérifier en jeu sur une zone de spawn dense.
- Ne pas casser : login, sélection/création perso, entrée monde (13/46), reconnexion, sauvegarde serveur.
- Comparer au code Windows cité avant d'inventer une logique.
- Journaliser tout travail significatif dans `CHANGELOG.md` (convention `YYYY-MM-DD HH:MM:SS`, familles Décision/Assets/Build/Réseau/Rendu/Serveur/Doc).
