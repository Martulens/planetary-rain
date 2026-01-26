#pragma once

#ifndef VARIABLES_H
#define VARIABLES_H

#include "game.h"
#include "ui.h"

namespace var {
    extern std::shared_ptr<Game> mGame;
    extern std::shared_ptr<UI> mUi;

    void init();
    std::shared_ptr<Game> getGame();
    std::shared_ptr<UI> getUI();
}

#endif // VARIABLES_H