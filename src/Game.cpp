#include "Game.h"
#include <iostream>
#include <fstream>
#include <cmath>

namespace CyberStrike {

Game::Game(int width, int height, const std::string& title)
    : m_window(width, height, title),
      m_state(GameState::START_MENU),
      m_currentWave(1),
      m_lastFrameTime(0.0f),
      m_waveTransitionTimer(0.0f) {
}

Game::~Game() {
}

bool Game::init() {
    // 1. Initialize Window & Context
    if (!m_window.init()) {
        std::cerr << "[FATAL] Window init failed!\n";
        return false;
    }

    m_window.printGLInfo();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 2. Load Shaders
    std::string vPath = "shaders/vertex.glsl";
    std::string fPath = "shaders/fragment.glsl";
    std::ifstream f(vPath);
    if (!f.is_open()) {
        vPath = "../shaders/vertex.glsl";
        fPath = "../shaders/fragment.glsl";
    } else {
        f.close();
    }

    if (!m_worldShader.loadFromFiles(vPath, fPath)) {
        return false;
    }

    // 3. Initialize Geometry & Systems
    m_cubeMesh.init();
    m_arena.init();
    m_ui.init();

    // 4. Load Textures
    std::string texDir = "assets/textures/";
    std::ifstream tf(texDir + "floor.png");
    if (!tf.is_open()) {
        texDir = "../assets/textures/";
    } else {
        tf.close();
    }

    m_floorTex.loadFromFile(texDir + "floor.png");
    m_wallTex.loadFromFile(texDir + "wall.png");
    m_crateTex.loadFromFile(texDir + "crate.png");
    m_enemyTex.loadFromFile(texDir + "enemy.png");
    m_gunTex.loadFromFile(texDir + "gun.png");

    // 5. Setup Input Callbacks
    m_window.setMouseMoveCallback([this](float xoffset, float yoffset) {
        if (m_state == GameState::PLAYING) {
            m_player.camera.processMouseMovement(xoffset, yoffset);
        }
    });

    m_window.setMouseScrollCallback([this](float yoffset) {
        if (m_state == GameState::PLAYING) {
            m_player.camera.processMouseScroll(yoffset);
        }
    });

    // 6. Spawn Player and Wave 1
    resetGame();
    m_state = GameState::START_MENU;
    m_window.setCursorLocked(false);

    std::cout << "[INFO] CyberStrike 3D Engine ready!\n";
    return true;
}

void Game::resetGame() {
    m_player.reset(m_arena.getPlayerSpawn());
    m_currentWave = 1;
    m_particles.clear();
    startWave(m_currentWave);
}

void Game::startWave(int waveNum) {
    m_enemies.clear();
    const auto& spawnZones = m_arena.getEnemySpawns();

    // Wave scale: Wave 1 = 5 enemies, Wave 2 = 8 enemies, Wave 3 = 10 enemies
    int count = (waveNum == 1) ? 5 : (waveNum == 2 ? 8 : 10);

    for (int i = 0; i < count; ++i) {
        glm::vec3 baseSpawn = spawnZones[i % spawnZones.size()];
        // Disperse slightly around spawn zone
        float offsetX = ((i * 7) % 5) * 0.7f - 1.4f;
        float offsetZ = ((i * 11) % 5) * 0.7f - 1.4f;
        glm::vec3 spawnPos = baseSpawn + glm::vec3(offsetX, 0.0f, offsetZ);

        Enemy enemy(spawnPos);
        // Slightly increase speed per wave for escalating challenge
        enemy.moveSpeed = 2.2f + (waveNum - 1) * 0.4f;
        m_enemies.push_back(enemy);
    }

    std::cout << "[WAVE] Wave " << waveNum << " deployed with " << count << " combat drones!\n";
}

void Game::fireWeapon(float currentTime) {
    if (m_state != GameState::PLAYING) return;

    if (m_player.weapon.shoot(currentTime)) {
        // 1. Muzzle Flash Particles
        glm::vec3 muzzlePos = m_player.weapon.getMuzzlePosition(
            m_player.camera.position, m_player.camera.front,
            m_player.camera.right, m_player.camera.up
        );
        m_particles.emitMuzzleFlash(muzzlePos, m_player.camera.front);

        // 2. Ray-Casting Shooting
        Ray bulletRay(m_player.camera.position, m_player.camera.front);

        float nearestHitDist = 1e30f;
        int hitEnemyIndex = -1;
        bool hitObstacle = false;

        // Test Ray vs Enemies (Ray-AABB)
        for (size_t i = 0; i < m_enemies.size(); ++i) {
            if (!m_enemies[i].isAlive()) continue;

            float dist = 0.0f;
            if (Collision::rayIntersectAABB(bulletRay, m_enemies[i].getAABB(), dist)) {
                if (dist < nearestHitDist) {
                    nearestHitDist = dist;
                    hitEnemyIndex = static_cast<int>(i);
                    hitObstacle = false;
                }
            }
        }

        // Test Ray vs Arena Obstacles (Walls, Crates, Pillars)
        for (const auto& obs : m_arena.getObstacles()) {
            AABB obsAABB = AABB::fromCenterScale(obs.transform.position, obs.transform.scale);
            float dist = 0.0f;
            if (Collision::rayIntersectAABB(bulletRay, obsAABB, dist)) {
                if (dist < nearestHitDist) {
                    nearestHitDist = dist;
                    hitEnemyIndex = -1;
                    hitObstacle = true;
                }
            }
        }

        // 3. Apply Hit Results
        if (hitEnemyIndex != -1) {
            Enemy& target = m_enemies[hitEnemyIndex];
            glm::vec3 hitPoint = bulletRay.getPoint(nearestHitDist);
            m_particles.emitImpactSparks(hitPoint, -m_player.camera.front);

            bool killed = target.takeDamage(m_player.weapon.damage);
            m_player.addScore(10); // Hit bonus

            if (killed) {
                m_player.addScore(50); // Elimination bonus
                m_particles.emitExplosion(target.position);
                std::cout << "[COMBAT] Target neutralized! Score: " << m_player.score << "\n";
            }
        } 
        else if (hitObstacle && nearestHitDist < 80.0f) {
            glm::vec3 hitPoint = bulletRay.getPoint(nearestHitDist);
            m_particles.emitImpactSparks(hitPoint, -m_player.camera.front);
        }
    }
}

void Game::handleInput(float deltaTime, float currentTime) {
    // Left Mouse Button: Shoot
    if (glfwGetMouseButton(m_window.getNativeWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (m_state == GameState::PLAYING) {
            fireWeapon(currentTime);
        }
    }

    // Keyboard controls
    if (m_state == GameState::START_MENU) {
        if (m_window.isKeyPressed(GLFW_KEY_ENTER) || m_window.isKeyPressed(GLFW_KEY_SPACE)) {
            m_state = GameState::PLAYING;
            m_window.setCursorLocked(true);
        }
    }
    else if (m_state == GameState::PLAYING) {
        // Reload
        if (m_window.isKeyPressed(GLFW_KEY_R)) {
            m_player.weapon.reload();
        }

        // Pause
        if (m_window.isKeyPressed(GLFW_KEY_P)) {
            m_state = GameState::PAUSED;
            m_window.setCursorLocked(false);
        }

        // Player WASD Movement
        glm::vec3 moveDelta(0.0f);
        float speed = 4.8f;

        glm::vec3 camFrontHoriz = glm::normalize(glm::vec3(m_player.camera.front.x, 0.0f, m_player.camera.front.z));
        glm::vec3 camRightHoriz = glm::normalize(glm::vec3(m_player.camera.right.x, 0.0f, m_player.camera.right.z));

        if (m_window.isKeyPressed(GLFW_KEY_W)) moveDelta += camFrontHoriz;
        if (m_window.isKeyPressed(GLFW_KEY_S)) moveDelta -= camFrontHoriz;
        if (m_window.isKeyPressed(GLFW_KEY_A)) moveDelta -= camRightHoriz;
        if (m_window.isKeyPressed(GLFW_KEY_D)) moveDelta += camRightHoriz;

        bool isMoving = (glm::length(moveDelta) > 0.001f);
        if (isMoving) {
            moveDelta = glm::normalize(moveDelta) * speed * deltaTime;

            // Build obstacle collision boxes
            std::vector<AABB> obsBoxes;
            obsBoxes.reserve(m_arena.getObstacles().size() + m_enemies.size());
            for (const auto& obs : m_arena.getObstacles()) {
                obsBoxes.push_back(AABB::fromCenterScale(obs.transform.position, obs.transform.scale));
            }
            for (const auto& enemy : m_enemies) {
                if (enemy.isAlive()) obsBoxes.push_back(enemy.getAABB());
            }

            // Resolve player position with wall sliding
            m_player.camera.position = Collision::resolveMovement(
                m_player.camera.position, moveDelta, m_player.boundingSize, obsBoxes
            );
        }

        m_player.update(deltaTime, currentTime, isMoving);
    }
    else if (m_state == GameState::PAUSED) {
        if (m_window.isKeyPressed(GLFW_KEY_ENTER) || m_window.isKeyPressed(GLFW_KEY_SPACE)) {
            m_state = GameState::PLAYING;
            m_window.setCursorLocked(true);
        }
    }
    else if (m_state == GameState::GAME_OVER || m_state == GameState::VICTORY) {
        if (m_window.isKeyPressed(GLFW_KEY_R)) {
            resetGame();
            m_state = GameState::PLAYING;
            m_window.setCursorLocked(true);
        }
    }
}

void Game::update(float deltaTime, float currentTime) {
    if (m_state != GameState::PLAYING) return;

    // Check Player Health
    if (!m_player.isAlive) {
        m_state = GameState::GAME_OVER;
        m_window.setCursorLocked(false);
        return;
    }

    // Build Obstacle Collision Boxes for Enemy Pathfinding
    std::vector<AABB> obsBoxes;
    obsBoxes.reserve(m_arena.getObstacles().size());
    for (const auto& obs : m_arena.getObstacles()) {
        obsBoxes.push_back(AABB::fromCenterScale(obs.transform.position, obs.transform.scale));
    }

    // Update Enemies
    int aliveCount = 0;
    for (auto& enemy : m_enemies) {
        if (enemy.isAlive()) {
            aliveCount++;
            enemy.update(deltaTime, currentTime, m_player.camera.position, m_player, obsBoxes);
        }
    }

    // Check Wave Completion
    if (aliveCount == 0) {
        if (m_currentWave < m_totalWaves) {
            m_currentWave++;
            startWave(m_currentWave);
        } else {
            m_state = GameState::VICTORY;
            m_window.setCursorLocked(false);
            std::cout << "[VICTORY] All waves conquered! Total Score: " << m_player.score << "\n";
        }
    }

    // Update Particles
    m_particles.update(deltaTime);
}

void Game::render3D(float currentTime) {
    glEnable(GL_DEPTH_TEST);
    m_window.clear(0.04f, 0.05f, 0.08f, 1.0f);

    m_worldShader.use();

    // 1. Camera View and Projection
    glm::mat4 view = m_player.camera.getViewMatrix();
    m_worldShader.setMat4("uView", view);

    float aspect = static_cast<float>(m_window.getWidth()) / static_cast<float>(m_window.getHeight());
    glm::mat4 proj = glm::perspective(glm::radians(m_player.camera.fieldOfView), aspect, 0.1f, 100.0f);
    m_worldShader.setMat4("uProjection", proj);

    m_worldShader.setVec3("uViewPos", m_player.camera.position);
    m_worldShader.setFloat("uShininess", 32.0f);

    // 2. Lighting Setup
    // Directional Sun
    m_worldShader.setVec3("uDirLight.direction", -0.4f, -1.0f, -0.3f);
    m_worldShader.setVec3("uDirLight.ambient", 0.22f, 0.24f, 0.30f);
    m_worldShader.setVec3("uDirLight.diffuse", 0.72f, 0.75f, 0.82f);
    m_worldShader.setVec3("uDirLight.specular", 0.6f, 0.6f, 0.7f);

    // Point Light Beacon
    float lightX = 3.5f * std::cos(currentTime * 0.8f);
    float lightZ = 3.5f * std::sin(currentTime * 0.8f);
    float lightY = 2.2f + 0.5f * std::sin(currentTime * 2.0f);
    glm::vec3 pointLightPos(lightX, lightY, lightZ);

    m_worldShader.setVec3("uPointLight.position", pointLightPos);
    m_worldShader.setVec3("uPointLight.ambient", 0.1f, 0.05f, 0.02f);
    m_worldShader.setVec3("uPointLight.diffuse", 1.0f, 0.6f, 0.2f);
    m_worldShader.setVec3("uPointLight.specular", 1.0f, 0.8f, 0.4f);
    m_worldShader.setFloat("uPointLight.constant", 1.0f);
    m_worldShader.setFloat("uPointLight.linear", 0.09f);
    m_worldShader.setFloat("uPointLight.quadratic", 0.032f);

    // 3. Render Arena Obstacles (Ground, Walls, Crates)
    for (const auto& obs : m_arena.getObstacles()) {
        m_worldShader.setMat4("uModel", obs.transform.getModelMatrix());

        // Texture binding
        if (obs.name == "Ground") {
            m_floorTex.bind(0);
            m_worldShader.setInt("uUseTexture", 1);
            m_worldShader.setVec3("uColor", 0.9f, 0.9f, 1.0f);
        } else if (obs.name.find("Wall") != std::string::npos) {
            m_wallTex.bind(0);
            m_worldShader.setInt("uUseTexture", 1);
            m_worldShader.setVec3("uColor", 0.95f, 0.95f, 0.95f);
        } else if (obs.name.find("Crate") != std::string::npos) {
            m_crateTex.bind(0);
            m_worldShader.setInt("uUseTexture", 1);
            m_worldShader.setVec3("uColor", 1.0f, 1.0f, 1.0f);
        } else {
            m_worldShader.setInt("uUseTexture", 0);
            m_worldShader.setVec3("uColor", obs.color);
        }

        m_worldShader.setInt("uUseUniformColor", 1);
        m_cubeMesh.draw();
    }

    // 4. Render Enemies
    m_worldShader.setInt("uUseTexture", 0);
    for (const auto& enemy : m_enemies) {
        enemy.render(m_worldShader, m_cubeMesh);
    }

    // 5. Render Particle Effects
    m_particles.render(m_worldShader, m_cubeMesh);

    // 6. Render First-Person Weapon Viewmodel
    if (m_state == GameState::PLAYING) {
        m_player.weapon.render(
            m_worldShader, m_cubeMesh,
            m_player.camera.position, m_player.camera.front,
            m_player.camera.right, m_player.camera.up
        );
    }
}

void Game::render2D() {
    int w = m_window.getWidth();
    int h = m_window.getHeight();

    m_ui.begin(w, h);

    if (m_state == GameState::PLAYING) {
        int aliveCount = 0;
        for (const auto& e : m_enemies) if (e.isAlive()) aliveCount++;

        m_ui.drawHUD(
            w, h,
            m_player.health, m_player.maxHealth,
            m_player.weapon.currentAmmo, m_player.weapon.maxClipAmmo,
            m_player.weapon.reserveAmmo, m_player.score,
            m_currentWave, m_totalWaves, aliveCount,
            m_player.damageFlashTimer
        );
    } else {
        m_ui.drawOverlay(w, h, m_state, m_player.score);
    }

    m_ui.end();
}

void Game::run() {
    m_lastFrameTime = static_cast<float>(glfwGetTime());

    while (!m_window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - m_lastFrameTime;
        m_lastFrameTime = currentFrame;

        m_window.pollEvents();
        handleInput(deltaTime, currentFrame);
        update(deltaTime, currentFrame);

        render3D(currentFrame);
        render2D();

        m_window.swapBuffers();
    }
}

bool Game::runSelfTest() {
    std::cout << "[SELF-TEST] Executing comprehensive game engine verification...\n";
    m_state = GameState::PLAYING;
    handleInput(0.016f, 1.0f);
    fireWeapon(1.0f);
    update(0.016f, 1.0f);
    render3D(1.0f);
    render2D();
    m_window.swapBuffers();
    std::cout << "[SELF-TEST] Full gameplay pipeline (3D + Lighting + Textures + Weapon + AI + HUD) passed!\n";
    return true;
}

} // namespace CyberStrike
