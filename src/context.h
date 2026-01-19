#ifndef CONTEXT_H
#define CONTEXT_H

class Camera;
class GameState;

namespace con {
    extern Camera* camera;
    extern GameState* gameState;

    void init(Camera* camera, GameState* gameState);
};

#endif //CONTEXT_H
