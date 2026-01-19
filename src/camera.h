#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include "glm/glm.hpp"
#include "framework.h"

class Camera {
public:
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    int width;
    int height;

    bool firstPerson = true;
    float fieldOfView = 60.0f;

public:
    float pitch = 20.0f;
    float viewAngle = 0.0f;
    float distance = 30.0f;

    Camera() = default;
    Camera(int w, int h, glm::vec3 pos, glm::vec3 up, float angle);

    void update(int w = -1, int h = -1);
    void updateProjection();

    void zoom(float offset);
    void rotatePitch(float offset);
    void rotateAngleAround(float offset);
    void changeDir(glm::vec3 dir);

    glm::vec3 getMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const;
};

#endif CAMERA_CLASS_H