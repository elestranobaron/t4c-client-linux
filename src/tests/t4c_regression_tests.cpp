/**
 * Tests de non-regression — baseline immuable + invariants moteur.
 * Executer apres chaque build : ./scripts/run_regression_tests.sh
 *
 * Ajouter un test ici pour chaque comportement valide qu'on ne doit jamais casser.
 */
#include "game/T4CMovementBaseline.h"
#include "game/T4CUnitSpriteNames.h"
#include "tests/T4CTestHarness.h"

#include <cmath>

namespace {

/** Table TileSet::MoveToPosition (MovX, MovY) -> direction. */
struct MoveToPosCase {
    int movX;
    int movY;
    int direction;
};

constexpr MoveToPosCase kMoveToPositionTable[] = {
    {0, 1, 2},   {0, -1, 8},  {1, 0, 6},  {-1, 0, 4},
    {1, 1, 3},   {1, -1, 9},  {-1, 1, 1}, {-1, -1, 7},
};

}  // namespace

T4C_TEST(direction_from_world_delta_matches_windows) {
    for (const MoveToPosCase &c : kMoveToPositionTable) {
        T4C_ASSERT_EQ(T4CDirectionFromWorldDelta(c.movX, c.movY), c.direction);
    }
}

T4C_TEST(direction_from_sent_opcode_matches_delta) {
    for (std::uint16_t op = 1; op <= 8; ++op) {
        int dx = 0;
        int dy = 0;
        T4C_ASSERT(T4CMoveDeltaFromOpcode(op, dx, dy));
        T4C_ASSERT_EQ(T4CDirectionFromMoveOpcode(op), T4CDirectionFromWorldDelta(dx, dy));
    }
}

T4C_TEST(server_packet_opcode_1_is_not_move_direction) {
    T4C_ASSERT_EQ(kT4CEventObjectMovedOpcode, 1);

    // Regression Phase 4c : lire opcode paquet entrant (=1) pour sud donne dos (8).
    const int wrongIfUsingPacketOpcode = T4CDirectionFromMoveOpcode(kT4CEventObjectMovedOpcode);
    T4C_ASSERT_EQ(wrongIfUsingPacketOpcode, 8);

    const int correctSouth = T4CDirectionFromServerMoveConfirm(0, 1, kT4CEventObjectMovedOpcode);
    T4C_ASSERT_EQ(correctSouth, 2);
    T4C_ASSERT(wrongIfUsingPacketOpcode != correctSouth);
}

T4C_TEST(all_eight_directions_via_server_confirm) {
    for (const MoveToPosCase &c : kMoveToPositionTable) {
        T4C_ASSERT_EQ(T4CDirectionFromServerMoveConfirm(c.movX, c.movY, kT4CEventObjectMovedOpcode),
                      c.direction);
    }
}

T4C_TEST(move_opcode_arrow_mapping) {
    T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(false, false, true, false), 1);   // haut
    T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(false, false, false, true), 5);  // bas
    T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(true, false, false, false), 7);  // gauche
    T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(false, true, false, false), 3);  // droite
    T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(true, false, true, false), 8);   // haut-gauche
    T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(false, true, true, false), 2);   // haut-droite
}

T4C_TEST(move_opcode_delta_roundtrip) {
    for (std::uint16_t op = 1; op <= 8; ++op) {
        int dx = 0;
        int dy = 0;
        T4C_ASSERT(T4CMoveDeltaFromOpcode(op, dx, dy));
        const bool left = dx < 0;
        const bool right = dx > 0;
        const bool up = dy < 0;
        const bool down = dy > 0;
        T4C_ASSERT_EQ(T4CMoveOpcodeFromArrows(left, right, up, down), op);
    }
}

T4C_TEST(invalid_move_opcode) {
    int dx = 0;
    int dy = 0;
    T4C_ASSERT(!T4CMoveDeltaFromOpcode(0, dx, dy));
    T4C_ASSERT(!T4CMoveDeltaFromOpcode(9, dx, dy));
}

T4C_TEST(player_scroll_eight_steps_per_tile) {
    T4CPlayerViewState st{};
    st.serverX = 10;
    st.serverY = 20;
    st.displayX = 9.f;
    st.displayY = 20.f;

    for (int i = 0; i < kT4CPlayerScrollSteps - 1; ++i) {
        T4C_ASSERT(T4CPlayerScrollStep(&st));
    }
    T4C_ASSERT(!T4CPlayerScrollStep(&st));
    T4C_ASSERT_NEAR(st.displayX, 10.f);
    T4C_ASSERT_NEAR(st.displayY, 20.f);
    T4C_ASSERT(!T4CPlayerViewIsScrolling(st));
}

T4C_TEST(server_move_does_not_snap_display) {
    T4CPlayerViewState st{};
    st.serverX = 5;
    st.serverY = 5;
    st.displayX = 5.f;
    st.displayY = 5.f;

    T4CPlayerViewApplyServerMove(&st, 6, 5);
    T4C_ASSERT_EQ(st.serverX, 6u);
    T4C_ASSERT_NEAR(st.displayX, 5.f);
    T4C_ASSERT(T4CPlayerViewIsScrolling(st));
}

T4C_TEST(teleport_snaps_display) {
    T4CPlayerViewState st{};
    st.serverX = 1;
    st.serverY = 2;
    st.displayX = 0.f;
    st.displayY = 2.f;

    T4CPlayerViewApplyServerMove(&st, 100, 200);
    T4CPlayerViewSnapDisplay(&st);
    T4C_ASSERT_EQ(st.serverX, 100u);
    T4C_ASSERT_NEAR(st.displayX, 100.f);
    T4C_ASSERT_NEAR(st.displayY, 200.f);
    T4C_ASSERT(!T4CPlayerViewIsScrolling(st));
}

T4C_TEST(walk_anim_reset_policy) {
    T4C_ASSERT(T4CWalkAnimShouldResetFrame(false, 2, 2));
    T4C_ASSERT(T4CWalkAnimShouldResetFrame(true, 2, 6));
    T4C_ASSERT(!T4CWalkAnimShouldResetFrame(true, 2, 2));
}

T4C_TEST(walk_anim_nine_frame_cycle) {
    T4C_ASSERT_EQ(kT4CWalkAnimFrames, 9);
    int frame = 0;
    for (int i = 0; i < 8; ++i) {
        frame = T4CWalkAnimNextFrame(frame);
    }
    T4C_ASSERT_EQ(frame, 8);
    T4C_ASSERT_EQ(T4CWalkAnimNextFrame(frame), 0);
}

T4C_TEST(walk_anim_npc_six_frame_cycle) {
    T4C_ASSERT_EQ(kT4CNpcWalkAnimFrames, 6);
    int frame = 0;
    for (int i = 0; i < 5; ++i) {
        frame = T4CWalkAnimNextNpcFrame(frame);
    }
    T4C_ASSERT_EQ(frame, 5);
    T4C_ASSERT_EQ(T4CWalkAnimNextNpcFrame(frame), 0);
    T4CUnitSpriteView view{};
    view.angleDegrees = 180;
    T4C_ASSERT(T4CUnitSpriteFrameName("Rat", view, 5, kT4CNpcWalkAnimFrames - 1) == "Rat180-f");
}

T4C_TEST(walk_anim_puppet_thirteen_frame_cycle) {
    T4C_ASSERT_EQ(kT4CPuppetWalkAnimFrames, 13);
    int frame = 0;
    for (int i = 0; i < 12; ++i) {
        frame = T4CWalkAnimNextPuppetFrame(frame);
    }
    T4C_ASSERT_EQ(frame, 12);
    T4C_ASSERT_EQ(T4CWalkAnimNextPuppetFrame(frame), 0);
}

T4C_TEST(idle_sprite_frame_names) {
    T4CUnitSpriteView view{};
    view.angleDegrees = 180;
    T4C_ASSERT(T4CUnitSpriteIdleFrameName("Warrio", view, 0) == "WarrioStMov180-a");
    T4C_ASSERT(T4CUnitSpriteIdleFrameName("Warrio", view, 3) == "WarrioStMov180-d");
}

T4C_TEST(idle_anim_four_frame_cycle) {
    T4C_ASSERT_EQ(kT4CIdleAnimFrames, 4);
    int frame = 0;
    for (int i = 0; i < 3; ++i) {
        frame = T4CWalkAnimNextIdleFrame(frame);
    }
    T4C_ASSERT_EQ(frame, 3);
    T4C_ASSERT_EQ(T4CWalkAnimNextIdleFrame(frame), 0);
}

T4C_TEST(walk_sprite_frame_names) {
    T4CUnitSpriteView view{};
    view.angleDegrees = 90;
    T4C_ASSERT(T4CUnitSpriteFrameName("Warrio", view, 0) == "Warrio090-a");
    T4C_ASSERT(T4CUnitSpriteFrameName("PupWhiteRobe", view, 2) == "PupWhiteRobe090-c");
}

T4C_TEST(attack_sprite_frame_names) {
    T4CUnitSpriteView view{};
    view.angleDegrees = 45;
    /* Nom Icon3D reel : « <Base>A<angle>-<lettre> » (GoblinA045-a existe dans V2DataI.did,
     * GoblinStAtt045-a n'existe pas). */
    T4C_ASSERT(T4CUnitAttackSpriteFrameName("Goblin", view, 0) == "GoblinA045-a");
    T4C_ASSERT(T4CUnitAttackSpriteFrameName("Goblin", view, 3) == "GoblinA045-d");
    T4C_ASSERT(T4CUnitAttackSpriteFrameName("Goblin", view, 8) == "GoblinA045-i");
}

T4C_TEST(corpse_sprite_frame_names) {
    T4C_ASSERT(T4CUnitCorpseSpriteFrameName("Goblin", 0) == "GoblinC-a");
    T4C_ASSERT(T4CUnitCorpseSpriteFrameName("Goblin", 2) == "GoblinC-c");
}

T4C_TEST(creature_corpse_deposit_object_type) {
    T4C_ASSERT(T4CIsCreatureCorpseAppearance(16005));
    T4C_ASSERT(T4CIsCreatureCorpseAppearance(26005));
    T4C_ASSERT(!T4CIsCreatureCorpseAppearance(20001));
    T4C_ASSERT(T4CNormalizeDepositObjectType(16005) == 15005);
    T4C_ASSERT(T4CNormalizeDepositObjectType(10002) == 20039);
}

T4C_TEST(sprite_palette_key) {
    T4C_ASSERT(T4CUnitSpritePaletteKey("PupWhiteRobe090-a") == "PupWhiteRobe");
    T4C_ASSERT(T4CUnitSpritePaletteKey("PupBodyClothSet1090-c") == "PupBodyClothSet1");
}

T4C_TEST(diagonal_scroll) {
    T4CPlayerViewState st{};
    st.serverX = 11;
    st.serverY = 21;
    st.displayX = 10.f;
    st.displayY = 20.f;

    for (int i = 0; i < kT4CPlayerScrollSteps; ++i) {
        T4CPlayerScrollStep(&st);
    }
    T4C_ASSERT_NEAR(st.displayX, 11.f);
    T4C_ASSERT_NEAR(st.displayY, 21.f);
}

int main() {
    std::fprintf(stderr, "t4c_regression_tests\n");

    T4C_RUN(direction_from_world_delta_matches_windows);
    T4C_RUN(direction_from_sent_opcode_matches_delta);
    T4C_RUN(server_packet_opcode_1_is_not_move_direction);
    T4C_RUN(all_eight_directions_via_server_confirm);
    T4C_RUN(move_opcode_arrow_mapping);
    T4C_RUN(move_opcode_delta_roundtrip);
    T4C_RUN(invalid_move_opcode);
    T4C_RUN(player_scroll_eight_steps_per_tile);
    T4C_RUN(server_move_does_not_snap_display);
    T4C_RUN(teleport_snaps_display);
    T4C_RUN(walk_anim_reset_policy);
    T4C_RUN(walk_anim_nine_frame_cycle);
    T4C_RUN(walk_anim_npc_six_frame_cycle);
    T4C_RUN(walk_anim_puppet_thirteen_frame_cycle);
    T4C_RUN(idle_sprite_frame_names);
    T4C_RUN(idle_anim_four_frame_cycle);
    T4C_RUN(walk_sprite_frame_names);
    T4C_RUN(attack_sprite_frame_names);
    T4C_RUN(sprite_palette_key);
    T4C_RUN(diagonal_scroll);

    return t4c_test_main_finish();
}
