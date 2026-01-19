#include "context.h"

#include <algorithm>

namespace con{
    Camera* camera = nullptr;
    GameState* gameState = nullptr;

    void init(Camera* cam, GameState* gs){
      camera = cam;
      gameState = gs;
    }
}
