#ifndef CONFIG_H
#define CONFIG_H

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 900;
const int refreshTimeMs = 16;

const glm::vec3 skyColorConst = glm::vec3(0.5f, 0.4f, 0.8f);

#define PLAYER_MOUSE_SENSITIVITY 0.15f
#define MOVE_SPEED 0.2f

#define CAMERA_TRANSITION_DURATION 2.0f
#define SKYBOX_CUBE_TEXTURE_FILE_PREFIX "textures/skybox/"

#define CHUNK_SIZE 300
#define CUTOFF_ANGLE 20.0f

#define FOG_DENSITY 0.003
#define FOG_GRADIENT 1.5

#define LIGHTS_MAX 20

#endif // CONFIG_H