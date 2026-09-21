#pragma once

#include "Core/Window.h"
#include "Core/Transform.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "Arena.h"
#include "Collision.h"
#include "Weapon.h"
#include "Particle.h"
#include "UI.h"
#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"
#include "Renderer/Texture.h"

#include <vector>
#include <memory>

namespace CyberStrike {

/**
 * @brief Master Game Controller orchestrating the main game loop, wave spawning,
 *        ray-casting shooting, collision resolution, particle effects, and UI state.
 */
class Game {
public:
    Game(int width = 1024, int height = 768, const std::string& title = "CyberStrike 3D");
    ~Game();

    bool init();
    void run();

    // Testing helper to run a single frame and verify
    bool runSelfTest();

private:
    Window m_window;
    Shader m_worldShader;
    CubeMesh m_cubeMesh;
    Arena m_arena;
    Player m_player;
    ParticleSystem m_particles;
    UI m_ui;

    // Textures
    Texture m_floorTex;
    Texture m_wallTex;
    Texture m_crateTex;
    Texture m_enemyTex;
    Texture m_gunTex;

    // Game Waves & State
    GameState m_state;
    int m_currentWave;
    const int m_totalWaves = 3;
    std::vector<Enemy> m_enemies;

    // Timing
    float m_lastFrameTime;
    float m_waveTransitionTimer;

    // Private game loop subroutines
    void startWave(int waveNum);
    void handleInput(float deltaTime, float currentTime);
    void update(float deltaTime, float currentTime);
    void render3D(float currentTime);
    void render2D();
    void fireWeapon(float currentTime);
    void resetGame();
};

} // namespace CyberStrike
