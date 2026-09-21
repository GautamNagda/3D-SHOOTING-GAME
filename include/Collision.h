#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

namespace CyberStrike {

/**
 * @brief Axis-Aligned Bounding Box (AABB) defined by minimum and maximum corner points.
 * 
 * Computer Graphics Concept:
 * - Bounding Volume Hierarchy (BVH) & AABB:
 *   Simplified geometric proxy used to accelerate collision detection from O(triangles)
 *   to O(1) interval comparisons.
 */
struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& minP, const glm::vec3& maxP) : min(minP), max(maxP) {}

    // Static constructor from center and scale
    static AABB fromCenterScale(const glm::vec3& center, const glm::vec3& scale) {
        glm::vec3 half = scale * 0.5f;
        return AABB(center - half, center + half);
    }

    glm::vec3 getCenter() const { return (min + max) * 0.5f; }
    glm::vec3 getSize() const { return max - min; }
};

/**
 * @brief 3D Ray defined by origin point and normalized direction vector:
 *        R(t) = O + t * D,  t >= 0
 * 
 * Computer Graphics Concept:
 * - Ray Casting:
 *   Shooting a ray from the camera center into the 3D scene to detect target hits.
 */
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f) {}
    Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) {}

    glm::vec3 getPoint(float t) const { return origin + direction * t; }
};

/**
 * @brief Collision utilities providing AABB overlap tests, Kay-Kajiya slab ray-intersection,
 *        and axis-aligned collision resolution.
 */
class Collision {
public:
    /**
     * @brief Tests if two AABBs overlap across all three spatial axes.
     */
    static bool checkAABBOverlap(const AABB& a, const AABB& b);

    /**
     * @brief Computes Ray vs AABB intersection using the Kay-Kajiya slab algorithm.
     * @param ray 3D Ray from weapon/camera.
     * @param box Target AABB.
     * @param outDistance Distance along the ray (t) to the nearest entry point.
     * @return true if ray intersects the box in front of origin (t > 0).
     */
    static bool rayIntersectAABB(const Ray& ray, const AABB& box, float& outDistance);

    /**
     * @brief Resolves player movement against environment AABBs by testing and sliding per axis.
     * @param currentPos Current position.
     * @param movement Desired displacement (velocity * dt).
     * @param playerSize Bounding size of player (e.g. 0.8, 1.8, 0.8).
     * @param obstacleBoxes List of obstacle bounding boxes.
     * @return glm::vec3 Adjusted new position avoiding penetration.
     */
    static glm::vec3 resolveMovement(const glm::vec3& currentPos,
                                     const glm::vec3& movement,
                                     const glm::vec3& playerSize,
                                     const std::vector<AABB>& obstacleBoxes);
};

} // namespace CyberStrike
