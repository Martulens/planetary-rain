#include "context.h"
#include "scene.h"

#include <memory>


Context::Context(std::shared_ptr<Camera> camera,
            std::shared_ptr<GameState> gameState,
            std::vector<std::shared_ptr<Light>> lights,
            std::vector<std::shared_ptr<Object>> objects): camera(camera), gameState(gameState), lights(lights), objects(objects) {


}
