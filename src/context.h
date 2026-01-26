#ifndef CONTEXT_H
#define CONTEXT_H
#include <memory>
#include <vector>

class Camera;
class Light;
class Scene;
class Object;
struct GameState;

class Context {
private:

public:
    Context() = default;
    Context(std::shared_ptr<Camera> camera,
            std::shared_ptr<GameState> gameState,
            std::vector<std::shared_ptr<Light>> lights,
            std::vector<std::shared_ptr<Object>> objects);

    // === GETTERS

};
#endif //CONTEXT_H
