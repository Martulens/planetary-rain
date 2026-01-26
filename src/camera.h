#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include "config.h"
#include "glm/glm.hpp"
#include "framework.h"

class Camera {
public:
private:
    glm::vec3 mPosition = CAM_POS;
    glm::vec3 mDirection = CAM_DIR;
    glm::vec3 mUp = CAM_UP;

    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;

    int mWidth;
    int mHeight;

    float mFieldOfView = FOV;
    float mPitch = PITCH;
    float mViewAngle = YAW;
    float mDistance = DISTANCE;
public:
    Camera() = default;
    Camera(int w, int h, glm::vec3 pos, glm::vec3 dir, float angle);

    void update(int w = -1, int h = -1);
    void updateProjection();

    // === MOVEMENT
    // -> angling
    void zoom(float offset);
    void rotatePitch(float offset);
    void rotateAngleAround(float offset);
    void changeDir(glm::vec3 dir);
    void setPosition(glm::vec3 pos);
    // -> position changes
    void moveForward(float amount);
    void moveRight(float amount);
    void moveUp(float amount);

    // === GETTERS
    glm::vec3 getPosition() const { return mPosition; }
    glm::vec3 getDirection() const { return mDirection; }
    glm::vec3 getUp() const { return mUp; }
    glm::vec3 getRight() const { return glm::normalize(glm::cross(mDirection, mUp)); }
    glm::mat4 getViewMatrix() const { return mViewMatrix; }
    glm::mat4 getProjectionMatrix() const { return mProjectionMatrix; }
    float getFOV() const { return mFieldOfView; }

    glm::vec3 getMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const;
};


#endif