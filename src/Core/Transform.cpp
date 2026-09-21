#include "Core/Transform.h"

namespace CyberStrike {

Transform::Transform()
    : position(0.0f, 0.0f, 0.0f),
      rotation(0.0f, 0.0f, 0.0f),
      scale(1.0f, 1.0f, 1.0f) {
}

Transform::Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
    : position(pos),
      rotation(rot),
      scale(scl) {
}

glm::mat4 Transform::getModelMatrix() const {
    // Start with 4x4 Identity Matrix
    glm::mat4 model = glm::mat4(1.0f);

    // 1. Translation: Shifts object from local origin to World Position
    // Matrix:
    // [ 1  0  0  Px ]
    // [ 0  1  0  Py ]
    // [ 0  0  1  Pz ]
    // [ 0  0  0  1  ]
    model = glm::translate(model, position);

    // 2. Rotation: Rotates around X (Pitch), Y (Yaw), and Z (Roll) axes
    // Applied in Yaw -> Pitch -> Roll (or Z-Y-X) order
    if (rotation.y != 0.0f) {
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (rotation.x != 0.0f) {
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (rotation.z != 0.0f) {
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // 3. Scaling: Enlarges or shrinks along local X, Y, Z axes
    // Matrix:
    // [ Sx 0  0  0 ]
    // [ 0  Sy 0  0 ]
    // [ 0  0  Sz 0 ]
    // [ 0  0  0  1 ]
    model = glm::scale(model, scale);

    return model;
}

void Transform::translate(const glm::vec3& delta) {
    position += delta;
}

void Transform::rotate(const glm::vec3& deltaDegrees) {
    rotation += deltaDegrees;
}

void Transform::setScale(float uniformScale) {
    scale = glm::vec3(uniformScale);
}

} // namespace CyberStrike
