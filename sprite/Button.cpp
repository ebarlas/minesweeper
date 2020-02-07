#include "Button.h"

namespace minesweeper {
    Button::Button(ImageRepo &imageRepo, const Options &options, const Layout &layout) :
            Sprite(imageRepo, layout.getFace()),
            state(GameState::INIT),
            revealed(0),
            options(options) {

    }

    void Button::setListeners(const std::vector<GameStateListenerWPtr> &v) {
        listeners = v;
    }

    void Button::handleClick(SDL_MouseButtonEvent evt) {
        state = GameState::INIT;
        revealed = 0;
        notifyListeners();
    }

    void Button::onReveal(bool mine, bool adjacentMines) {
        if (mine) {
            state = GameState::LOST;
            notifyListeners();
        } else {
            if (state == GameState::INIT) {
                state = GameState::PLAYING;
                notifyListeners();
            }
            revealed++;
            if (revealed == options.getBlanks()) {
                state = GameState::WON;
                notifyListeners();
            }
        }
    }

    void Button::render() {
        getFaceImage()->render(&boundingBox);
    }

    TexturePtr Button::getFaceImage() {
        switch (state) {
            case GameState::INIT:
            case GameState::PLAYING:
                return imageRepo.get("face_playing");
            case GameState::WON:
                return imageRepo.get("face_win");
            default:
                return imageRepo.get("face_lose");
        }
    }

    void Button::notifyListeners() {
        for (auto &listener : listeners)
            if (auto spt = listener.lock())
                spt->onStateChange(state);
    }
}
