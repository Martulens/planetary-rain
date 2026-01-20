#include "camera.h"
#include <iostream>

Camera::Camera(int windowWidth, int windowHeight, glm::vec3 initialPosition, glm::vec3 viewDirection, float angle) {
    width = windowWidth;
    height = windowHeight;
    position = initialPosition;
    direction = glm::normalize(viewDirection);
    viewAngle = angle;

    updateProjection();
    update(-1, -1);
}

void Camera::update(int windowWidth, int windowHeight) {
    if (windowWidth > 0) width = windowWidth;
    if (windowHeight > 0) height = windowHeight;

    float yaw = glm::radians(viewAngle);
    float p = glm::radians(pitch);

    direction.x = cos(p) * -sin(yaw);
    direction.y = sin(p);
    direction.z = -cos(p) * cos(yaw);
    direction = glm::normalize(direction);

    viewMatrix = glm::lookAt(position, position + direction, up);
    updateProjection();
}

void Camera::updateProjection() {
    float aspect = (height > 0) ? static_cast<float>(width) / height : 1.0f;
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspect, 0.1f, 1000.0f);
}

void Camera::zoom(float offset) {
    fieldOfView = glm::clamp(fieldOfView - offset, 20.0f, 90.0f);
    updateProjection();
}

void Camera::rotatePitch(float offset) {
    pitch = glm::clamp(pitch + offset, -89.0f, 89.0f);
}

void Camera::rotateAngleAround(float offset) {
    viewAngle += offset;
}

void Camera::changeDir(glm::vec3 dir) {
    direction = glm::normalize(dir);
}

void Camera::setPosition(glm::vec3 pos) {
    position = pos;
}

void Camera::moveForward(float amount) {
    glm::vec3 forward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
    position += forward * amount;
}

void Camera::moveRight(float amount) {
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    position += right * amount;
}

void Camera::moveUp(float amount) {
    position += up * amount;
}

glm::vec3 Camera::getMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const {
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec4 rayWorld = glm::inverse(viewMatrix) * rayEye;
    return glm::normalize(glm::vec3(rayWorld));
}