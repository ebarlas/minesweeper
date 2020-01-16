#pragma once

#include "Sprite.h"

#include "SDL2/framework/ITexture.h"

#include "SDL.h"
#include <vector>

namespace minesweeper {

class IDigits {
public:
    virtual ~IDigits() = default;

public:
    virtual sdl::ITextureWPtr digit(unsigned value) const = 0;
};

using IDigitsPtr = std::shared_ptr<IDigits>;


class Digits : public IDigits {
public:
    Digits(const ContextPtr& context) {
        auto repo = context->imageRepo;
        _digits = { repo->get("minesweeper_digit_zero"),
                    repo->get("minesweeper_digit_one"),
                    repo->get("minesweeper_digit_two"),
                    repo->get("minesweeper_digit_three"),
                    repo->get("minesweeper_digit_four"),
                    repo->get("minesweeper_digit_five"),
                    repo->get("minesweeper_digit_six"),
                    repo->get("minesweeper_digit_seven"),
                    repo->get("minesweeper_digit_eight"),
                    repo->get("minesweeper_digit_nine")
                  };
    }

public:
    sdl::ITextureWPtr digit(unsigned value) const override {
        if( value > 9) return sdl::ITextureWPtr();
        return _digits[value];
    }

private:
    std::vector<sdl::ITextureWPtr> _digits;
};

class DigitPanel : public Sprite
{
public:
    DigitPanel(const ContextPtr &context, const IDigitsPtr& digits, const SDL_Rect &boundingBox)
        : Sprite(context, boundingBox), _digits(digits)
    {
    }

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
        return _digits->digit(digit).lock();
    }

private:
    const IDigitsPtr _digits;
};

}
