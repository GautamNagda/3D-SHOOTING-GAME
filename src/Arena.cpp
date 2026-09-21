#include "Arena.h"
#include <iostream>

namespace CyberStrike {

Arena::Arena()
    : m_playerSpawn(0.0f, 1.2f, 9.5f) {
}

Arena::~Arena() {
}

void Arena::addObstacle(const std::string& name, const glm::vec3& pos, const glm::vec3& scale,
                       const glm::vec3& color, int colorMode, float rotY) {
    Obstacle obs;
    obs.name = name;
    obs.transform.position = pos;
    obs.transform.scale = scale;
    obs.transform.rotation = glm::vec3(0.0f, rotY, 0.0f);
    obs.color = color;
    obs.colorMode = colorMode;
    m_obstacles.push_back(obs);
}

void Arena::init() {
    m_obstacles.clear();
    m_enemySpawns.clear();

    // 1. Ground Platform (Floor top surface sits at Y = 0.0)
    // Center at (0, -0.1, 0), scale (26, 0.2, 26) -> Y interval [-0.2, 0.0]
    addObstacle("Ground",
                glm::vec3(0.0f, -0.1f, 0.0f),
                glm::vec3(26.0f, 0.2f, 26.0f),
                glm::vec3(0.12f, 0.14f, 0.18f), 1);

    // 2. Four Outer Perimeter Boundary Walls (Height = 3.0m, Thickness = 0.8m)
    const float wallHeight = 3.0f;
    const float wallY = wallHeight * 0.5f; // Center at Y = 1.5
    const float arenaHalfSize = 13.0f;
    const glm::vec3 wallColor(0.20f, 0.24f, 0.32f);

    // North Wall (Z = -13.0)
    addObstacle("Wall_North",
                glm::vec3(0.0f, wallY, -arenaHalfSize),
                glm::vec3(26.8f, wallHeight, 0.8f),
                wallColor, 1);

    // South Wall (Z = +13.0)
    addObstacle("Wall_South",
                glm::vec3(0.0f, wallY, arenaHalfSize),
                glm::vec3(26.8f, wallHeight, 0.8f),
                wallColor, 1);

    // East Wall (X = +13.0)
    addObstacle("Wall_East",
                glm::vec3(arenaHalfSize, wallY, 0.0f),
                glm::vec3(0.8f, wallHeight, 26.8f),
                wallColor, 1);

    // West Wall (X = -13.0)
    addObstacle("Wall_West",
                glm::vec3(-arenaHalfSize, wallY, 0.0f),
                glm::vec3(0.8f, wallHeight, 26.8f),
                wallColor, 1);

    // 3. Four Corner Energy Columns (Tall pillars with cyber-cyan color)
    const glm::vec3 pillarColor(0.10f, 0.75f, 0.95f);
    const float pillarOffset = 10.0f;
    const float pillarHeight = 4.0f;
    addObstacle("Pillar_NW", glm::vec3(-pillarOffset, pillarHeight * 0.5f, -pillarOffset), glm::vec3(1.2f, pillarHeight, 1.2f), pillarColor, 1);
    addObstacle("Pillar_NE", glm::vec3( pillarOffset, pillarHeight * 0.5f, -pillarOffset), glm::vec3(1.2f, pillarHeight, 1.2f), pillarColor, 1);
    addObstacle("Pillar_SW", glm::vec3(-pillarOffset, pillarHeight * 0.5f,  pillarOffset), glm::vec3(1.2f, pillarHeight, 1.2f), pillarColor, 1);
    addObstacle("Pillar_SE", glm::vec3( pillarOffset, pillarHeight * 0.5f,  pillarOffset), glm::vec3(1.2f, pillarHeight, 1.2f), pillarColor, 1);

    // 4. Central Defensive Bunker & Cover Crates
    // Central platform
    addObstacle("Central_Bunker",
                glm::vec3(0.0f, 0.6f, 0.0f),
                glm::vec3(3.0f, 1.2f, 3.0f),
                glm::vec3(0.25f, 0.30f, 0.40f), 1);

    // North Tactical Barrier
    addObstacle("Barrier_North",
                glm::vec3(0.0f, 0.75f, -5.5f),
                glm::vec3(5.0f, 1.5f, 0.8f),
                glm::vec3(0.28f, 0.33f, 0.42f), 1);

    // Left Sector Cover Crates (Cyber Orange)
    const glm::vec3 crateColor(0.85f, 0.48f, 0.12f);
    addObstacle("Crate_L1", glm::vec3(-5.5f, 0.7f, -3.0f), glm::vec3(1.4f, 1.4f, 1.4f), crateColor, 1, 20.0f);
    addObstacle("Crate_L2", glm::vec3(-4.5f, 0.5f,  3.5f), glm::vec3(1.2f, 1.0f, 1.2f), crateColor, 1, -15.0f);
    addObstacle("Crate_L3", glm::vec3(-7.0f, 0.5f,  1.0f), glm::vec3(1.0f, 1.0f, 1.0f), crateColor, 1, 45.0f);

    // Right Sector Cover Crates
    addObstacle("Crate_R1", glm::vec3( 5.5f, 0.7f, -3.0f), glm::vec3(1.4f, 1.4f, 1.4f), crateColor, 1, -25.0f);
    addObstacle("Crate_R2", glm::vec3( 4.5f, 0.5f,  3.5f), glm::vec3(1.2f, 1.0f, 1.2f), crateColor, 1, 10.0f);
    addObstacle("Crate_R3", glm::vec3( 7.0f, 0.5f,  1.0f), glm::vec3(1.0f, 1.0f, 1.0f), crateColor, 1, 30.0f);

    // 5. Enemy Tactical Spawn Zones
    m_enemySpawns.push_back(glm::vec3(-8.0f, 1.0f, -9.0f));  // NW
    m_enemySpawns.push_back(glm::vec3( 8.0f, 1.0f, -9.0f));  // NE
    m_enemySpawns.push_back(glm::vec3( 0.0f, 1.0f, -10.0f)); // Far North
    m_enemySpawns.push_back(glm::vec3(-9.5f, 1.0f,  0.0f));  // Mid-West
    m_enemySpawns.push_back(glm::vec3( 9.5f, 1.0f,  0.0f));  // Mid-East

    std::cout << "[INFO] Arena initialized successfully! (" 
              << m_obstacles.size() << " obstacles, " 
              << m_enemySpawns.size() << " enemy spawn zones)\n";
}

void Arena::render(const Shader& shader, const CubeMesh& cubeMesh) const {
    for (const auto& obs : m_obstacles) {
        // Upload object's Model Transformation Matrix (TRS)
        shader.setMat4("uModel", obs.transform.getModelMatrix());

        // Upload object's aesthetic color and color mode
        shader.setVec3("uColor", obs.color);
        shader.setInt("uUseUniformColor", obs.colorMode);

        // Draw the cube primitive with this obstacle's transform & color
        cubeMesh.draw();
    }
}

} // namespace CyberStrike
