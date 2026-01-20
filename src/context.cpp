#include "context.h"
#include "scene.h"  // FIXED: Use quotes for local headers

namespace con {
    Camera* camera = nullptr;
    GameState* gameState = nullptr;
    Scene* scene = nullptr;

    void init(Camera* cam, GameState* gs) {
        camera = cam;
        gameState = gs;
        scene = new Scene();
        scene->init();
    }
}