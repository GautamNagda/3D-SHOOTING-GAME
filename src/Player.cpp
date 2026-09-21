#include "Player.h"
#include <algorithm>

namespace CyberStrike {

Player::Player(const glm::vec3& spawnPos)
    : camera(spawnPos, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f),
      health(100),
      maxHealth(100),
      score(0),
      isAlive(true),
      boundingSize(0.8f, 1.8f, 0.8f),
      damageFlashTimer(0.0f) {
}

Player::~Player() {
}

void Player::reset(const glm::vec3& spawnPos) {
    camera.position = spawnPos;
    camera.yaw = -90.0f;
    camera.pitch = 0.0f;
    camera.fieldOfView = 60.0f;

    health = maxHealth;
    score = 0;
    isAlive = true;
    damageFlashTimer = 0.0f;

    weapon.currentAmmo = weapon.maxClipAmmo;
    weapon.reserveAmmo = 90;
    weapon.isReloading = false;
    weapon.recoilOffset = 0.0f;
}

void Player::update(float deltaTime, float currentTime, bool isMoving) {
    weapon.update(deltaTime, currentTime, isMoving);

    if (damageFlashTimer > 0.0f) {
        damageFlashTimer = std::max(0.0f, damageFlashTimer - deltaTime * 3.0f);
    }
}

void Player::takeDamage(int amount) {
    if (!isAlive) return;

    health = std::max(0, health - amount);
    damageFlashTimer = 1.0f; // Trigger red damage feedback

    if (health <= 0) {
        isAlive = false;
        std::cout << "[GAME] Player eliminated! Health reached 0.\n";
    }
}

void Player::addScore(int points) {
    score += points;
}

} // namespace CyberStrike
