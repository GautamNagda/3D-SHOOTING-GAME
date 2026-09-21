#pragma once

#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"

#include <glm/glm.hpp>
#include <vector>

namespace CyberStrike {

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float size;
    float life;
    float maxLife;

    bool isAlive() const { return life > 0.0f; }
};

/**
 * @brief Lightweight 3D particle system managing muzzle flashes, bullet impact sparks,
 *        and enemy destruction explosions.
 * 
 * Computer Graphics Concept:
 * - Particle Systems:
 *   Simulating fluid/amorphous visual phenomena (fire, sparks, flashes) by updating
 *   large numbers of simple geometric primitives with velocity, gravity, and lifetime decay.
 */
class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void update(float deltaTime);
    void render(const Shader& shader, const CubeMesh& cubeMesh) const;

    /**
     * @brief Spawns a bright muzzle flash burst at the gun barrel.
     */
    void emitMuzzleFlash(const glm::vec3& muzzlePos, const glm::vec3& direction);

    /**
     * @brief Spawns impact sparks where a ray-cast bullet hits a wall, crate, or target.
     */
    void emitImpactSparks(const glm::vec3& hitPoint, const glm::vec3& hitNormal);

    /**
     * @brief Spawns a fiery particle explosion when an enemy is destroyed.
     */
    void emitExplosion(const glm::vec3& centerPos);

    void clear();

private:
    std::vector<Particle> m_particles;
};

} // namespace CyberStrike
