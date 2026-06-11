#pragma once

#include "network/T4CNpcDialog.h"
#include "network/T4CPlayerInventory.h"

#include "game/T4CPuppetDraw.h"
#include "game/T4CUnitSpriteNames.h"

#include <cstdint>
#include <string>
#include <vector>

class TFCPacket;

struct T4CCharacterSlot {
    std::string name;
    std::uint16_t race{0};
    std::uint16_t level{0};
};

struct T4CEnterWorldSpawn {
    unsigned int x{2880};
    unsigned int y{1083};
    unsigned short world{0};
    bool valid{false};
};

/** Stats combat / feuille perso (opcode 43 Character::PacketStatus, aligne Packet.cpp RQ_GetStatus). */
struct T4CPlayerStatus {
    unsigned int hp{0};
    unsigned int maxHp{0};
    unsigned short mana{0};
    unsigned short maxMana{0};
    std::uint16_t level{0};
    unsigned short ac{0};
    unsigned short str{0};
    unsigned short end{0};
    unsigned short agi{0};
    unsigned short wis{0};
    unsigned short intel{0};
    unsigned short weight{0};
    unsigned short maxWeight{0};
    /** Or transporte (opcode 53 RQ_GoldChange ; maj live). */
    std::uint32_t gold{0};
    /** XP totale (opcode 43 / 44). */
    std::uint64_t xp{0};
    /** Seuil XP prochain niveau (opcode 37, Exp2Go Windows). */
    std::uint64_t xpToNextLevel{0};
    bool valid{false};
};

/** Stats affichees apres opcode 25/31 (Character::packet_stats, aligne TFCSocket.cpp). */
struct T4CCharacterRolledStats {
    unsigned char agi{0};
    unsigned char end{0};
    unsigned char intel{0};
    unsigned char luck{0};
    unsigned char str{0};
    unsigned char wil{0};
    unsigned char wis{0};
    unsigned int maxHp{0};
    unsigned int hp{0};
    unsigned short maxMana{0};
    unsigned short mana{0};
    bool valid{false};
};

/** Perso actif (selection + sync serveur PacketPopup type 10004). */
struct T4CActivePlayer {
    std::string name;
    std::uint16_t race{0};
    /** Niveau connu (opcode 26 ; maj opcode 43 / 37). */
    std::uint16_t level{0};
    /** ID apparence serveur (10001–10004 mâle, 15001–15004 femelle, 10011/10012 puppet). */
    std::uint16_t appearance{0};
    /** Classe 0–3 (Warrio/Wizard/Cleric/Thief) — questionnaire creation ou race 10001–10004. */
    std::uint8_t classIndex{0};
    /** Equipement puppet (opcode 68) — __OBJGROUP_* du serveur. */
    std::uint16_t puppetBody{0};
    std::uint16_t puppetFeet{0};
    std::uint16_t puppetGloves{0};
    std::uint16_t puppetHelm{0};
    std::uint16_t puppetLegs{0};
    std::uint16_t puppetWeaponR{0};
    std::uint16_t puppetWeaponL{0};
    std::uint16_t puppetCape{0};
    bool puppetKnown{false};
    unsigned int serverX{0};
    unsigned int serverY{0};
    std::int32_t unitId{0};
    bool female{false};
    bool valid{false};
};

/** Nom NPCList / spr_pal pour le sprite PC (ex. « Thief », « Warrio »). */
const char *T4CPlayerSpriteNpcName(const T4CActivePlayer &player);

/** Direction visuelle T4C (1–9) depuis delta tuile monde (TileSet::MoveToPosition). */
int T4CDirectionFromWorldDelta(int dx, int dy);

/** Direction visuelle T4C (1–9) depuis un opcode deplacement 1–8. */
int T4CDirectionFromMoveOpcode(std::uint16_t moveOpcode);

T4CUnitSpriteView T4CUnitSpriteViewFromDirection(int direction);

/** Sprite PC complet pour direction + frame animation. */
std::string T4CPlayerUnitSpriteName(const T4CActivePlayer &player, int direction, int frameIndex);

/** Frame 3D statique debout face camera (Warrio000-a). */
std::string T4CPlayerStandingSpriteName(const T4CActivePlayer &player);

/**
 * Démarre la pile CCommCenter (UDP éphémère local, envoi vers host + port UDP).
 * hostField : IPv4 ou nom d'hôte (suffixe :port optionnel ignoré au profit de portField).
 * portField : chaîne décimale 1–65535 ; vide ou invalide → port par défaut T4C 11677.
 */
bool T4CLoginSessionStart(const std::string &hostField, const std::string &portField, const std::string &login,
                          const std::string &password);

/** Appel interne depuis T4CCommBridge (une TFCPacket par datagramme applicatif). */
void T4CLoginSessionHandlePacket(TFCPacket *msg);

void T4CLoginSessionShutdown();

/** Déconnexion serveur : SafePlug+ExitGame immédiat, fermeture UDP en arrière-plan. */
void T4CLoginSessionDisconnectInGame();

/** True si une session UDP est active ou un logout SafePlug est encore en cours. */
bool T4CLoginSessionIsNetworkActive();

/** True pendant le decompte SafePlug (~15 s) apres Esc depuis le monde. */
bool T4CLoginSessionIsLogoutInProgress();

/** Secondes restantes avant qu'un nouveau Connect soit accepte (0 = OK). */
int T4CLoginSessionGetReconnectCooldownSeconds();

/** A appeler depuis la boucle login (rejoint le thread logout termine). */
void T4CLoginSessionPollBackgroundTasks();

/** Ligne HUD monde (etat opcode 46, rappel deplacement local). */
std::string T4CLoginSessionGetWorldHudLine();

/** Annule retries « already logged » et ferme la session UDP (nouveau Connect). */
/** True pendant le pipeline auth (Register…liste persos), avant entree en monde. */
bool T4CLoginSessionIsAuthInProgress();

void T4CLoginSessionAbortLogin();

/** Dernier message d'erreur auth (opcode 14 refuse), vide si aucun. */
std::string T4CLoginSessionGetLastAuthError();

/** Après retour login depuis le monde : permet une nouvelle entrée en carte. */
void T4CLoginSessionResetAfterReturnToLogin();

/** True une fois quand la liste persos (26) a ete recue. */
bool T4CLoginSessionConsumeCharacterListReady();

/** Copie la liste persos parsee (thread-safe). */
void T4CLoginSessionCopyCharacterList(std::vector<T4CCharacterSlot> *outSlots, int *outMaxPerAccount);

/** Envoie RQ_CreatePlayer (25). stats[5] = reponses questionnaire (V1: defauts fixes). sex: 0=homme, 1=femme. */
bool T4CLoginSessionRequestCreatePlayer(const std::string &name, unsigned char sex,
                                        const unsigned char stats[5]);

bool T4CLoginSessionIsWaitingCreatePlayer();

bool T4CLoginSessionHasCreatePlayerError();

std::string T4CLoginSessionGetCreatePlayerErrorMessage();

void T4CLoginSessionClearCreatePlayerError();

/** Reinitialise l'etat client creation/reroll (ecran creation ouvert). */
void T4CLoginSessionPrepareForCreateScreen();

/** Envoie RQ_QueryNameExistence (90) — validation nom cote serveur (aligne Windows). */
bool T4CLoginSessionRequestQueryNameExistence(const std::string &name);

/** True une fois quand le joueur a valide l'ecran reroll (liste 26 demandee). */
bool T4CLoginSessionConsumeCreatePlayerSuccess();

/** True pendant l'ecran reroll apres opcode 25 OK (perso cree, stats modifiables). */
bool T4CLoginSessionIsInCreateRerollPhase();

/** True une fois quand de nouvelles stats arrivent (opcode 25 ou 31). */
bool T4CLoginSessionConsumeRolledStatsUpdate(T4CCharacterRolledStats *outStats);

/** Envoie RQ_Reroll (31) — relance les des cote serveur. */
bool T4CLoginSessionRequestCreateReroll();

/** Valide les stats courantes : refresh liste 26 puis entree en monde (aligne Windows). */
bool T4CLoginSessionConfirmCreateReroll();

/** Annule la creation : opcode 15 sur le perso provisoire, retour selection. */
bool T4CLoginSessionCancelCreateReroll();

/** Envoie RQ_DeletePlayer (15) puis refresh RQ_GetPersonnalPClist (26). */
bool T4CLoginSessionRequestDeletePlayer(const std::string &playerName);

bool T4CLoginSessionIsWaitingDeletePlayer();

bool T4CLoginSessionHasDeletePlayerError();

std::string T4CLoginSessionGetDeletePlayerErrorMessage();

void T4CLoginSessionClearDeletePlayerError();

/** Envoie RQ_PutPlayerInGame (13) avec le nom choisi (une seule requete en vol). */
bool T4CLoginSessionRequestPutPlayerInGame(const std::string &playerName);

bool T4CLoginSessionIsWaitingPutPlayerInGame();

bool T4CLoginSessionHasPutPlayerInGameError();

/** Message d'erreur serveur (opcode 13 court) ; vide si aucune erreur. */
std::string T4CLoginSessionGetPutPlayerInGameErrorMessage();

void T4CLoginSessionClearPutPlayerInGameError();

/** True une fois quand opcode 13 OK + 46/60 envoyes — spawn serveur dans *outSpawn. */
bool T4CLoginSessionConsumeEnterWorldReady(T4CEnterWorldSpawn *outSpawn);

/** Copie le perso choisi (race, nom) + dernier PacketPopup 10004 si reçu. */
void T4CLoginSessionGetActivePlayer(T4CActivePlayer *outPlayer);

/** Dernier opcode 43 / maj partielle 33·67 (thread-safe). */
void T4CLoginSessionGetPlayerStatus(T4CPlayerStatus *outStatus);

/** True une fois quand HP/mana/stats viennent d'etre mis a jour (opcode 43, 33 ou 67). */
bool T4CLoginSessionConsumePlayerStatusUpdate(T4CPlayerStatus *outStatus);

/** Demande explicite RQ_GetStatus (43) — refresh cote serveur. */
bool T4CLoginSessionRequestPlayerStatus();

/** Inventaire / skills / sorts / coffre banque (opcodes 18, 39, 62, 106). */
void T4CLoginSessionGetBackpack(T4CPlayerBackpack *out);
void T4CLoginSessionGetSkillBook(T4CPlayerSkillBook *out);
void T4CLoginSessionGetSpellBook(T4CPlayerSpellBook *out);
void T4CLoginSessionGetBankChest(T4CPlayerBankChest *out);
void T4CLoginSessionGetEquipment(T4CPlayerEquipment *out);

/** True une fois apres maj inventaire, skills, sorts ou coffre. */
bool T4CLoginSessionConsumeInventoryUpdate();

bool T4CLoginSessionRequestSkillList();
bool T4CLoginSessionRequestSpellList();
bool T4CLoginSessionRequestViewBackpack();
bool T4CLoginSessionRequestViewEquipped();

/**
 * Demande les noms manquants (opcode 59) pour le sac ou le coffre banque.
 * A appeler depuis la boucle monde (limite interne par frame).
 */
void T4CLoginSessionPollItemNameRequests(T4CItemSearchPlace place, int maxPerTick);

/** Coffre banque visible (opcode 109/110). */
bool T4CLoginSessionIsBankChestUiVisible();

/** True une fois par nouveau PacketPopup 10004 (position / apparence serveur). */
bool T4CLoginSessionConsumePlayerPopupUpdate(T4CActivePlayer *outPlayer);

/** Position apres RQ_TeleportPlayer (57). */
struct T4CPlayerTeleport {
    unsigned int x{0};
    unsigned int y{0};
    unsigned short world{0};
    /** true si le paquet contenait bien un monde (0 = surface est un monde valide). */
    bool hasWorld{false};
};

bool T4CLoginSessionConsumePlayerTeleport(T4CPlayerTeleport *outTeleport);

/** True une fois opcode 46 OK (in_game cote client). */
bool T4CLoginSessionIsWorldSessionReady();

/** Demande les unites peripheriques (RQ_GetNearItems 60 → reponse serveur opcode 16). */
bool T4CLoginSessionRequestNearItems();

/** Envoie un déplacement (opcodes 1–8, aligné TFCSocket.cpp). Retourne false si pas en jeu. */
bool T4CLoginSessionSendMove(std::uint16_t moveOpcode);

/** Attaque une unite (opcode 24 RQ_Attack — coords tuile cible + unitId). */
bool T4CLoginSessionSendAttack(unsigned int targetX, unsigned int targetY, std::int32_t targetUnitId);

/** Parler a un PNJ (opcode 30 RQ_DirectedTalk). */
bool T4CLoginSessionSendDirectedTalk(unsigned int targetX, unsigned int targetY, std::int32_t targetUnitId,
                                     int direction);

/** Option dialogue / lien [Buy] (opcode 50 RQ_DirectedTalkNoFeed). */
bool T4CLoginSessionSendDirectedTalkLink(const std::string &linkText);

/** Rompre conversation (opcode 36). */
bool T4CLoginSessionSendBreakConversation();

void T4CLoginSessionGetTalkState(T4CTalkState *outTalk);

/** Derniere replique PNJ (opcode 27) — une fois par message. */
bool T4CLoginSessionConsumeNpcSpeech(T4CNpcSpeech *outSpeech);

/** Liste boutique/train ouverte (opcodes 40/41/55/56). */
bool T4CLoginSessionConsumeShopList(T4CShopList *outShop);
void T4CLoginSessionGetShopList(T4CShopList *outShop);
void T4CLoginSessionCloseShop();

/** Utilise un objet / porte / portail (opcode 23 RQ_UseObject). */
bool T4CLoginSessionSendUseObject(unsigned int targetX, unsigned int targetY, std::int32_t unitId);

/** Ramasse un objet au sol (opcode 11 RQ_GetObject). */
bool T4CLoginSessionSendGetObject(unsigned int targetX, unsigned int targetY, std::int32_t unitId);

/** Achat PNJ (opcode 41 sortant, V3_BuyDlg) — items = {objectId, qty}. */
bool T4CLoginSessionSendBuyItems(const std::vector<std::pair<std::int32_t, std::uint16_t>> &items);

/** Vente PNJ (opcode 56 sortant) — items = {objectId, qty}. */
bool T4CLoginSessionSendSellItems(const std::vector<std::pair<std::int32_t, std::uint32_t>> &items);

/** Entrainement compétences (opcode 40 sortant) — skills = {skillId, points}. */
bool T4CLoginSessionSendTrainSkills(const std::vector<std::pair<std::uint16_t, std::uint16_t>> &skills);

/** Apprentissage compétences/sorts (opcode 55 sortant) — liste de skillId. */
bool T4CLoginSessionSendTeachSkills(const std::vector<std::uint16_t> &skillIds);

/** Equipe un objet du sac (opcode 21 RQ_EquipObject). */
bool T4CLoginSessionSendEquipObject(std::int32_t itemId);

/** Desequipe un slot (opcode 22 RQ_UnequipObject — code position Windows TFCPlayer.h). */
bool T4CLoginSessionSendUnequipObject(T4CEquipSlot slot);

/** Jette un objet au sol a la position joueur (opcode 12 RQ_DepositObject). */
bool T4CLoginSessionSendDropObject(std::int32_t itemId, std::uint32_t qty);

/** Detruit un objet du sac (opcode 85 RQ_JunkItems). */
bool T4CLoginSessionSendJunkItems(std::int32_t itemId, std::uint32_t qty);

/** Utilise un objet du sac sur soi (manger/boire/lire — RQ_UseObject coords 0,0 + itemId). */
bool T4CLoginSessionSendUseBagItem(std::int32_t itemId);

/** Lance un sort (opcode 32 RQ_CastSpell). targetId=0 + coords 0,0 = sur soi. */
bool T4CLoginSessionSendCastSpell(std::uint16_t spellId, unsigned int targetX, unsigned int targetY,
                                  std::int32_t targetId);

/** Utilise une competence sans cible (opcode 42 RQ_UseSkill — mediter, etc.). */
bool T4CLoginSessionSendUseSkill(std::uint16_t skillId);

/** Bascule le mode combat (opcode 198 RQ_AttackMode — Ctrl+C Windows). */
bool T4CLoginSessionSendAttackMode(bool enable);

/** Etat du mode combat confirme par le serveur (reponse 198). */
bool T4CLoginSessionGetAttackMode();

/** Chat local (opcode 27 sortant — parole proximite). */
bool T4CLoginSessionSendChatLocal(const std::string &text);

/** Cri de zone (opcode 28 RQ_Shout). */
bool T4CLoginSessionSendShout(const std::string &text);

/** Message prive (opcode 29 RQ_Page) vers un joueur nomme. */
bool T4CLoginSessionSendPage(const std::string &targetName, const std::string &text);

/** Message chat recu (27 joueur/PNJ, 29 page, 63/102 systeme) ou echo local. */
enum class T4CChatKind : std::uint8_t {
    Local,
    Shout,
    Page,
    System,
};

struct T4CChatMessage {
    T4CChatKind kind{T4CChatKind::Local};
    std::string speaker;
    std::string text;
    std::uint32_t color{0xFFFFFFu};
    std::int32_t unitId{0};
};

/** Vide la file chat (thread-safe, depuis la boucle GameWorld). */
void T4CLoginSessionDrainChatMessages(std::vector<T4CChatMessage> *outMessages);

/** Heure du monde T4C (opcode 45 RQ_GetTime — g_TimeStructure Windows). */
struct T4CGameTime {
    unsigned char second{0};
    unsigned char minute{0};
    unsigned char hour{12};
    unsigned char day{0};
    unsigned char week{0};
    unsigned char month{0};
    unsigned short year{0};
    bool valid{false};
};

void T4CLoginSessionGetGameTime(T4CGameTime *outTime);

/** Demande l'heure serveur (opcode 45 sortant). */
bool T4CLoginSessionRequestGameTime();

/** Ajoute un message systeme local (UI). */
void T4CLoginSessionPushSystemMessage(const std::string &text);

/** Position confirmee par le serveur (opcodes 1–8, joueur local). */
bool T4CLoginSessionConsumeLocalPlayerMove(unsigned int *outX, unsigned int *outY,
                                           std::uint16_t *outMoveOpcode);

/** Met a jour les coords affichees du perso actif (apres mouvement local). */
void T4CLoginSessionUpdateActivePlayerPosition(unsigned int x, unsigned int y);

/** Evenement reseau → rendu : spawn / deplacement / maj stats / despawn d'une unite distante. */
enum class T4CRemoteUnitEventKind : std::uint8_t {
    Spawn,
    Move,
    Update,
    Remove,
    /** Opcode 10001 : anim attaque ('A') vers (x,y) = cible. */
    Attack,
};

struct T4CRemoteUnitEvent {
    T4CRemoteUnitEventKind kind{T4CRemoteUnitEventKind::Spawn};
    std::int32_t unitId{0};
    std::uint16_t appearance{0};
    unsigned int x{0};
    unsigned int y{0};
    char hpPercent{0};
    /** Opcode deplacement 1–8 si connu (TFCMoveID) — direction visuelle. */
    std::uint16_t moveOpcode{0};
    /** Cible attaque (opcode 10001) — direction StAtt vers (targetX, targetY). */
    unsigned int targetX{0};
    unsigned int targetY{0};
    bool hasAttackTarget{false};
};

/** Marqueur objet sol (portes/coffres/objets) extrait des opcodes 16/1. */
struct T4CGroundObjectMarker {
    std::int32_t unitId{0};
    std::uint16_t appearance{0};
    unsigned int x{0};
    unsigned int y{0};
};

/** Vide la file d'evenements unites distantes (thread-safe, depuis la boucle GameWorld). */
void T4CLoginSessionDrainRemoteUnitEvents(std::vector<T4CRemoteUnitEvent> *outEvents);

/** Nom NPCList / VSF pour une apparence serveur (20006 → Warrio, etc.). */
const char *T4CSpriteNameFromAppearance(std::uint16_t appearance);

/** Bases uniques de la table creatures (gen_creature_appearance_table.py) — preload auto. */
void T4CCollectCreaturePreloadBases(std::vector<std::string> *out);

/** Sprite a dessiner pour une unite distante (puppet equipement ou apparence brute). */
const char *T4CRemoteUnitSpriteName(std::int32_t unitId, std::uint16_t appearance);

/** True si l'apparence est un puppet equipe (10011/10012). */
bool T4CIsPuppetAppearance(std::uint16_t appearance);

/** Equipement puppet d'une unite distante (opcode 68). */
bool T4CLoginSessionGetRemotePuppetDress(std::int32_t unitId, T4CPuppetDress *outDress);

/** True si cette unite ne doit pas etre dessinee comme NPC distant (joueur local / PC proche). */
bool T4CLoginSessionShouldSkipRemoteUnit(std::uint16_t appearance, std::int32_t unitId, unsigned int x,
                                         unsigned int y);

/** Reinitialise la file (logout, teleport, retour login). */
void T4CLoginSessionClearRemoteUnits();
void T4CLoginSessionCopyGroundObjectMarkers(std::vector<T4CGroundObjectMarker> *outMarkers);

/** Texte de degats flottant (opcode 124/125). */
struct T4CFloatingDamage {
    std::int32_t unitId{0};
    std::string text;
    std::uint32_t color{0};
};

/** True une fois quand le joueur meurt (opcode 47). */
bool T4CLoginSessionConsumePlayerDeath();

/** Attaque locale (le joueur frappe) : true une seule fois par swing, coords du defenseur. */
bool T4CLoginSessionConsumeLocalAttack(unsigned *targetX, unsigned *targetY);

/** Demande le nom reel d'une unite (RQ_GetUnitName 35) — dedupliquee/cachee. */
void T4CLoginSessionRequestUnitName(std::int32_t unitId, unsigned int x, unsigned int y);

/** Nom reel + couleur serveur d'une unite si deja recu (« Dark Fang », « Mithrand »…). */
bool T4CLoginSessionGetUnitName(std::int32_t unitId, std::string *outName, std::uint32_t *outColor);

bool T4CLoginSessionIsPlayerDead();

/** Etat mort NMS (opcodes 200/201) — timer + bouton ressusciter. */
struct T4CDeathState {
    bool active{false};
    bool canResurrect{false};
    std::uint16_t timeCurrent{0};
    std::uint16_t timeTotal{0};
};

void T4CLoginSessionGetDeathState(T4CDeathState *outState);
void T4CLoginSessionClearPlayerDeath();

/** RQ_NM_DeathResurect (202) — teleport temple via NMResurect. */
bool T4CLoginSessionSendDeathResurrect();

/** Derniers textes de degats/soins a afficher (ephemere, thread-safe). */
void T4CLoginSessionDrainFloatingDamage(std::vector<T4CFloatingDamage> *outDamage);

/** @deprecated Utiliser ConsumeCharacterListReady + flux selection. */
bool T4CLoginSessionConsumeNetworkSuccessDialog();
