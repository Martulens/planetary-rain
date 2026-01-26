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

    float fieldOfView = 60.0f;

public:
    float pitch = 0.0f;
    float viewAngle = 270.0f;   // yaw
    float distance = 30.0f;

    Camera() = default;
    Camera(int w, int h, glm::vec3 pos, glm::vec3 dir, float angle);

    void update(int w = -1, int h = -1);
    void updateProjection();

    void zoom(float offset);
    void rotatePitch(float offset);
    void rotateAngleAround(float offset);
    void changeDir(glm::vec3 dir);
    void setPosition(glm::vec3 pos);

    // === MOVEMENT
    void moveForward(float amount);
    void moveRight(float amount);
    void moveUp(float amount);

    // === GETTERS
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDirection() const { return direction; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return glm::normalize(glm::cross(direction, up)); }
    glm::mat4 getViewMatrix() const { return viewMatrix; }
    glm::mat4 getProjectionMatrix() const { return projectionMatrix; }
    float getFOV() const { return fieldOfView; }

    glm::vec3 getMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const;
};


#endif