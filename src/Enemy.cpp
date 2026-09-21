#include "Enemy.h"
#include "Player.h"

#include <cmath>
#include <algorithm>

namespace CyberStrike {

Enemy::Enemy(const glm::vec3& spawnPos)
    : position(spawnPos),
      size(1.0f, 1.2f, 1.0f),
      rotationY(0.0f),
      health(100),
      maxHealth(100),
      attackDamage(10),
      moveSpeed(2.4f),
      detectionRange(25.0f),
      attackRange(2.2f),
      attackCooldown(1.0f),
      lastAttackTime(-1.0f),
      hitFlashTimer(0.0f),
      hoverTime(0.0f),
      state(EnemyState::IDLE) {
}

Enemy::~Enemy() {
}

bool Enemy::takeDamage(int amount) {
    if (state == EnemyState::DEAD) return false;

    health = std::max(0, health - amount);
    hitFlashTimer = 0.25f; // Flash white/red

    if (health <= 0) {
        state = EnemyState::DEAD;
        return true; // killed
    }
    return false;
}

void Enemy::update(float deltaTime, float currentTime, const glm::vec3& playerPos,
                   Player& player, const std::vector<AABB>& obstacleBoxes) {
    if (state == EnemyState::DEAD) return;

    // Advance hovering sine animation
    hoverTime += deltaTime * 3.0f;
    float hoverOffset = std::sin(hoverTime) * 0.15f;

    // Decay damage flash
    if (hitFlashTimer > 0.0f) {
        hitFlashTimer = std::max(0.0f, hitFlashTimer - deltaTime);
    }

    // Compute distance and direction to player
    glm::vec3 toPlayer = playerPos - position;
    float distToPlayer = glm::length(toPlayer);

    // Orientation: Rotate around Y-axis to track player
    if (distToPlayer > 0.001f) {
        glm::vec3 dir = glm::normalize(toPlayer);
        rotationY = glm::degrees(std::atan2(dir.x, dir.z));
    }

    // Finite State Machine (FSM) AI Transitions
    if (distToPlayer > detectionRange) {
        // State 1: IDLE (Player outside detection range)
        state = EnemyState::IDLE;
    } 
    else if (distToPlayer <= attackRange) {
        // State 2: ATTACK (Player in close proximity)
        state = EnemyState::ATTACK;

        if (currentTime - lastAttackTime >= attackCooldown) {
            player.takeDamage(attackDamage);
            lastAttackTime = currentTime;
        }
    } 
    else {
        // State 3: CHASE (Move toward player while navigating around obstacles)
        state = EnemyState::CHASE;

        glm::vec3 moveDir = glm::normalize(glm::vec3(toPlayer.x, 0.0f, toPlayer.z));
        glm::vec3 displacement = moveDir * moveSpeed * deltaTime;

        // Resolve collision against arena walls & crates using our AABB slider
        position = Collision::resolveMovement(position, displacement, size, obstacleBoxes);
    }

    // Keep grounded height with hover bobbing
    position.y = 1.0f + hoverOffset;
}

void Enemy::render(const Shader& shader, const CubeMesh& cubeMesh) const {
    if (state == EnemyState::DEAD) return;

    // Transformation matrix for the drone base
    glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), position);
    baseModel = glm::rotate(baseModel, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

    // Determine damage flash color
    bool isFlashing = (hitFlashTimer > 0.0f);
    glm::vec3 hullColor = isFlashing ? glm::vec3(1.0f, 0.9f, 0.9f) : glm::vec3(0.22f, 0.24f, 0.28f);
    glm::vec3 eyeColor  = isFlashing ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 0.1f, 0.15f);

    // 1. Central Drone Fuselage (Dark Carbon Armor)
    {
        glm::mat4 model = glm::scale(baseModel, glm::vec3(0.85f, 0.55f, 0.85f));
        shader.setMat4("uModel", model);
        shader.setVec3("uColor", hullColor);
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }

    // 2. Glowing Crimson Optical Sensor (Front Eye)
    {
        glm::mat4 eyeModel = glm::translate(baseModel, glm::vec3(0.0f, 0.0f, 0.44f));
        eyeModel = glm::scale(eyeModel, glm::vec3(0.35f, 0.22f, 0.12f));
        shader.setMat4("uModel", eyeModel);
        shader.setVec3("uColor", eyeColor);
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }

    // 3. Left Flight Wing / Thruster Pod
    {
        glm::mat4 wingL = glm::translate(baseModel, glm::vec3(-0.55f, 0.05f, 0.0f));
        wingL = glm::scale(wingL, glm::vec3(0.22f, 0.35f, 0.70f));
        shader.setMat4("uModel", wingL);
        shader.setVec3("uColor", isFlashing ? hullColor : glm::vec3(0.85f, 0.15f, 0.20f));
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }

    // 4. Right Flight Wing / Thruster Pod
    {
        glm::mat4 wingR = glm::translate(baseModel, glm::vec3(0.55f, 0.05f, 0.0f));
        wingR = glm::scale(wingR, glm::vec3(0.22f, 0.35f, 0.70f));
        shader.setMat4("uModel", wingR);
        shader.setVec3("uColor", isFlashing ? hullColor : glm::vec3(0.85f, 0.15f, 0.20f));
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }
}

} // namespace CyberStrike
