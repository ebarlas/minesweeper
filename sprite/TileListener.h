#ifndef MINESWEEPER_TILELISTENER_H
#define MINESWEEPER_TILELISTENER_H

#include <memory>

namespace minesweeper {
    class TileListener {
    public:
        virtual void onReveal(bool mine, bool adjacentMines);
        virtual void onClear();
        virtual void onFlag(bool flagged);
    };

    using TileListenerPtr = std::shared_ptr<TileListener>;
    using TileListenerWPtr = std::weak_ptr<TileListener>;
};

#endif
