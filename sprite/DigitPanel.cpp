#include "DigitPanel.h"

namespace minesweeper {
    void DigitPanel::render() {
        imageRepo.get("digit_panel")->render(&boundingBox);

        int value = getDisplayValue();
        int onesDigit = value % 10;
        int tensDigit = (value / 10) % 10;
        int hundredsDigit = (value / 100) % 10;

        SDL_Rect rect = getDigitRect(0);
        imageRepo.get(DIGITS[hundredsDigit])->render(&rect);

        rect = getDigitRect(1);
        imageRepo.get(DIGITS[tensDigit])->render(&rect);

        rect = getDigitRect(2);
        imageRepo.get(DIGITS[onesDigit])->render(&rect);
    }

    DigitPanel::DigitPanel(ImageRepo &imageRepo, SDL_Rect boundingBox) : Sprite(imageRepo, boundingBox) {

    }
}
