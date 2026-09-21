#pragma once

#include "Collision.h"
#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace CyberStrike {

class Player; // Forward declaration

enum class EnemyState {
    IDLE,
    CHASE,
    ATTACK,
    DEAD
};

/**
 * @brief Autonomous 3D Cyber-Drone enemy entity with Finite State Machine (FSM) AI,
 *        pathfinding towards player, combat attacks, and dynamic 3D rendering.
 * 
 * Computer Graphics Concept:
 * - Hierarchical 3D Mesh Composition:
 *   The enemy drone is assembled from a central fuselage, side flight thrusters,
 *   and a glowing crimson sensor eye oriented toward the target.
 * - Procedural Hovering Animation:
 *   Harmonic vertical oscillation: y = y0 + A * sin(omega * t).
 * - Finite State Machine (FSM):
 *   IDLE -> CHASE -> ATTACK -> TAKE DAMAGE -> DEAD.
 */
class Enemy {
public:
    glm::vec3 position;
    glm::vec3 size;
    float rotationY;

    int health;
    int maxHealth;
    int attackDamage;
    float moveSpeed;

    float detectionRange;
    float attackRange;
    float attackCooldown;
    float lastAttackTime;

    float hitFlashTimer;
    float hoverTime;
    EnemyState state;

    Enemy(const glm::vec3& spawnPos = glm::vec3(0.0f, 1.0f, -8.0f));
    ~Enemy();

    /**
     * @brief Updates FSM AI: evaluates distance, pursues player, attacks when in range,
     *        and resolves collisions against arena obstacles.
     */
    void update(float deltaTime, float currentTime, const glm::vec3& playerPos,
                Player& player, const std::vector<AABB>& obstacleBoxes);

    /**
     * @brief Inflicts bullet damage on enemy.
     * @return true if enemy died from this hit.
     */
    bool takeDamage(int amount);

    /**
     * @brief Renders the multi-part 3D sci-fi drone model in World Space.
     */
    void render(const Shader& shader, const CubeMesh& cubeMesh) const;

    AABB getAABB() const {
        return AABB::fromCenterScale(position, size);
    }

    bool isAlive() const { return state != EnemyState::DEAD; }
};

} // namespace CyberStrike
