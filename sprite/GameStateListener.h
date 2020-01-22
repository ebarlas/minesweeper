#ifndef MINESWEEPER_GAMESTATELISTENER_H
#define MINESWEEPER_GAMESTATELISTENER_H

namespace minesweeper {
    enum class GameState {
        INIT,
        PLAYING,
        WON,
        LOST
    };

    class GameStateListener {
    public:
        virtual void onStateChange(GameState state) = 0;
        virtual ~GameStateListener() = default;
    };

    using GameStateListenerPtr = std::shared_ptr<GameStateListener>;
    using GameStateListenerWPtr = std::weak_ptr<GameStateListener>;
};

#endif
