#include "Layout.h"

namespace minesweeper {
    Layout::Layout(Mode::Enum mode) : mode(mode) {

    }

    int Layout::getTileSide() {
        return TILE_SIDE;
    }

    [[nodiscard]] SDL_Rect Layout::getDigitPanel(int left, int top) const {
        return {left, top, DIGIT_PANEL_WIDTH, DIGIT_PANEL_HEIGHT};
    }

    [[nodiscard]] SDL_Rect Layout::getFlagsDigitPanel() const {
        return getDigitPanel(DIGIT_PANEL_OFFSET[mode], FLAGS_TOP);
    }

    [[nodiscard]] SDL_Rect Layout::getTimerDigitPanel() const {
        return getDigitPanel(WINDOW_WIDTH[mode] - DIGIT_PANEL_OFFSET[mode] - DIGIT_PANEL_WIDTH, TIMER_TOP);
    }

    [[nodiscard]] SDL_Rect Layout::getDigit(int left, int top, int position) const {
        return {
                left + DIGIT_PANEL_HORZ_MARGIN * (position + 1) + DIGIT_WIDTH * position,
                top + DIGIT_PANEL_VERT_MARGIN,
                DIGIT_WIDTH,
                DIGIT_HEIGHT};
    }

    [[nodiscard]] SDL_Rect Layout::getFlagsDigit(int position) const {
        return getDigit(DIGIT_PANEL_OFFSET[mode], FLAGS_TOP, position);
    }

    [[nodiscard]] SDL_Rect Layout::getTimerDigit(int position) const {
        return getDigit(WINDOW_WIDTH[mode] - DIGIT_PANEL_OFFSET[mode] - DIGIT_PANEL_WIDTH, TIMER_TOP, position);
    }

    [[nodiscard]] SDL_Rect Layout::getFace() const {
        return {WINDOW_WIDTH[mode] / 2 - FACE_WIDTH / 2, FACE_TOP, FACE_WIDTH, FACE_HEIGHT};
    }

    [[nodiscard]] SDL_Rect Layout::getTile(int gridX, int gridY, int row, int col) const {
        return {gridX + col * TILE_SIDE, gridY + row * TILE_SIDE, TILE_SIDE, TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect Layout::getGrid() const {
        return {
                GRID_LEFT,
                GRID_TOP,
                COLUMNS[mode] * TILE_SIDE,
                ROWS[mode] * TILE_SIDE};
    }

    [[nodiscard]] SDL_Rect Layout::getBackground() const {
        return {0, 0, WINDOW_WIDTH[mode], WINDOW_HEIGHT[mode]};
    }

    [[nodiscard]] SDL_Rect Layout::getWindow() const {
        return {WINDOW_LEFT, WINDOW_TOP, WINDOW_WIDTH[mode], WINDOW_HEIGHT[mode]};
    }
}

