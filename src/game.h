#ifndef GAME_H
#define GAME_H
#include "callbacks.h"
#include "draw.h"

// main game logic
class Game{
private:
    Draw draw;

    std::shared_ptr<Camera> camera = nullptr;
    std::shared_ptr<GameState> gameState = nullptr;
    std::shared_ptr<Scene> scene = nullptr;

public:
    Game();

    void restartGame();
    void cleanUpObjects();

    // === GETTERS
    std::shared_ptr<Camera> getCamera() const{ return camera;}
    std::shared_ptr<GameState> getGameState() const { return gameState;}
    std::shared_ptr<Scene> getScene() const { return scene;}
}
#endif
