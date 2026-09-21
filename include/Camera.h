#pragma once

#include "Core/GLCommon.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CyberStrike {

// Enumeration for directional camera movements
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

/**
 * @brief First-person 3D Camera system implementing Euler angles, mouse-look,
 *        and LookAt View matrix calculation.
 * 
 * Computer Graphics Concept:
 * - View Transformation:
 *   Transforms vertices from 3D World Space into View / Camera Space, where the camera
 *   is positioned at the origin (0, 0, 0) and oriented looking down the negative Z-axis.
 * - Orthonormal Camera Basis:
 *   Constructed from three mutually perpendicular unit vectors:
 *   Right (R), Up (U), and Forward/Direction (F).
 * - Euler Angles:
 *   Yaw (horizontal rotation around Y) and Pitch (vertical tilt up/down).
 *   Pitch is clamped to [-89 deg, +89 deg] to avoid Gimbal Lock.
 */
class Camera {
public:
    // Camera spatial attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler Angles (in degrees)
    float yaw;
    float pitch;

    // Camera operational parameters
    float movementSpeed;
    float mouseSensitivity;
    float fieldOfView; // in degrees

    /**
     * @brief Constructor initializing default FPS camera state.
     */
    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 1.5f, 5.0f),
           glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f),
           float startYaw = -90.0f,
           float startPitch = 0.0f);

    /**
     * @brief Computes the 4x4 View Matrix using glm::lookAt.
     * @return glm::mat4 The View Transformation Matrix.
     */
    glm::mat4 getViewMatrix() const;

    /**
     * @brief Processes keyboard input (WASD) with delta-time normalization.
     * @param direction Movement direction enum.
     * @param deltaTime Elapsed time between current and previous frame.
     * @param constrainToGround If true, movement is strictly horizontal (FPS walking).
     */
    void processKeyboard(CameraMovement direction, float deltaTime, bool constrainToGround = true);

    /**
     * @brief Processes mouse movement offsets to update camera Yaw and Pitch.
     * @param xoffset Delta X movement from mouse.
     * @param yoffset Delta Y movement from mouse.
     * @param constrainPitch Prevents camera from flipping upside down (> 89 degrees).
     */
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    /**
     * @brief Processes mouse scroll wheel to adjust field of view (zoom).
     */
    void processMouseScroll(float yoffset);

private:
    /**
     * @brief Recalculates Front, Right, and Up unit vectors from updated Euler angles.
     */
    void updateCameraVectors();
};

} // namespace CyberStrike
