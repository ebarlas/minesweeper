#ifndef MINESWEEPER_LAYOUT_H
#define MINESWEEPER_LAYOUT_H

#import "SDL.h"
#import "Mode.h"

namespace minesweeper {
    class Layout {
    public:
        explicit Layout(const Mode::Enum mode);
        static int getTileSide();
        [[nodiscard]] SDL_Rect getDigitPanel(int left, int top) const;
        [[nodiscard]] SDL_Rect getFlagsDigitPanel() const;
        [[nodiscard]] SDL_Rect getTimerDigitPanel() const;
        [[nodiscard]] SDL_Rect getDigit(int left, int top, int position) const;
        [[nodiscard]] SDL_Rect getFlagsDigit(int position) const;
        [[nodiscard]] SDL_Rect getTimerDigit(int position) const;
        [[nodiscard]] SDL_Rect getFace() const;
        [[nodiscard]] SDL_Rect getTile(int gridX, int gridY, int row, int col) const;
        [[nodiscard]] SDL_Rect getGrid() const;
        [[nodiscard]] SDL_Rect getBackground() const;
        [[nodiscard]] SDL_Rect getWindow() const;
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
    };
}

#endif
