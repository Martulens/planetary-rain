#ifndef CONTEXT_H
#define CONTEXT_H

class Camera;
class GameState;
class Scene;

namespace con {
    extern Camera* camera;
    extern GameState* gameState;
    extern Scene* scene;

    void init(Camera* camera, GameState* gameState);
};

#endif //CONTEXT_H
