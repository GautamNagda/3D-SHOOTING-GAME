#include "Particle.h"
#include <cstdlib>
#include <cmath>

namespace CyberStrike {

ParticleSystem::ParticleSystem() {
    m_particles.reserve(256);
}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::clear() {
    m_particles.clear();
}

static float randomFloat(float min, float max) {
    return min + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min);
}

void ParticleSystem::emitMuzzleFlash(const glm::vec3& muzzlePos, const glm::vec3& direction) {
    // 6-8 rapid bright sparks and a central flash
    Particle flash;
    flash.position = muzzlePos;
    flash.velocity = direction * 1.5f;
    flash.color = glm::vec3(0.1f, 0.9f, 1.0f); // Bright cyan plasma flash
    flash.size = 0.12f;
    flash.life = 0.05f;
    flash.maxLife = 0.05f;
    m_particles.push_back(flash);

    for (int i = 0; i < 6; ++i) {
        Particle spark;
        spark.position = muzzlePos;
        glm::vec3 spread(randomFloat(-0.3f, 0.3f), randomFloat(-0.3f, 0.3f), randomFloat(-0.3f, 0.3f));
        spark.velocity = (direction + spread) * randomFloat(3.0f, 6.0f);
        spark.color = glm::vec3(0.9f, 0.7f, 0.2f); // Golden spark
        spark.size = 0.04f;
        spark.life = randomFloat(0.08f, 0.14f);
        spark.maxLife = spark.life;
        m_particles.push_back(spark);
    }
}

void ParticleSystem::emitImpactSparks(const glm::vec3& hitPoint, const glm::vec3& hitNormal) {
    for (int i = 0; i < 10; ++i) {
        Particle spark;
        spark.position = hitPoint + hitNormal * 0.05f;
        glm::vec3 randomDir(randomFloat(-1.0f, 1.0f), randomFloat(0.2f, 1.2f), randomFloat(-1.0f, 1.0f));
        spark.velocity = (hitNormal + randomDir) * randomFloat(2.5f, 5.5f);
        spark.color = glm::vec3(1.0f, randomFloat(0.4f, 0.8f), 0.1f); // Hot orange spark
        spark.size = 0.05f;
        spark.life = randomFloat(0.15f, 0.30f);
        spark.maxLife = spark.life;
        m_particles.push_back(spark);
    }
}

void ParticleSystem::emitExplosion(const glm::vec3& centerPos) {
    for (int i = 0; i < 24; ++i) {
        Particle p;
        p.position = centerPos + glm::vec3(randomFloat(-0.2f, 0.2f), randomFloat(-0.2f, 0.2f), randomFloat(-0.2f, 0.2f));
        p.velocity = glm::vec3(randomFloat(-4.0f, 4.0f), randomFloat(1.0f, 6.0f), randomFloat(-4.0f, 4.0f));
        
        // Fiery red/orange/yellow gradient
        float r = randomFloat(0.0f, 1.0f);
        if (r < 0.4f) p.color = glm::vec3(1.0f, 0.2f, 0.1f);
        else if (r < 0.7f) p.color = glm::vec3(1.0f, 0.6f, 0.1f);
        else p.color = glm::vec3(1.0f, 0.9f, 0.3f);

        p.size = randomFloat(0.08f, 0.18f);
        p.life = randomFloat(0.35f, 0.65f);
        p.maxLife = p.life;
        m_particles.push_back(p);
    }
}

void ParticleSystem::update(float deltaTime) {
    const float gravity = -9.8f;

    for (size_t i = 0; i < m_particles.size(); ) {
        m_particles[i].life -= deltaTime;

        if (m_particles[i].life <= 0.0f) {
            // Remove dead particle by swapping with last element (O(1))
            m_particles[i] = m_particles.back();
            m_particles.pop_back();
        } else {
            // Physics Euler integration
            m_particles[i].velocity.y += gravity * deltaTime * 0.5f; // reduced gravity for aesthetic float
            m_particles[i].position += m_particles[i].velocity * deltaTime;
            ++i;
        }
    }
}

void ParticleSystem::render(const Shader& shader, const CubeMesh& cubeMesh) const {
    for (const auto& p : m_particles) {
        float lifeRatio = p.life / p.maxLife;
        float currentScale = p.size * lifeRatio;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position);
        model = glm::scale(model, glm::vec3(currentScale));

        shader.setMat4("uModel", model);
        shader.setVec3("uColor", p.color * (0.5f + 0.5f * lifeRatio));
        shader.setInt("uUseUniformColor", 1);

        cubeMesh.draw();
    }
}

} // namespace CyberStrike
