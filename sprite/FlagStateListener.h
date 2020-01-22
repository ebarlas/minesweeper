#ifndef MINESWEEPER_FLAGSTATELISTENER_H
#define MINESWEEPER_FLAGSTATELISTENER_H

#include <memory>

namespace minesweeper {
    class FlagStateListener {
    public:
        virtual void onFlagStateChange(bool exhausted) = 0;
        virtual ~FlagStateListener() = default;
    };

    using FlagStateListenerPtr = std::shared_ptr<FlagStateListener>;
    using FlagStateListenerWPtr = std::weak_ptr<FlagStateListener>;
};

#endif
