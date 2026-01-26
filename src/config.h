#ifndef CONFIG_H
#define CONFIG_H

constexpr int WINDOW_WIDTH = 1500;
constexpr int WINDOW_HEIGHT = 1024;
constexpr int refreshTimeMs = 16;

constexpr glm::vec3 skyColorConst = glm::vec3(0.5f, 0.4f, 0.8f);

constexpr  float PLAYER_MOUSE_SENSITIVITY = 0.15f;
constexpr float MOVE_SPEED = 0.2f;

constexpr std::string SKYBOX_CUBE_TEXTURE_FILE_PREFIX = "textures/skybox/";

constexpr float CUTOFF_ANGLE = 20.0f;

constexpr float FOG_DENSITY = .003f;
constexpr float FOG_GRADIENT = 1.5f;

constexpr int LIGHTS_MAX = 20;

#endif // CONFIG_H