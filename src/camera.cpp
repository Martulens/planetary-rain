#include "camera.h"
#include <iostream>

Camera::Camera(int windowWidth, int windowHeight, glm::vec3 initialPosition, glm::vec3 viewDirection, float viewAngle) {
    width = windowWidth;
    height = windowHeight;
    direction = glm::normalize(viewDirection);
    updateProjection();
}

void Camera::update(int windowWidth, int windowHeight) {
    if (windowWidth > 0) width = windowWidth;
    if (windowHeight > 0) height = windowHeight;

    position = position + glm::vec3(1.75f * direction.x, 4.5f, 1.75f * direction.z);
    direction = glm::normalize(direction);
    viewMatrix = glm::lookAt(position, position + direction, up);
}

void Camera::updateProjection() {
    float aspect = (height > 0) ? static_cast<float>(width) / height : 1.0f;
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspect, 0.1f, 1000.0f);
}

void Camera::zoom(float offset) {
    distance = glm::clamp(distance - offset, 5.0f, 100.0f);
}

void Camera::rotatePitch(float offset) {
    pitch = glm::clamp(pitch + offset, 5.0f, 85.0f);
}


void Camera::rotateAngleAround(float offset) {
    viewAngle += offset;
}

void Camera::changeDir(glm::vec3 dir){
    direction = dir;
}


glm::vec3 Camera::getMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const{
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec4 rayWorld = glm::inverse(viewMatrix) * rayEye;
    return glm::normalize(glm::vec3(rayWorld));
}
