#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace CyberStrike {

enum class GameState {
    START_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    VICTORY
};

/**
 * @brief Heads-Up Display (HUD) and 2D Menu renderer using 2D Orthographic Projection.
 * 
 * Computer Graphics Concept:
 * - Orthographic Projection (2D HUD):
 *   glm::ortho(0, width, 0, height) projects 2D screen-space pixel coordinates directly
 *   into Normalized Device Coordinates (NDC) with no perspective foreshortening.
 * - 2D UI Quad Rendering & Blending:
 *   Alpha blending (glEnable(GL_BLEND), glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))
 *   enables translucent overlays, damage vignettes, and HUD panels.
 * - Depth Test Disabling for UI:
 *   glDisable(GL_DEPTH_TEST) ensures 2D UI elements are always drawn on top of the 3D scene.
 */
class UI {
public:
    UI();
    ~UI();

    void init();
    void destroy();

    /**
     * @brief Begins 2D HUD rendering pass (disables depth testing, enables alpha blending).
     */
    void begin(int screenWidth, int screenHeight);

    /**
     * @brief Ends 2D HUD rendering pass (restores 3D depth testing).
     */
    void end();

    /**
     * @brief Draws a 2D colored rectangle in screen-pixel space.
     * @param x Left coordinate (pixels).
     * @param y Bottom coordinate (pixels).
     * @param width Width (pixels).
     * @param height Height (pixels).
     * @param color RGBA color vector.
     */
    void drawRect(float x, float y, float width, float height, const glm::vec4& color);

    /**
     * @brief Draws the first-person targeting crosshair at the screen center.
     */
    void drawCrosshair(int screenWidth, int screenHeight);

    /**
     * @brief Draws the complete in-game HUD (Health bar, Ammo counter, Wave indicator, Score).
     */
    void drawHUD(int screenWidth, int screenHeight, int health, int maxHealth,
                 int currentAmmo, int maxClip, int reserveAmmo, int score,
                 int currentWave, int totalWaves, int remainingEnemies, float damageFlash);

    /**
     * @brief Draws full-screen overlay menus: Start Menu, Pause, Game Over, and Victory screens.
     */
    void drawOverlay(int screenWidth, int screenHeight, GameState state, int finalScore);

    /**
     * @brief Draws numeric digits (0-9) using 2D geometric segments.
     */
    void drawNumber(int number, float x, float y, float size, const glm::vec4& color);

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_uiProgram;

    glm::mat4 m_orthoMatrix;

    void initShaders();
    void drawDigit(int digit, float x, float y, float size, const glm::vec4& color);
};

} // namespace CyberStrike
