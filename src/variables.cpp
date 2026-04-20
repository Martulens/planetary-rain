#include "variables.h"

namespace var {
    std::shared_ptr<Game> mGame;
    std::shared_ptr<UI> mUi;

    void init() {
        mGame = std::make_shared<Game>();
        mUi = std::make_shared<UI>();
    }

    std::shared_ptr<Game> getGame() { return mGame; }
    std::shared_ptr<UI> getUI() { return mUi; }
}