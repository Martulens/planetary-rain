#ifndef CONFIG_H
#define CONFIG_H

// === WINDOW SETUP
constexpr int WINDOW_WIDTH = 1500;
constexpr int WINDOW_HEIGHT = 1024;
constexpr int refreshTimeMs = 16;

// === PLAYER CONFIG
constexpr  float PLAYER_MOUSE_SENSITIVITY = 0.15f;
constexpr float MOVE_SPEED = 0.2f;

// === CAMERA SETUP
constexpr glm::vec3 CAM_POS = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 CAM_DIR = glm::vec3(100.0f, 0.0f, 20.0f);
constexpr glm::vec3 CAM_UP = glm::vec3(0.0f, 1.0f, 0.0f);

constexpr float CUTOFF_ANGLE = 20.0f;
constexpr float PITCH = 0.0f;
constexpr float YAW = 270.0f;
constexpr float DISTANCE = 30.0f;

constexpr float PITCH_VAL = 89.0f;
constexpr float Z_NEAR = 0.1f;
constexpr float Z_FAR = 1000.0f;
constexpr float FOV_MIN = 20.0f;
constexpr float FOV = 60.0f;
constexpr float FOV_MAX = 90.0f;

// === SCENE
// -> surrounding
constexpr glm::vec4 SKY_COLOR = glm::vec4(0.5f, 0.4f, 0.8f, 1.0f);
constexpr float FOG_DENSITY = .003f;
constexpr float FOG_GRADIENT = 1.5f;
// -> objects
constexpr glm::vec3 SPHERE_POS = glm::vec3(100.0f, 0.0f, 20.0f);
// -> light
constexpr int LIGHTS_MAX = 20;
constexpr int ENV_MAP_RESOLUTION = 256;
//-> skybox
const std::string SKYBOX_CUBE_TEXTURE_FILE_PREFIX = "textures/skybox/";


// -----------------------------------------------------------------------


// === CUBE VERTICES & NORMALS
constexpr glm::vec3 cubeVertices[24] = {
    // +x
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),

    // -x
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),

    // +y
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),

    // -y
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),

    // +z
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),

    // -z
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
};

constexpr glm::vec3 cubeNormals[6] = {
    // x
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    // y
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    // z
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f)
};
#endif // CONFIG_H