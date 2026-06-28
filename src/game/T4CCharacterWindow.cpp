#include "game/T4CCharacterWindow.h"

#include "game/T4CGroundObjectSprites.h"
#include "game/T4CInvItemSprites.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

namespace {

// Sprites de fond V3 (résolus par identifiant string depuis l'atlas V2, comme Windows).
constexpr const char *kStatBack = "V3_StatBack8n";
constexpr const char *kInvBack = "V3_InvBack";
constexpr const char *kSpellBack = "V3_SpellBack";

const SDL_Color kText{226, 220, 198, 255};
const SDL_Color kValue{255, 255, 255, 255};
const SDL_Color kGold{255, 196, 64, 255};
const SDL_Color kDim{150, 144, 122, 255};
const SDL_Color kTabActive{255, 236, 170, 255};

void fillRect(SDL_Renderer *r, float x, float y, float w, float h, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_FRect rect{x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

void frameRect(SDL_Renderer *r, float x, float y, float w, float h, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_FRect rect{x, y, w, h};
    SDL_RenderRect(r, &rect);
}

void text(const T4CUiFont *f, SDL_Renderer *r, const char *s, float x, float y, SDL_Color c) {
    if (f && f->isReady() && s && s[0] != '\0') {
        f->drawText(r, s, x, y, c);
    }
}

// Texte aligné à droite sur xRight.
void textRight(const T4CUiFont *f, SDL_Renderer *r, const char *s, float xRight, float y, SDL_Color c) {
    if (!f || !f->isReady() || !s || s[0] == '\0') {
        return;
    }
    int w = 0;
    int h = 0;
    f->measureText(s, &w, &h);
    f->drawText(r, s, xRight - static_cast<float>(w), y, c);
}

// Dessine l'icône d'un objet (sprite monde de l'objgroup) centrée dans une cellule.
void drawItemIcon(SDL_Renderer *r, const T4CV2SpriteAtlas &atlas, std::uint16_t appearance, float cx,
                  float cy) {
    if (const char *sprite = T4CInvItemSpriteName(appearance); sprite != nullptr) {
        if (atlas.TryDrawSpriteByName(r, sprite, cx, cy)) {
            return;
        }
    }
    fillRect(r, cx + 4.f, cy + 4.f, 18.f, 18.f, {70, 64, 50, 255});
}

void statLine(const T4CUiFont *f, SDL_Renderer *r, const char *label, unsigned int value, float x, float y) {
    text(f, r, label, x, y, kText);
    char buf[24];
    std::snprintf(buf, sizeof(buf), "%u", value);
    textRight(f, r, buf, x + 132.f, y, kValue);
}

void statPair(const T4CUiFont *f, SDL_Renderer *r, const char *label, unsigned int cur, unsigned int max,
              float x, float y) {
    text(f, r, label, x, y, kText);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%u/%u", cur, max);
    textRight(f, r, buf, x + 132.f, y, kValue);
}

/* ---- Layout partage render/clic (rects V3_InvDlg / V3_StatDlg / V3_SpellDlg). ---- */

struct SlotDef {
    T4CEquipSlot slot;
    const char *label;
    float x;
    float y;
    float w;
    float h;
};

constexpr SlotDef kEquipSlots[] = {
    {T4CEquipSlot::Helm, "Tete", 137.f, 97.f, 55.f, 55.f},
    {T4CEquipSlot::Necklace, "Cou", 57.f, 116.f, 29.f, 29.f},
    {T4CEquipSlot::Bracelets, "Brac.", 97.f, 109.f, 29.f, 29.f},
    {T4CEquipSlot::Ring1, "Bag.1", 202.f, 109.f, 29.f, 29.f},
    {T4CEquipSlot::Ring2, "Bag.2", 241.f, 116.f, 29.f, 29.f},
    {T4CEquipSlot::Body, "Corps", 43.f, 152.f, 55.f, 82.f},
    {T4CEquipSlot::Cape, "Cape", 229.f, 152.f, 55.f, 82.f},
    {T4CEquipSlot::WeaponRight, "Main D.", 43.f, 243.f, 55.f, 82.f},
    {T4CEquipSlot::WeaponLeft, "Main G.", 229.f, 243.f, 55.f, 82.f},
    {T4CEquipSlot::Gloves, "Gants", 43.f, 334.f, 43.f, 43.f},
    {T4CEquipSlot::Belt, "Ceint.", 89.f, 334.f, 43.f, 43.f},
    {T4CEquipSlot::Legs, "Jambes", 136.f, 322.f, 55.f, 66.f},
    {T4CEquipSlot::Feet, "Pieds", 195.f, 334.f, 43.f, 43.f},
};

constexpr int kBagCols = 8;
constexpr float kBagCell = 26.f;
constexpr float kBagGap = 3.f;
constexpr float kBagX = 318.f;
constexpr float kBagY = 112.f;
constexpr int kBagMaxSlots = 40;

struct ActionBtnDef {
    T4CCharacterWindow::Action action;
    const char *label;
    float x;
    float w;
};

constexpr float kActionBtnY = 378.f;
constexpr float kActionBtnH = 20.f;
constexpr ActionBtnDef kBagActions[] = {
    {T4CCharacterWindow::Action::EquipSelected, "Equiper", 318.f, 62.f},
    {T4CCharacterWindow::Action::UseSelected, "Utiliser", 384.f, 62.f},
    {T4CCharacterWindow::Action::DropSelected, "Jeter", 450.f, 50.f},
    {T4CCharacterWindow::Action::JunkSelected, "Detruire", 504.f, 62.f},
};

constexpr float kSkillListX = 40.f;
constexpr float kSkillListY = 340.f;
constexpr float kSkillLineH = 15.f;
constexpr int kSkillMaxShown = 6;

constexpr float kSpellListY = 135.f;
constexpr float kSpellSlotW = 222.f;
constexpr float kSpellSlotH = 40.f;
constexpr float kSpellRowStep = 47.f;
constexpr int kSpellMaxShown = 10;

float spellSlotX(const int col) { return col == 0 ? 46.f : 314.f; }

}  // namespace

SDL_FPoint T4CCharacterWindow::origin(const int screenW, const int screenH) {
    return SDL_FPoint{static_cast<float>((screenW - kW) / 2), static_cast<float>((screenH - kH) / 2)};
}

void T4CCharacterWindow::preloadSprites(T4CV2SpriteAtlas *atlas) const {
    if (!atlas) {
        return;
    }
    const std::vector<std::string> names = {kStatBack, kInvBack, kSpellBack};
    atlas->PreloadBanksForNames(names);
}

void T4CCharacterWindow::render(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                                const int screenW, const int screenH, const Tab tab,
                                const T4CActivePlayer &player, const T4CPlayerStatus &status,
                                const T4CPlayerEquipment &equipment, const T4CPlayerBackpack &backpack,
                                const T4CPlayerSkillBook &skills, const T4CPlayerSpellBook &spells,
                                const int selectedBag, const InvDragView drag) const {
    if (!renderer) {
        return;
    }
    const SDL_FPoint o = origin(screenW, screenH);
    const float ox = o.x;
    const float oy = o.y;

    // Voile derrière la modale.
    fillRect(renderer, 0.f, 0.f, static_cast<float>(screenW), static_cast<float>(screenH), {0, 0, 0, 120});

    // Fond du shell (sprite V3 selon l'onglet, sinon panneau plein).
    const char *back = tab == Tab::Inventory ? kInvBack : (tab == Tab::Spells ? kSpellBack : kStatBack);
    if (!atlas.TryDrawSpriteByName(renderer, back, ox, oy)) {
        fillRect(renderer, ox, oy, static_cast<float>(kW), static_cast<float>(kH), {24, 22, 30, 245});
        frameRect(renderer, ox, oy, static_cast<float>(kW), static_cast<float>(kH), {96, 88, 66, 255});
    }

    // Bouton fermer (554,27)-(581,51).
    fillRect(renderer, ox + 554.f, oy + 27.f, 27.f, 24.f, {80, 36, 32, 255});
    frameRect(renderer, ox + 554.f, oy + 27.f, 27.f, 24.f, {150, 90, 80, 255});
    text(font, renderer, "X", ox + 562.f, oy + 31.f, kValue);

    // Barre d'onglets (y=63..83).
    struct TabDef {
        Tab tab;
        const char *label;
        float x;
        float w;
    };
    const TabDef tabs[] = {
        {Tab::Stats, "Personnage", 33.f, 76.f},
        {Tab::Inventory, "Inventaire", 112.f, 76.f},
        {Tab::Spells, "Sorts", 191.f, 60.f},
    };
    for (const TabDef &t : tabs) {
        const bool active = t.tab == tab;
        fillRect(renderer, ox + t.x, oy + 63.f, t.w, 20.f, active ? SDL_Color{60, 54, 40, 255}
                                                                  : SDL_Color{34, 30, 24, 220});
        frameRect(renderer, ox + t.x, oy + 63.f, t.w, 20.f, {90, 82, 60, 255});
        text(font, renderer, t.label, ox + t.x + 6.f, oy + 65.f, active ? kTabActive : kDim);
    }

    // Titre + nom + niveau.
    if (player.valid && !player.name.empty()) {
        text(font, renderer, player.name.c_str(), ox + 127.f, oy + 90.f, kText);
    }
    const std::uint16_t level = status.valid && status.level != 0 ? status.level : player.level;
    if (level != 0) {
        char lv[24];
        std::snprintf(lv, sizeof(lv), "Niv. %u", static_cast<unsigned>(level));
        text(font, renderer, lv, ox + 68.f, oy + 90.f, kText);
    }

    switch (tab) {
        case Tab::Stats:
            renderStats(renderer, atlas, font, ox, oy, player, status, skills);
            break;
        case Tab::Inventory:
            renderInventory(renderer, atlas, font, ox, oy, player, status, equipment, backpack, selectedBag,
                            drag);
            break;
        case Tab::Spells:
            renderSpells(renderer, atlas, font, ox, oy, spells);
            break;
    }
}

void T4CCharacterWindow::renderStats(SDL_Renderer *renderer, const T4CV2SpriteAtlas & /*atlas*/,
                                     const T4CUiFont *font, const float ox, const float oy,
                                     const T4CActivePlayer & /*player*/, const T4CPlayerStatus &status,
                                     const T4CPlayerSkillBook &skills) const {
    // Colonne gauche : attributs (V3_StatDlg m_statL).
    const float lx = ox + 69.f;
    statLine(font, renderer, "Force", status.str, lx, oy + 184.f);
    statLine(font, renderer, "Endurance", status.end, lx, oy + 210.f);
    statLine(font, renderer, "Agilite", status.agi, lx, oy + 236.f);
    statLine(font, renderer, "Intelligence", status.intel, lx, oy + 262.f);
    statLine(font, renderer, "Sagesse", status.wis, lx, oy + 288.f);

    // Colonne droite : dérivés (m_statR).
    const float rx = ox + 320.f;
    statLine(font, renderer, "Armure (CA)", status.ac, rx, oy + 184.f);
    statPair(font, renderer, "Vie", status.hp, status.maxHp, rx, oy + 210.f);
    statPair(font, renderer, "Mana", status.mana, status.maxMana, rx, oy + 236.f);
    statPair(font, renderer, "Poids", status.weight, status.maxWeight, rx, oy + 262.f);

    // XP (V3_StatDlg [306]/[307]).
    {
        char buf[48];
        std::snprintf(buf, sizeof(buf), "XP: %llu", static_cast<unsigned long long>(status.xp));
        text(font, renderer, buf, rx, oy + 288.f, kText);
        if (status.xpToNextLevel > status.xp) {
            std::snprintf(buf, sizeof(buf), "Reste: %llu",
                          static_cast<unsigned long long>(status.xpToNextLevel - status.xp));
            text(font, renderer, buf, rx, oy + 308.f, kDim);
        }
    }

    // Liste de compétences (V3_StatDlg skill list, SkillIconsV3 + force/vraie force).
    text(font, renderer, "Competences (clic = utiliser)", ox + 36.f, oy + 322.f, kTabActive);
    float y = oy + 340.f;
    int shown = 0;
    for (const T4CPlayerSkill &sk : skills.skills) {
        if (shown >= 6 || y > oy + 430.f) {
            break;
        }
        char line[160];
        std::snprintf(line, sizeof(line), "%s", sk.name.empty() ? "?" : sk.name.c_str());
        text(font, renderer, line, ox + 40.f, y, kText);
        char pts[24];
        std::snprintf(pts, sizeof(pts), "%u", static_cast<unsigned>(sk.points));
        textRight(font, renderer, pts, ox + 560.f, y, kValue);
        y += 15.f;
        ++shown;
    }
    if (skills.skills.empty()) {
        text(font, renderer, "(aucune)", ox + 40.f, y, kDim);
    }
}

void T4CCharacterWindow::renderInventory(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas,
                                         const T4CUiFont *font, const float ox, const float oy,
                                         const T4CActivePlayer & /*player*/, const T4CPlayerStatus &status,
                                         const T4CPlayerEquipment &equipment,
                                         const T4CPlayerBackpack &backpack, const int selectedBag,
                                         const InvDragView drag) const {
    // Emplacements d'équipement (paperdoll) — rects V3_InvDlg. Drag depuis slot = masquer source.
    for (const SlotDef &s : kEquipSlots) {
        fillRect(renderer, ox + s.x, oy + s.y, s.w, s.h, {28, 26, 34, 220});
        frameRect(renderer, ox + s.x, oy + s.y, s.w, s.h, {80, 74, 56, 255});
        text(font, renderer, s.label, ox + s.x + 2.f, oy + s.y + 1.f, kDim);
        const bool hideEquip =
            drag.active && drag.fromEquip && drag.equipSlot == s.slot;
        if (!hideEquip) {
            for (const T4CEquippedItem &it : equipment.items) {
                if (it.slot == s.slot && it.appearance != 0) {
                    drawItemIcon(renderer, atlas, it.appearance, ox + s.x + s.w * 0.5f - 16.f,
                                 oy + s.y + s.h * 0.5f - 16.f);
                    break;
                }
            }
        }
    }

    // Sac : grille à droite (8 col × cellules 26+3). Le client Windows scrolle une bande étroite ;
    // on affiche une grille multi-lignes lisible à droite du paperdoll.
    text(font, renderer, "Sac", ox + kBagX, oy + 95.f, kTabActive);
    const float gx = ox + kBagX;
    const float gy = oy + kBagY;
    for (int i = 0; i < kBagMaxSlots; ++i) {
        const int col = i % kBagCols;
        const int row = i / kBagCols;
        const float cx = gx + static_cast<float>(col) * (kBagCell + kBagGap);
        const float cy = gy + static_cast<float>(row) * (kBagCell + kBagGap);
        const bool selected = i == selectedBag;
        fillRect(renderer, cx, cy, kBagCell, kBagCell,
                 selected ? SDL_Color{66, 58, 30, 240} : SDL_Color{30, 28, 22, 220});
        frameRect(renderer, cx, cy, kBagCell, kBagCell,
                  selected ? SDL_Color{255, 214, 90, 255} : SDL_Color{70, 64, 50, 255});
        const bool hideBag = drag.active && !drag.fromEquip && drag.bagIndex == i;
        if (backpack.valid && static_cast<size_t>(i) < backpack.items.size() && !hideBag) {
            const T4CBagItem &item = backpack.items[i];
            drawItemIcon(renderer, atlas, item.appearance, cx - 3.f, cy - 3.f);
            if (item.qty > 1) {
                char q[12];
                std::snprintf(q, sizeof(q), "%u", static_cast<unsigned>(item.qty));
                textRight(font, renderer, q, cx + kBagCell - 1.f, cy + kBagCell - 12.f, kValue);
            }
        }
    }

    // Objet tenu (drag sac ou equipe) : nom + boutons d'action (equip/use/drop/junk).
    const bool showBagActions =
        selectedBag >= 0 && backpack.valid && static_cast<size_t>(selectedBag) < backpack.items.size();
    const bool showEquipActions = drag.active && drag.fromEquip;
    if (showBagActions || showEquipActions) {
        char label[160] = "Objet";
        if (showBagActions) {
            const T4CBagItem &item = backpack.items[static_cast<size_t>(selectedBag)];
            std::snprintf(label, sizeof(label), "%s%s", item.name.empty() ? "Objet" : item.name.c_str(),
                          item.charges > 0 ? " *" : "");
        } else {
            for (const T4CEquippedItem &it : equipment.items) {
                if (it.slot == drag.equipSlot) {
                    std::snprintf(label, sizeof(label), "%s%s",
                                  it.name.empty() ? "Objet equipe" : it.name.c_str(),
                                  it.charges > 0 ? " *" : "");
                    break;
                }
            }
        }
        text(font, renderer, label, ox + kBagX, oy + kActionBtnY - 18.f, kGold);
        for (const ActionBtnDef &btn : kBagActions) {
            fillRect(renderer, ox + btn.x, oy + kActionBtnY, btn.w, kActionBtnH, {52, 46, 34, 240});
            frameRect(renderer, ox + btn.x, oy + kActionBtnY, btn.w, kActionBtnH, {120, 106, 72, 255});
            text(font, renderer, btn.label, ox + btn.x + 5.f, oy + kActionBtnY + 3.f, kText);
        }
    }

    // Bandeau bas : attributs + or + poids (V3_InvBackIcons).
    const float by = oy + 405.f;
    statLine(font, renderer, "FOR", status.str, ox + 73.f, by);
    statLine(font, renderer, "END", status.end, ox + 220.f, by);
    statLine(font, renderer, "AGI", status.agi, ox + 368.f, by);
    {
        char goldBuf[32];
        std::snprintf(goldBuf, sizeof(goldBuf), "Or: %u", status.gold);
        text(font, renderer, goldBuf, ox + 460.f, by, kGold);
    }
    text(font, renderer, "Poids", ox + kBagX, oy + 366.f, kText);
    {
        char w[32];
        std::snprintf(w, sizeof(w), "%u/%u", status.weight, status.maxWeight);
        textRight(font, renderer, w, ox + 560.f, oy + 366.f, kValue);
    }
}

void T4CCharacterWindow::renderSpells(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas,
                                      const T4CUiFont *font, const float ox, const float oy,
                                      const T4CPlayerSpellBook &spells) const {
    text(font, renderer, "Grimoire (clic = lancer)", ox + 36.f, oy + 110.f, kTabActive);
    if (spells.spells.empty()) {
        text(font, renderer, "(aucun sort)", ox + 46.f, oy + 140.f, kDim);
        return;
    }
    // 10 lignes par page, 2 colonnes (5+5), slot 222×40 (V3_SpellDlg).
    for (size_t i = 0; i < spells.spells.size() && i < 10; ++i) {
        const T4CPlayerSpell &sp = spells.spells[i];
        const int col = static_cast<int>(i) / 5;
        const int row = static_cast<int>(i) % 5;
        const float rx = ox + (col == 0 ? 46.f : 314.f);
        const float ry = oy + 135.f + static_cast<float>(row) * 47.f;
        fillRect(renderer, rx, ry, 222.f, 40.f, {30, 28, 38, 220});
        frameRect(renderer, rx, ry, 222.f, 40.f, {80, 74, 56, 255});
        // Icône 32×32 (slot+4). Sans table SpellIcons, cadre + repli.
        fillRect(renderer, rx + 4.f, ry + 4.f, 32.f, 32.f, {44, 40, 54, 255});
        text(font, renderer, sp.name.empty() ? "?" : sp.name.c_str(), rx + 40.f, ry + 4.f, kText);
        char info[48];
        std::snprintf(info, sizeof(info), "Mana %u  Niv %u", static_cast<unsigned>(sp.manaCost),
                      static_cast<unsigned>(sp.level));
        text(font, renderer, info, rx + 40.f, ry + 22.f, kDim);
    }
}

T4CCharacterWindow::HitTestResult T4CCharacterWindow::hitTest(
    const int screenW, const int screenH, const float mx, const float my, const Tab current,
    const T4CPlayerEquipment &equipment, const T4CPlayerBackpack &backpack,
    const T4CPlayerSkillBook &skills, const T4CPlayerSpellBook &spells, const int selectedBag,
    const InvDragView drag) const {
    HitTestResult res{};
    const SDL_FPoint o = origin(screenW, screenH);
    const float ox = o.x;
    const float oy = o.y;

    const auto inRect = [](float px, float py, float x, float y, float w, float h) {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    };

    if (!inRect(mx, my, ox, oy, static_cast<float>(kW), static_cast<float>(kH))) {
        res.kind = HitKind::Outside;
        return res;
    }
    res.kind = HitKind::Inside;

    if (inRect(mx, my, ox + 554.f, oy + 27.f, 27.f, 24.f)) {
        res.kind = HitKind::Close;
        return res;
    }

    struct TabHit {
        Tab tab;
        float x;
        float w;
    };
    const TabHit tabs[] = {
        {Tab::Stats, 33.f, 76.f},
        {Tab::Inventory, 112.f, 76.f},
        {Tab::Spells, 191.f, 60.f},
    };
    for (const TabHit &t : tabs) {
        if (inRect(mx, my, ox + t.x, oy + 63.f, t.w, 20.f)) {
            res.kind = HitKind::Tab;
            res.tab = t.tab;
            return res;
        }
    }

    if (current == Tab::Inventory) {
        const bool actionTarget =
            (selectedBag >= 0 && backpack.valid &&
             static_cast<size_t>(selectedBag) < backpack.items.size()) ||
            (drag.active && drag.fromEquip);
        if (actionTarget) {
            for (const ActionBtnDef &btn : kBagActions) {
                if (inRect(mx, my, ox + btn.x, oy + kActionBtnY, btn.w, kActionBtnH)) {
                    res.kind = HitKind::ActionButton;
                    res.action = btn.action;
                    res.index = selectedBag;
                    return res;
                }
            }
        }
        const float gx = ox + kBagX;
        const float gy = oy + kBagY;
        for (int i = 0; i < kBagMaxSlots; ++i) {
            const int col = i % kBagCols;
            const int row = i / kBagCols;
            const float cx = gx + static_cast<float>(col) * (kBagCell + kBagGap);
            const float cy = gy + static_cast<float>(row) * (kBagCell + kBagGap);
            if (inRect(mx, my, cx, cy, kBagCell, kBagCell)) {
                res.kind = HitKind::BagCell;
                res.index = i;
                res.hasItem =
                    backpack.valid && static_cast<size_t>(i) < backpack.items.size();
                return res;
            }
        }
        for (const SlotDef &s : kEquipSlots) {
            if (inRect(mx, my, ox + s.x, oy + s.y, s.w, s.h)) {
                res.kind = HitKind::EquipSlot;
                res.slot = s.slot;
                for (const T4CEquippedItem &it : equipment.items) {
                    if (it.slot == s.slot && it.appearance != 0) {
                        res.hasItem = true;
                        break;
                    }
                }
                return res;
            }
        }
    } else if (current == Tab::Stats) {
        const int shown = std::min<int>(kSkillMaxShown, static_cast<int>(skills.skills.size()));
        for (int i = 0; i < shown; ++i) {
            const float ly = oy + kSkillListY + static_cast<float>(i) * kSkillLineH;
            if (inRect(mx, my, ox + kSkillListX, ly, 524.f, kSkillLineH)) {
                res.kind = HitKind::Skill;
                res.index = i;
                return res;
            }
        }
    } else if (current == Tab::Spells) {
        const int shown = std::min<int>(kSpellMaxShown, static_cast<int>(spells.spells.size()));
        for (int i = 0; i < shown; ++i) {
            const int col = i / 5;
            const int row = i % 5;
            const float rx = ox + spellSlotX(col);
            const float ry = oy + kSpellListY + static_cast<float>(row) * kSpellRowStep;
            if (inRect(mx, my, rx, ry, kSpellSlotW, kSpellSlotH)) {
                res.kind = HitKind::Spell;
                res.index = i;
                return res;
            }
        }
    }
    return res;
}
