#include "game/T4CMovementBaseline.h"

#include <algorithm>
#include <cmath>

bool T4CMoveDeltaFromOpcode(const std::uint16_t opcode, int &dx, int &dy) {
    switch (opcode) {
        case 1:
            dx = 0;
            dy = -1;
            return true;
        case 2:
            dx = 1;
            dy = -1;
            return true;
        case 3:
            dx = 1;
            dy = 0;
            return true;
        case 4:
            dx = 1;
            dy = 1;
            return true;
        case 5:
            dx = 0;
            dy = 1;
            return true;
        case 6:
            dx = -1;
            dy = 1;
            return true;
        case 7:
            dx = -1;
            dy = 0;
            return true;
        case 8:
            dx = -1;
            dy = -1;
            return true;
        default:
            return false;
    }
}

std::uint16_t T4CMoveOpcodeFromArrows(const bool left, const bool right, const bool up, const bool down) {
    if (up && left) {
        return 8;
    }
    if (up && right) {
        return 2;
    }
    if (down && left) {
        return 6;
    }
    if (down && right) {
        return 4;
    }
    if (up) {
        return 1;
    }
    if (down) {
        return 5;
    }
    if (left) {
        return 7;
    }
    if (right) {
        return 3;
    }
    return 0;
}

int T4CDirectionFromWorldDelta(const int dx, const int dy) {
    const int sx = (dx > 0) - (dx < 0);
    const int sy = (dy > 0) - (dy < 0);

    if (sx == 0 && sy > 0) {
        return 2;
    }
    if (sx == 0 && sy < 0) {
        return 8;
    }
    if (sx < 0 && sy == 0) {
        return 4;
    }
    if (sx > 0 && sy == 0) {
        return 6;
    }
    if (sx < 0 && sy > 0) {
        return 1;
    }
    if (sx > 0 && sy > 0) {
        return 3;
    }
    if (sx < 0 && sy < 0) {
        return 7;
    }
    if (sx > 0 && sy < 0) {
        return 9;
    }
    return 2;
}

int T4CDirectionFromMoveOpcode(const std::uint16_t moveOpcode) {
    int dx = 0;
    int dy = 0;
    if (!T4CMoveDeltaFromOpcode(moveOpcode, dx, dy)) {
        return 1;
    }
    return T4CDirectionFromWorldDelta(dx, dy);
}

int T4CDirectionFromServerMoveConfirm(const int dx, const int dy, const std::uint16_t packetOpcode) {
    (void)packetOpcode;
    return T4CDirectionFromWorldDelta(dx, dy);
}

bool T4CPlayerViewIsScrolling(const T4CPlayerViewState &state) {
    const float dx = static_cast<float>(state.serverX) - state.displayX;
    const float dy = static_cast<float>(state.serverY) - state.displayY;
    return dx * dx + dy * dy > 0.0001f;
}

bool T4CPlayerScrollStep(T4CPlayerViewState *state, const int scrollSteps) {
    if (!state || scrollSteps <= 0) {
        return false;
    }
    const float tx = static_cast<float>(state->serverX);
    const float ty = static_cast<float>(state->serverY);
    const float dx = tx - state->displayX;
    const float dy = ty - state->displayY;
    if (dx * dx + dy * dy < 0.0001f) {
        state->displayX = tx;
        state->displayY = ty;
        return false;
    }
    // Windows : g_MOVX/g_MOVY par axe, 8 frames par tuile — pas un lerp vectoriel.
    const float step = 1.f / static_cast<float>(scrollSteps);
    if (dx > 0.f) {
        state->displayX += std::min(dx, step);
    } else if (dx < 0.f) {
        state->displayX += std::max(dx, -step);
    }
    if (dy > 0.f) {
        state->displayY += std::min(dy, step);
    } else if (dy < 0.f) {
        state->displayY += std::max(dy, -step);
    }
    return T4CPlayerViewIsScrolling(*state);
}

void T4CPlayerViewSnapDisplay(T4CPlayerViewState *state) {
    if (!state) {
        return;
    }
    state->displayX = static_cast<float>(state->serverX);
    state->displayY = static_cast<float>(state->serverY);
}

void T4CPlayerViewApplyServerMove(T4CPlayerViewState *state, const unsigned int newX, const unsigned int newY) {
    if (!state) {
        return;
    }
    state->serverX = newX;
    state->serverY = newY;
}

bool T4CWalkAnimShouldResetFrame(const bool wasMoving, const int oldDirection, const int newDirection) {
    return !wasMoving || oldDirection != newDirection;
}

int T4CWalkAnimNextFrame(const int frame) {
    if (kT4CWalkAnimFrames <= 0) {
        return 0;
    }
    return (frame + 1) % kT4CWalkAnimFrames;
}
