#pragma once

#ifndef DRAW_H
#define DRAW_H

#include <memory>
#include <scene.h>

#include "glm/vec3.hpp"
#include <utility>
#include <vector>

// Forward declarations
class Object;
class Camera;
class Light;
struct GameState;

class Draw{
private:
public:
    Draw() = default;

    // === Main drawing functions
    void drawWindow(std::shared_ptr<Scene> scene,
                    std::shared_ptr<Camera> camera,
                    std::shared_ptr<GameState> gameState);

    void drawScene( std::shared_ptr<Scene> scene,
                    std::shared_ptr<Camera> camera,
                    std::shared_ptr<GameState> gameState);

    // === Template drawing functions
    void setupTextures(std::shared_ptr<Object> obj, std::shared_ptr<Scene> scene);
    
    void drawTemplate(  std::shared_ptr<Object> obj,
                        std::shared_ptr<Camera> camera,
                        std::shared_ptr<Scene> scene);

    void setupUniforms( std::shared_ptr<Object> object,
                        std::shared_ptr<Camera> cam,
                        std::shared_ptr<Scene> scene);

};
#endif // DRAW_H