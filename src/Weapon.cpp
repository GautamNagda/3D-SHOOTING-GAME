#include "Weapon.h"
#include <cmath>
#include <algorithm>

namespace CyberStrike {

Weapon::Weapon()
    : currentAmmo(30),
      maxClipAmmo(30),
      reserveAmmo(90),
      damage(35),
      fireRate(0.14f),
      lastShotTime(-1.0f),
      isReloading(false),
      reloadTime(1.5f),
      reloadTimer(0.0f),
      recoilOffset(0.0f),
      bobbingTimer(0.0f) {
}

Weapon::~Weapon() {
}

bool Weapon::shoot(float currentTime) {
    if (isReloading) return false;
    if (currentTime - lastShotTime < fireRate) return false;

    if (currentAmmo <= 0) {
        reload();
        return false;
    }

    currentAmmo--;
    lastShotTime = currentTime;
    recoilOffset = 0.08f; // 8cm kickback
    return true;
}

void Weapon::reload() {
    if (isReloading || currentAmmo == maxClipAmmo || reserveAmmo <= 0) return;
    isReloading = true;
    reloadTimer = reloadTime;
}

void Weapon::update(float deltaTime, float currentTime, bool isMoving) {
    // Smooth recoil recovery (spring back to default position)
    if (recoilOffset > 0.0f) {
        recoilOffset = std::max(0.0f, recoilOffset - deltaTime * 0.7f);
    }

    // Process Reload timer
    if (isReloading) {
        reloadTimer -= deltaTime;
        if (reloadTimer <= 0.0f) {
            int needed = maxClipAmmo - currentAmmo;
            int toLoad = std::min(needed, reserveAmmo);
            currentAmmo += toLoad;
            reserveAmmo -= toLoad;
            isReloading = false;
        }
    }

    // Viewmodel walking bobbing
    if (isMoving) {
        bobbingTimer += deltaTime * 8.0f;
    } else {
        bobbingTimer *= 0.85f; // smoothly settle
    }
}

glm::vec3 Weapon::getMuzzlePosition(const glm::vec3& camPos, const glm::vec3& camFront,
                                   const glm::vec3& camRight, const glm::vec3& camUp) const {
    // Barrel extends forward along the front vector
    return camPos + (camRight * 0.24f) + (camUp * -0.16f) + (camFront * 0.65f);
}

void Weapon::render(const Shader& shader, const CubeMesh& cubeMesh,
                   const glm::vec3& camPos, const glm::vec3& camFront,
                   const glm::vec3& camRight, const glm::vec3& camUp) const {
    // Procedural weapon sway and bobbing offsets
    float bobX = std::cos(bobbingTimer * 0.5f) * 0.012f;
    float bobY = std::abs(std::sin(bobbingTimer)) * 0.015f;
    float reloadDip = isReloading ? -0.15f : 0.0f;

    // Weapon origin in front-right of camera
    glm::vec3 gunOrigin = camPos 
                        + (camRight * (0.24f + bobX)) 
                        + (camUp * (-0.20f + bobY + reloadDip)) 
                        + (camFront * (0.42f - recoilOffset));

    // Construct local camera rotation matrix so gun aligns with camera orientation
    glm::mat4 rotMatrix(1.0f);
    rotMatrix[0] = glm::vec4(camRight, 0.0f);
    rotMatrix[1] = glm::vec4(camUp, 0.0f);
    rotMatrix[2] = glm::vec4(-camFront, 0.0f);

    // 1. Gun Main Receiver Body (Dark Gunmetal)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), gunOrigin);
        model = model * rotMatrix;
        model = glm::scale(model, glm::vec3(0.08f, 0.10f, 0.32f));

        shader.setMat4("uModel", model);
        shader.setVec3("uColor", 0.15f, 0.17f, 0.22f);
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }

    // 2. Gun Barrel (Extends forward)
    {
        glm::vec3 barrelPos = gunOrigin + (camFront * 0.18f) + (camUp * 0.02f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), barrelPos);
        model = model * rotMatrix;
        model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.20f));

        shader.setMat4("uModel", model);
        shader.setVec3("uColor", 0.08f, 0.09f, 0.11f);
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }

    // 3. Ammo Magazine (Extends downward)
    {
        glm::vec3 magPos = gunOrigin + (camFront * -0.04f) + (camUp * -0.09f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), magPos);
        model = model * rotMatrix;
        model = glm::scale(model, glm::vec3(0.05f, 0.12f, 0.07f));

        shader.setMat4("uModel", model);
        shader.setVec3("uColor", 0.25f, 0.28f, 0.35f);
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }

    // 4. Plasma Emitter Stripe (Bright Glowing Cyan Accent)
    {
        glm::vec3 emitterPos = gunOrigin + (camFront * 0.12f) + (camUp * 0.04f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), emitterPos);
        model = model * rotMatrix;
        model = glm::scale(model, glm::vec3(0.045f, 0.025f, 0.12f));

        shader.setMat4("uModel", model);
        shader.setVec3("uColor", 0.0f, 0.85f, 1.0f); // Bright Cyber Cyan
        shader.setInt("uUseUniformColor", 1);
        cubeMesh.draw();
    }
}

} // namespace CyberStrike
