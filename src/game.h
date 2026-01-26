#ifndef GAME_H
#define GAME_H
#include "callbacks.h"
#include "draw.h"
#include "scene.h"

// main game logic
class Game{
private:
    std::shared_ptr<Draw> mDraw;

    std::shared_ptr<Camera> mCamera = nullptr;
    std::shared_ptr<GameState> mGameState = nullptr;
    std::shared_ptr<Scene> mScene = nullptr;

public:
    Game();

    void restartGame();
    void cleanUpObjects();

    // === GETTERS
    std::shared_ptr<Camera> getCamera() const{ return mCamera;}
    std::shared_ptr<GameState> getGameState() const { return mGameState;}
    std::shared_ptr<Scene> getScene() const { return mScene;}

    std::shared_ptr<Draw> getDraw() const { return mDraw;}
};
#endif
