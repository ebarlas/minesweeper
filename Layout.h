#pragma once

#include "Mode.h"

#include "SDL.h"

namespace minesweeper {

class Layout
{
private:
    static constexpr int ROWS[]{9, 16, 16};
    static constexpr int COLUMNS[]{9, 16, 30};

    static constexpr int WINDOW_WIDTH[]{210, 350, 630};
    static constexpr int WINDOW_HEIGHT[]{276, 416, 416};
    static constexpr int WINDOW_LEFT = 100;
    static constexpr int WINDOW_TOP = 100;

    static constexpr int TILE_SIDE = 20;

    static constexpr int DIGIT_PANEL_WIDTH = 65;
    static constexpr int DIGIT_PANEL_HEIGHT = 37;
    static constexpr int DIGIT_WIDTH = 19;
    static constexpr int DIGIT_HEIGHT = 33;
    static constexpr int DIGIT_PANEL_HORZ_MARGIN = (DIGIT_PANEL_WIDTH - (3 * DIGIT_WIDTH)) / 4;
    static constexpr int DIGIT_PANEL_VERT_MARGIN = (DIGIT_PANEL_HEIGHT - DIGIT_HEIGHT) / 2;

    static constexpr int DIGIT_PANEL_OFFSET[]{16, 20, 20};

    static constexpr int TIMER_TOP = 21;

    static constexpr int FLAGS_TOP = 21;

    static constexpr int FACE_WIDTH = 42;
    static constexpr int FACE_HEIGHT = 42;
    static constexpr int FACE_TOP = 19;

    static constexpr int GRID_LEFT = 15;
    static constexpr int GRID_TOP = 81;

    const Mode::Enum mode;
public:
    explicit Layout(const Mode::Enum mode) : mode(mode) {

    }

    static int getTileSide() {
        return TILE_SIDE;
    }

    [[nodiscard]] SDL_Rect getDigitPanel(int left, int top) const {
        return {left, top, DIGIT_PANEL_WIDTH, DIGIT_PANEL_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getFlagsDigitPanel() const {
        return getDigitPanel(DIGIT_PANEL_OFFSET[mode], FLAGS_TOP);
    }

    [[nodiscard]] SDL_Rect getTimerDigitPanel() const {
        return getDigitPanel(WINDOW_WIDTH[mode] - DIGIT_PANEL_OFFSET[mode] - DIGIT_PANEL_WIDTH, TIMER_TOP);
    }

    [[nodiscard]] SDL_Rect getDigit(int left, int top, int position) const {
        return {
                left + DIGIT_PANEL_HORZ_MARGIN * (position + 1) + DIGIT_WIDTH * position,
                top + DIGIT_PANEL_VERT_MARGIN,
                DIGIT_WIDTH,
                DIGIT_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getFlagsDigit(int position) const {
        return getDigit(DIGIT_PANEL_OFFSET[mode], FLAGS_TOP, position);
    }

    [[nodiscard]] SDL_Rect getTimerDigit(int position) const {
        return getDigit(WINDOW_WIDTH[mode] - DIGIT_PANEL_OFFSET[mode] - DIGIT_PANEL_WIDTH, TIMER_TOP, position);
    }

    [[nodiscard]] SDL_Rect getFace() const {
        return {WINDOW_WIDTH[mode] / 2 - FACE_WIDTH / 2, FACE_TOP, FACE_WIDTH, FACE_HEIGHT};
    }

    [[nodiscard]] SDL_Rect getTile(int gridX, int gridY, int row, int col) const {
        return {gridX + col * TILE_SIDE, gridY + row * TILE_SIDE, TILE_SIDE, TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect getGrid() const {
        return {
                GRID_LEFT,
                GRID_TOP,
                COLUMNS[mode] * TILE_SIDE,
                ROWS[mode] * TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect getBackground() const {
        return {0, 0, WINDOW_WIDTH[mode], WINDOW_HEIGHT[mode]};
    }

    [[nodiscard]] SDL_Rect getWindow() const {
        return {WINDOW_LEFT, WINDOW_TOP, WINDOW_WIDTH[mode], WINDOW_HEIGHT[mode]};
    }
};

} // namespace minesweeper
