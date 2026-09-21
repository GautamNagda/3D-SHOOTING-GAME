#pragma once

#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"
#include "Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace CyberStrike {

/**
 * @brief First-person Firearm weapon system handling shooting, ammunition,
 *        reloading, recoil animation, and 3D viewmodel rendering.
 * 
 * Computer Graphics Concept:
 * - Viewmodel / Weapon Hierarchy:
 *   The weapon is transformed relative to the camera's local coordinate frame
 *   (Front, Right, Up) so it moves and rotates seamlessly with the player's view.
 * - Recoil & Bobbing Procedural Animation:
 *   Trigonometric interpolation transforms the weapon model in local space to simulate
 *   walking sway and kickback recoil.
 */
class Weapon {
public:
    // Gun specifications
    int currentAmmo;
    int maxClipAmmo;
    int reserveAmmo;
    int damage;

    float fireRate;        // Cooldown between shots (seconds)
    float lastShotTime;
    bool isReloading;
    float reloadTime;      // Total time required to reload (seconds)
    float reloadTimer;

    // Viewmodel animation offsets
    float recoilOffset;
    float bobbingTimer;

    Weapon();
    ~Weapon();

    /**
     * @brief Attempts to fire the weapon. Checks ammo and fire rate cooldown.
     * @return true if shot was successfully fired.
     */
    bool shoot(float currentTime);

    /**
     * @brief Initiates weapon clip reload.
     */
    void reload();

    /**
     * @brief Updates reload timers, recoil recovery, and walking bobbing.
     */
    void update(float deltaTime, float currentTime, bool isMoving);

    /**
     * @brief Renders the 3D first-person gun model in front of the camera.
     */
    void render(const Shader& shader, const CubeMesh& cubeMesh,
                const glm::vec3& camPos, const glm::vec3& camFront,
                const glm::vec3& camRight, const glm::vec3& camUp) const;

    /**
     * @brief Returns the 3D World Space position of the gun barrel for muzzle flash particles.
     */
    glm::vec3 getMuzzlePosition(const glm::vec3& camPos, const glm::vec3& camFront,
                               const glm::vec3& camRight, const glm::vec3& camUp) const;
};

} // namespace CyberStrike
