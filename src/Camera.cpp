#include "Camera.h"
#include <cmath>
#include <algorithm>

namespace CyberStrike {

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch)
    : position(startPosition),
      worldUp(startUp),
      yaw(startYaw),
      pitch(startPitch),
      front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(4.5f),       // 4.5 meters per second
      mouseSensitivity(0.1f),    // 0.1 degrees per mouse pixel
      fieldOfView(60.0f) {       // 60 degrees vertical FOV
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    // LookAt calculation:
    // Eye Position: position
    // Target: position + front
    // Up Vector: up
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime, bool constrainToGround) {
    float velocity = movementSpeed * deltaTime;

    // Determine movement direction vector
    glm::vec3 moveFront = front;
    glm::vec3 moveRight = right;

    if (constrainToGround) {
        // Project front and right onto the X-Z plane to prevent flying or sinking into ground
        moveFront.y = 0.0f;
        moveRight.y = 0.0f;

        if (glm::length(moveFront) > 0.0001f) moveFront = glm::normalize(moveFront);
        if (glm::length(moveRight) > 0.0001f) moveRight = glm::normalize(moveRight);
    }

    if (direction == CameraMovement::FORWARD)  position += moveFront * velocity;
    if (direction == CameraMovement::BACKWARD) position -= moveFront * velocity;
    if (direction == CameraMovement::LEFT)     position -= moveRight * velocity;
    if (direction == CameraMovement::RIGHT)    position += moveRight * velocity;
    if (direction == CameraMovement::UP)       position += worldUp * velocity;
    if (direction == CameraMovement::DOWN)     position -= worldUp * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Constrain pitch to avoid Gimbal Lock and screen inversion
    if (constrainPitch) {
        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the new Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    fieldOfView -= yoffset;
    if (fieldOfView < 30.0f) fieldOfView = 30.0f;
    if (fieldOfView > 90.0f) fieldOfView = 90.0f;
}

void Camera::updateCameraVectors() {
    // Convert Euler angles (degrees to radians) to directional Cartesian vector
    glm::vec3 direction;
    direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    direction.y = std::sin(glm::radians(pitch));
    direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    front = glm::normalize(direction);

    // Orthonormal basis:
    // Right = Front x WorldUp
    // Up    = Right x Front
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

} // namespace CyberStrike
