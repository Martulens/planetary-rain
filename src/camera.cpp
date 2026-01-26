#include "camera.h"
#include <iostream>

#include "config.h"

Camera::Camera(int windowWidth, int windowHeight, glm::vec3 initialPosition, glm::vec3 viewDirection, float angle) {
    mWidth = windowWidth;
    mHeight = windowHeight;
    mPosition = initialPosition;
    mDirection = glm::normalize(viewDirection);
    mViewAngle = angle;

    updateProjection();
    update(-1, -1);
}

void Camera::update(int windowWidth, int windowHeight) {
    if (windowWidth > 0) mWidth = windowWidth;
    if (windowHeight > 0) mHeight = windowHeight;

    float yaw = glm::radians(mViewAngle);
    float p = glm::radians(mPitch);

    mDirection.x = cos(p) * -sin(yaw);
    mDirection.y = sin(p);
    mDirection.z = -cos(p) * cos(yaw);
    mDirection = glm::normalize(mDirection);

    //std::cout << "[CAMERA] mDirection: " << mDirection.x << ", " << mDirection.y << ", " << mDirection.z << std::endl;
    mViewMatrix = glm::lookAt(mPosition, mPosition + mDirection, mUp);
    updateProjection();
}

void Camera::updateProjection() {
    float aspect = (mHeight > 0) ? static_cast<float>(mWidth) / mHeight : 1.0f;
    mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView),
                                            aspect, Z_NEAR, Z_FAR);
}

void Camera::zoom(float offset) {
    mFieldOfView = glm::clamp(mFieldOfView - offset, FOV_MIN, FOV_MAX);
    updateProjection();
}

void Camera::rotatePitch(float offset) {
    mPitch = glm::clamp(mPitch + offset, -PITCH_VAL, PITCH_VAL);
}

void Camera::rotateAngleAround(float offset) {
    mViewAngle += offset;
}

void Camera::changeDir(glm::vec3 dir) {
    mDirection = glm::normalize(dir);
}

void Camera::setPosition(glm::vec3 pos) {
    mPosition = pos;
}

void Camera::moveForward(float amount) {
    glm::vec3 forward = glm::normalize(glm::vec3(mDirection.x, mDirection.y, mDirection.z));
    mPosition += forward * amount;
}

void Camera::moveRight(float amount) {
    glm::vec3 right = glm::normalize(glm::cross(mDirection, mUp));
    mPosition += right * amount;
}

void Camera::moveUp(float amount) {
    mPosition += mUp * amount;
}

glm::vec3 Camera::getMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const {
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(mProjectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec4 rayWorld = glm::inverse(mViewMatrix) * rayEye;
    return glm::normalize(glm::vec3(rayWorld));
}