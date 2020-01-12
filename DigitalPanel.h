#pragma once

#include "Sprite.h"

#include "SDL2/framework/ITexture.h"

#include "SDL.h"

namespace minesweeper {

class DigitPanel : public Sprite
{
public:
    DigitPanel(const ContextPtr &context, const SDL_Rect &boundingBox)
        : Sprite(context, boundingBox)
    {}

    virtual SDL_Rect getDigitRect(int position) = 0;
    virtual int getDisplayValue() = 0;

    void render() override
    {
        getTexture()->render(_boundingBox);

        auto value = getDisplayValue();
        auto onesDigit = value % 10;
        auto tensDigit = (value / 10) % 10;
        auto hundredsDigit = (value / 100) % 10;

        SDL_Rect rect = getDigitRect(0);
        getDigit(hundredsDigit)->render(rect);

        rect = getDigitRect(1);
        getDigit(tensDigit)->render(rect);

        rect = getDigitRect(2);
        getDigit(onesDigit)->render(rect);
    }

private:
    sdl::ITexturePtr getTexture()
    {
        return _context->imageRepo->get("minesweeper_digit_panel");
    }

    sdl::ITexturePtr getDigit(unsigned digit)
    {
        switch (digit) {
        case 1:
            return _context->imageRepo->get("minesweeper_digit_one");
        case 2:
            return _context->imageRepo->get("minesweeper_digit_two");
        case 3:
            return _context->imageRepo->get("minesweeper_digit_three");
        case 4:
            return _context->imageRepo->get("minesweeper_digit_four");
        case 5:
            return _context->imageRepo->get("minesweeper_digit_five");
        case 6:
            return _context->imageRepo->get("minesweeper_digit_six");
        case 7:
            return _context->imageRepo->get("minesweeper_digit_seven");
        case 8:
            return _context->imageRepo->get("minesweeper_digit_eight");
        case 9:
            return _context->imageRepo->get("minesweeper_digit_nine");
        default:
            return _context->imageRepo->get("minesweeper_digit_zero");
        }
    }
};

}
