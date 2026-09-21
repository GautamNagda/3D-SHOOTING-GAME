#pragma once

#include "Core/Transform.h"
#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace CyberStrike {

/**
 * @brief Represents a static 3D entity or obstacle in the arena with spatial
 *        transformations, aesthetic coloring, and AABB collision bounds.
 */
struct Obstacle {
    std::string name;
    Transform transform;
    glm::vec3 color;
    int colorMode; // 0 = vertex color, 1 = uniform solid, 2 = modulated

    // Computes the Axis-Aligned Bounding Box (AABB) in World Space
    glm::vec3 getMinBounds() const {
        return transform.position - (transform.scale * 0.5f);
    }

    glm::vec3 getMaxBounds() const {
        return transform.position + (transform.scale * 0.5f);
    }
};

/**
 * @brief Manages the 3D combat arena layout: ground platform, boundary perimeter walls,
 *        tactical cover crates, energy pillars, and player/enemy spawn locations.
 * 
 * Computer Graphics Concept:
 * - Scene Composition & Modular Geometry:
 *   Constructing a rich 3D level environment by composing primitive meshes with unique
 *   affine transformations (TRS matrices) and material properties.
 * - Spatial Coordinate Systems:
 *   Ground, walls, and obstacles are arranged systematically in 3D World Space coordinates.
 * - Axis-Aligned Bounding Boxes (AABB):
 *   Precalculating bounding intervals [min, max] for every obstacle to prepare for
 *   collision detection in subsequent steps.
 */
class Arena {
public:
    Arena();
    ~Arena();

    /**
     * @brief Generates all arena geometry: floor, 4 enclosing walls, cover crates, and pillars.
     */
    void init();

    /**
     * @brief Renders all obstacles in the arena using the active shader and cube mesh.
     */
    void render(const Shader& shader, const CubeMesh& cubeMesh) const;

    // Getters
    const std::vector<Obstacle>& getObstacles() const { return m_obstacles; }
    glm::vec3 getPlayerSpawn() const { return m_playerSpawn; }
    const std::vector<glm::vec3>& getEnemySpawns() const { return m_enemySpawns; }

private:
    std::vector<Obstacle> m_obstacles;
    glm::vec3 m_playerSpawn;
    std::vector<glm::vec3> m_enemySpawns;

    void addObstacle(const std::string& name, const glm::vec3& pos, const glm::vec3& scale,
                     const glm::vec3& color, int colorMode = 1, float rotY = 0.0f);
};

} // namespace CyberStrike
