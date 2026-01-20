#include "context.h"

#include <algorithm>
#include <scene.h>

namespace con{
    Camera* camera = nullptr;
    GameState* gameState = nullptr;

    void init(Camera* cam, GameState* gs){
        camera = cam;
        gameState = gs;
        scene = new Scene();
        scene->init();
    }
}
