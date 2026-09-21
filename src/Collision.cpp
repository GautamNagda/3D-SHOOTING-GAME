#include "Collision.h"

namespace CyberStrike {

bool Collision::checkAABBOverlap(const AABB& a, const AABB& b) {
    // Check overlap across all three spatial dimensions simultaneously
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool Collision::rayIntersectAABB(const Ray& ray, const AABB& box, float& outDistance) {
    // The Kay-Kajiya "Slab" Algorithm
    // An AABB is the intersection of three pairs of parallel planes (slabs: X, Y, Z).
    // We compute the ray entry (tmin) and exit (tmax) intervals across each slab.
    float tmin = -1e30f;
    float tmax =  1e30f;

    // Check X Slab
    if (std::abs(ray.direction.x) > 1e-6f) {
        float invD = 1.0f / ray.direction.x;
        float t1 = (box.min.x - ray.origin.x) * invD;
        float t2 = (box.max.x - ray.origin.x) * invD;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmin > tmax) return false;
    } else if (ray.origin.x < box.min.x || ray.origin.x > box.max.x) {
        return false;
    }

    // Check Y Slab
    if (std::abs(ray.direction.y) > 1e-6f) {
        float invD = 1.0f / ray.direction.y;
        float t1 = (box.min.y - ray.origin.y) * invD;
        float t2 = (box.max.y - ray.origin.y) * invD;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmin > tmax) return false;
    } else if (ray.origin.y < box.min.y || ray.origin.y > box.max.y) {
        return false;
    }

    // Check Z Slab
    if (std::abs(ray.direction.z) > 1e-6f) {
        float invD = 1.0f / ray.direction.z;
        float t1 = (box.min.z - ray.origin.z) * invD;
        float t2 = (box.max.z - ray.origin.z) * invD;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmin > tmax) return false;
    } else if (ray.origin.z < box.min.z || ray.origin.z > box.max.z) {
        return false;
    }

    // Hit occurred behind ray origin
    if (tmax < 0.0f) return false;

    // Return the nearest positive intersection point
    outDistance = (tmin >= 0.0f) ? tmin : tmax;
    return true;
}

glm::vec3 Collision::resolveMovement(const glm::vec3& currentPos,
                                     const glm::vec3& movement,
                                     const glm::vec3& playerSize,
                                     const std::vector<AABB>& obstacleBoxes) {
    glm::vec3 resolvedPos = currentPos;

    // 1. Resolve X-Axis Movement (Permits sliding along Z-facing walls)
    if (std::abs(movement.x) > 0.0001f) {
        glm::vec3 candidatePos = resolvedPos;
        candidatePos.x += movement.x;
        AABB candidateAABB = AABB::fromCenterScale(candidatePos, playerSize);

        bool collidesX = false;
        for (const auto& obs : obstacleBoxes) {
            if (checkAABBOverlap(candidateAABB, obs)) {
                collidesX = true;
                break;
            }
        }
        if (!collidesX) {
            resolvedPos.x = candidatePos.x;
        }
    }

    // 2. Resolve Z-Axis Movement (Permits sliding along X-facing walls)
    if (std::abs(movement.z) > 0.0001f) {
        glm::vec3 candidatePos = resolvedPos;
        candidatePos.z += movement.z;
        AABB candidateAABB = AABB::fromCenterScale(candidatePos, playerSize);

        bool collidesZ = false;
        for (const auto& obs : obstacleBoxes) {
            if (checkAABBOverlap(candidateAABB, obs)) {
                collidesZ = true;
                break;
            }
        }
        if (!collidesZ) {
            resolvedPos.z = candidatePos.z;
        }
    }

    return resolvedPos;
}

} // namespace CyberStrike
