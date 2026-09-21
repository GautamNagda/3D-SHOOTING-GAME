#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CyberStrike {

/**
 * @brief Represents 3D spatial properties (Position, Rotation, Scale) and computes
 *        the corresponding 4x4 Model Transformation Matrix.
 * 
 * Computer Graphics Concept:
 * - Affine Transformations in Homogeneous Coordinates:
 *   A 3D point (x, y, z) is represented as a 4D homogeneous vector (x, y, z, 1).
 *   Translation, rotation, and scaling are represented as 4x4 matrices.
 * - Matrix Multiplication Order:
 *   Matrix multiplication is non-commutative (A * B != B * A).
 *   To transform local model vertices correctly, we apply:
 *   Model Matrix = Translation * Rotation * Scale
 *   v_world = T * (R * (S * v_local))
 */
class Transform {
public:
    glm::vec3 position; // Translation vector (Tx, Ty, Tz) in World Space
    glm::vec3 rotation; // Euler rotation angles in degrees (Pitch, Yaw, Roll)
    glm::vec3 scale;    // Scaling factors (Sx, Sy, Sz)

    Transform();
    Transform(const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0.0f), const glm::vec3& scl = glm::vec3(1.0f));

    /**
     * @brief Computes the combined 4x4 Model Matrix using TRS order (Translation * Rotation * Scale).
     * @return glm::mat4 The composite affine model transformation matrix.
     */
    glm::mat4 getModelMatrix() const;

    // Helper transformation methods
    void translate(const glm::vec3& delta);
    void rotate(const glm::vec3& deltaDegrees);
    void setScale(float uniformScale);
};

} // namespace CyberStrike
