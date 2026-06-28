#pragma once

#include "assets/map/T4CV2SpriteAtlas.h"
#include "gui/T4CUiFont.h"
#include "network/T4CLoginSession.h"
#include "network/T4CPlayerInventory.h"

#include <SDL3/SDL.h>

/**
 * Fenêtre perso modale, calquée sur les V3_*Dlg Windows (shell 608×455, g_OptionDlgW/H).
 * Trois onglets reproduits : Personnage (stats + compétences), Inventaire (sac + paperdoll
 * équipement), Sorts. Coordonnées internes relatives à l'origine, comme les dialogs Windows.
 */
class T4CCharacterWindow {
   public:
    enum class Tab { Stats = 0, Inventory = 1, Spells = 2 };

    static constexpr int kW = 608;
    static constexpr int kH = 455;

    /** Action gameplay (boutons sac, competences, sorts). */
    enum class Action {
        None,
        EquipSelected,
        UseSelected,
        DropSelected,
        JunkSelected,
        UseSkill,
        CastSpell,
    };

    enum class HitKind {
        Outside,
        Inside,
        Close,
        Tab,
        BagCell,
        EquipSlot,
        ActionButton,
        Skill,
        Spell,
    };

    /** Objet en cours de drag : masquer la source dans le rendu inventaire. */
    struct InvDragView {
        bool active{false};
        bool fromEquip{false};
        int bagIndex{-1};
        T4CEquipSlot equipSlot{T4CEquipSlot::Body};
    };

    struct HitTestResult {
        HitKind kind{HitKind::Outside};
        Tab tab{Tab::Stats};
        Action action{Action::None};
        int index{-1};
        T4CEquipSlot slot{T4CEquipSlot::Body};
        bool hasItem{false};
    };

    struct ClickResult {
        bool consumed{false};
        bool close{false};
        bool tabChanged{false};
        Tab tab{Tab::Stats};
        Action action{Action::None};
        int index{-1};
        T4CEquipSlot slot{T4CEquipSlot::Body};
    };

    void preloadSprites(T4CV2SpriteAtlas *atlas) const;

    /** Origine (coin haut-gauche) de la fenêtre centrée à l'écran. */
    static SDL_FPoint origin(int screenW, int screenH);

    void render(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font, int screenW,
                int screenH, Tab tab, const T4CActivePlayer &player, const T4CPlayerStatus &status,
                const T4CPlayerEquipment &equipment, const T4CPlayerBackpack &backpack,
                const T4CPlayerSkillBook &skills, const T4CPlayerSpellBook &spells, int selectedBag,
                InvDragView drag) const;

    /** Hit-test souris (coords logiques rendu). */
    HitTestResult hitTest(int screenW, int screenH, float mx, float my, Tab current,
                          const T4CPlayerEquipment &equipment, const T4CPlayerBackpack &backpack,
                          const T4CPlayerSkillBook &skills, const T4CPlayerSpellBook &spells,
                          int selectedBag, InvDragView drag) const;

   private:
    void renderStats(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font, float ox,
                     float oy, const T4CActivePlayer &player, const T4CPlayerStatus &status,
                     const T4CPlayerSkillBook &skills) const;
    void renderInventory(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                         float ox, float oy, const T4CActivePlayer &player, const T4CPlayerStatus &status,
                         const T4CPlayerEquipment &equipment, const T4CPlayerBackpack &backpack,
                         int selectedBag, InvDragView drag) const;
    void renderSpells(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font, float ox,
                      float oy, const T4CPlayerSpellBook &spells) const;
};
