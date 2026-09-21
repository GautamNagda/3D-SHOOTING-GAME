#pragma once

#include "Camera.h"
#include "Weapon.h"
#include "Collision.h"

#include <glm/glm.hpp>
#include <vector>

namespace CyberStrike {

/**
 * @brief Manages the First-Person Player state: Health, Score, Weapon,
 *        Spatial Navigation, and Collision Bounding Volume.
 */
class Player {
public:
    Camera camera;
    Weapon weapon;

    int health;
    int maxHealth;
    int score;
    bool isAlive;

    glm::vec3 boundingSize; // Player collision box (0.8m x 1.8m x 0.8m)
    float damageFlashTimer; // Red screen vignette flash on taking damage

    Player(const glm::vec3& spawnPos = glm::vec3(0.0f, 1.2f, 9.5f));
    ~Player();

    void reset(const glm::vec3& spawnPos);
    void update(float deltaTime, float currentTime, bool isMoving);

    void takeDamage(int amount);
    void addScore(int points);

    // Collision Box in World Space (centered at player eye-height - half height)
    AABB getAABB() const {
        glm::vec3 center = camera.position - glm::vec3(0.0f, boundingSize.y * 0.5f - 0.2f, 0.0f);
        return AABB::fromCenterScale(center, boundingSize);
    }
};

} // namespace CyberStrike
