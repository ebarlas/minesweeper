#ifndef MINESWEEPER_DIGITPANEL_H
#define MINESWEEPER_DIGITPANEL_H

#include "Sprite.h"
#include "../sdl/ImageRepo.h"

namespace minesweeper {
    class DigitPanel : public Sprite {
    public:
        void render() override;
    protected:
        DigitPanel(ImageRepo &imageRepo, SDL_Rect boundingBox);
        virtual SDL_Rect getDigitRect(int position) = 0;
        virtual int getDisplayValue() = 0;
    private:
        static constexpr const char *DIGITS[]{"digit_zero", "digit_one", "digit_two", "digit_three", "digit_four",
                                              "digit_five", "digit_six", "digit_seven", "digit_eight", "digit_nine"};
    };
};

#endif
