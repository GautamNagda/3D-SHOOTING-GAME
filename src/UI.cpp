#include "UI.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

namespace CyberStrike {

UI::UI()
    : m_vao(0), m_vbo(0), m_uiProgram(0), m_orthoMatrix(1.0f) {
}

UI::~UI() {
    destroy();
}

void UI::destroy() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_uiProgram != 0) {
        glDeleteProgram(m_uiProgram);
        m_uiProgram = 0;
    }
}

void UI::initShaders() {
#ifdef __EMSCRIPTEN__
    const char* vertexShaderSource = R"(
        #version 300 es
        precision mediump float;
        layout (location = 0) in vec2 aPos;

        uniform mat4 uOrtho;
        uniform vec4 uRect; // [X, Y, Width, Height]

        void main() {
            vec2 pixelPos = uRect.xy + aPos * uRect.zw;
            gl_Position = uOrtho * vec4(pixelPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 300 es
        precision mediump float;
        out vec4 FragColor;
        uniform vec4 uColor;

        void main() {
            FragColor = uColor;
        }
    )";
#else
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;

        uniform mat4 uOrtho;
        uniform vec4 uRect; // [X, Y, Width, Height]

        void main() {
            vec2 pixelPos = uRect.xy + aPos * uRect.zw;
            gl_Position = uOrtho * vec4(pixelPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 uColor;

        void main() {
            FragColor = uColor;
        }
    )";
#endif

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fs);

    m_uiProgram = glCreateProgram();
    glAttachShader(m_uiProgram, vs);
    glAttachShader(m_uiProgram, fs);
    glLinkProgram(m_uiProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void UI::init() {
    initShaders();

    // Standard unit quad [0, 0] to [1, 1] (2 triangles = 6 vertices)
    const float quadVertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "[INFO] 2D UI and Orthographic HUD system initialized!\n";
}

void UI::begin(int screenWidth, int screenHeight) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(m_uiProgram);
    m_orthoMatrix = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));
    glUniformMatrix4fv(glGetUniformLocation(m_uiProgram, "uOrtho"), 1, GL_FALSE, glm::value_ptr(m_orthoMatrix));

    glBindVertexArray(m_vao);
}

void UI::end() {
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void UI::drawRect(float x, float y, float width, float height, const glm::vec4& color) {
    glUniform4f(glGetUniformLocation(m_uiProgram, "uRect"), x, y, width, height);
    glUniform4fv(glGetUniformLocation(m_uiProgram, "uColor"), 1, glm::value_ptr(color));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UI::drawCrosshair(int screenWidth, int screenHeight) {
    float cx = screenWidth * 0.5f;
    float cy = screenHeight * 0.5f;
    glm::vec4 crossColor(0.0f, 1.0f, 1.0f, 0.9f); // Bright Cyber Cyan

    // Horizontal bars
    drawRect(cx - 10.0f, cy - 1.0f, 7.0f, 2.0f, crossColor);
    drawRect(cx + 3.0f,  cy - 1.0f, 7.0f, 2.0f, crossColor);

    // Vertical bars
    drawRect(cx - 1.0f, cy - 10.0f, 2.0f, 7.0f, crossColor);
    drawRect(cx - 1.0f, cy + 3.0f,  2.0f, 7.0f, crossColor);

    // Center dot
    drawRect(cx - 1.0f, cy - 1.0f, 2.0f, 2.0f, glm::vec4(1.0f, 0.3f, 0.3f, 0.95f));
}

void UI::drawDigit(int digit, float x, float y, float size, const glm::vec4& color) {
    float w = size * 0.6f;
    float h = size;
    float t = size * 0.16f; // thickness

    // 7 segments: A (top), B (top-R), C (bot-R), D (bot), E (bot-L), F (top-L), G (mid)
    bool A = (digit != 1 && digit != 4);
    bool B = (digit != 5 && digit != 6);
    bool C = (digit != 2);
    bool D = (digit != 1 && digit != 4 && digit != 7);
    bool E = (digit == 0 || digit == 2 || digit == 6 || digit == 8);
    bool F = (digit != 1 && digit != 2 && digit != 3 && digit != 7);
    bool G = (digit != 0 && digit != 1 && digit != 7);

    float halfH = h * 0.5f;

    if (A) drawRect(x, y + h - t, w, t, color);
    if (B) drawRect(x + w - t, y + halfH, t, halfH, color);
    if (C) drawRect(x + w - t, y, t, halfH, color);
    if (D) drawRect(x, y, w, t, color);
    if (E) drawRect(x, y, t, halfH, color);
    if (F) drawRect(x, y + halfH, t, halfH, color);
    if (G) drawRect(x, y + halfH - t * 0.5f, w, t, color);
}

void UI::drawNumber(int number, float x, float y, float size, const glm::vec4& color) {
    if (number < 0) number = 0;
    std::string s = std::to_string(number);
    float spacing = size * 0.75f;
    for (size_t i = 0; i < s.size(); ++i) {
        int d = s[i] - '0';
        drawDigit(d, x + i * spacing, y, size, color);
    }
}

void UI::drawHUD(int screenWidth, int screenHeight, int health, int maxHealth,
                 int currentAmmo, int maxClip, int reserveAmmo, int score,
                 int currentWave, int totalWaves, int remainingEnemies, float damageFlash) {
    // 1. Full-screen Red Damage Flash Vignette
    if (damageFlash > 0.0f) {
        float alpha = std::min(0.5f, damageFlash * 0.5f);
        drawRect(0, 0, (float)screenWidth, 15.0f, glm::vec4(0.9f, 0.0f, 0.0f, alpha));
        drawRect(0, (float)screenHeight - 15.0f, (float)screenWidth, 15.0f, glm::vec4(0.9f, 0.0f, 0.0f, alpha));
        drawRect(0, 0, 15.0f, (float)screenHeight, glm::vec4(0.9f, 0.0f, 0.0f, alpha));
        drawRect((float)screenWidth - 15.0f, 0, 15.0f, (float)screenHeight, glm::vec4(0.9f, 0.0f, 0.0f, alpha));
    }

    // 2. Center Targeting Crosshair
    drawCrosshair(screenWidth, screenHeight);

    // 3. Health Bar (Bottom Left)
    float hpBarX = 40.0f;
    float hpBarY = 40.0f;
    float hpBarW = 220.0f;
    float hpBarH = 22.0f;

    // Outer border & dark background
    drawRect(hpBarX - 3.0f, hpBarY - 3.0f, hpBarW + 6.0f, hpBarH + 6.0f, glm::vec4(0.1f, 0.12f, 0.16f, 0.9f));
    drawRect(hpBarX, hpBarY, hpBarW, hpBarH, glm::vec4(0.2f, 0.05f, 0.05f, 0.8f));

    // Health Fill Bar (Green -> Yellow -> Red)
    float hpPct = std::max(0.0f, std::min(1.0f, (float)health / (float)maxHealth));
    glm::vec4 hpColor;
    if (hpPct > 0.5f) hpColor = glm::vec4(0.1f, 0.85f, 0.3f, 0.95f);
    else if (hpPct > 0.25f) hpColor = glm::vec4(0.95f, 0.8f, 0.1f, 0.95f);
    else hpColor = glm::vec4(0.95f, 0.15f, 0.15f, 0.95f);

    drawRect(hpBarX, hpBarY, hpBarW * hpPct, hpBarH, hpColor);

    // HP Number Digits
    drawNumber(health, hpBarX + hpBarW + 12.0f, hpBarY + 3.0f, 16.0f, hpColor);

    // 4. Ammo Bar & Bullet Pips (Bottom Right)
    float ammoBarW = 240.0f;
    float ammoBarX = screenWidth - ammoBarW - 40.0f;
    float ammoBarY = 40.0f;

    drawRect(ammoBarX - 3.0f, ammoBarY - 3.0f, ammoBarW + 6.0f, 28.0f, glm::vec4(0.1f, 0.12f, 0.16f, 0.9f));

    // Render individual bullet pips for current clip
    float pipWidth = (ammoBarW - 10.0f) / (float)maxClip;
    for (int i = 0; i < maxClip; ++i) {
        glm::vec4 pipColor = (i < currentAmmo) 
            ? glm::vec4(0.0f, 0.85f, 1.0f, 0.95f) 
            : glm::vec4(0.25f, 0.30f, 0.35f, 0.5f);
        drawRect(ammoBarX + 5.0f + i * pipWidth, ammoBarY + 5.0f, pipWidth - 2.0f, 14.0f, pipColor);
    }

    // Ammo Counter Digits
    drawNumber(currentAmmo, ammoBarX - 45.0f, ammoBarY + 5.0f, 16.0f, glm::vec4(0.0f, 0.85f, 1.0f, 0.95f));

    // 5. Score Plate (Top Left)
    float scoreX = 40.0f;
    float scoreY = screenHeight - 60.0f;
    drawRect(scoreX - 5.0f, scoreY - 5.0f, 180.0f, 32.0f, glm::vec4(0.10f, 0.12f, 0.18f, 0.85f));
    drawRect(scoreX, scoreY + 24.0f, 170.0f, 2.0f, glm::vec4(1.0f, 0.75f, 0.1f, 0.9f)); // Gold accent
    drawNumber(score, scoreX + 15.0f, scoreY + 5.0f, 16.0f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));

    // 6. Wave Indicator (Top Right)
    float waveW = 160.0f;
    float waveX = screenWidth - waveW - 40.0f;
    float waveY = screenHeight - 60.0f;
    drawRect(waveX - 5.0f, waveY - 5.0f, waveW + 10.0f, 32.0f, glm::vec4(0.10f, 0.12f, 0.18f, 0.85f));

    // 3 Segmented Wave Blocks
    float blockW = (waveW - 12.0f) / (float)totalWaves;
    for (int w = 1; w <= totalWaves; ++w) {
        glm::vec4 blockColor;
        if (w < currentWave) blockColor = glm::vec4(0.1f, 0.85f, 0.3f, 0.95f);      // Cleared
        else if (w == currentWave) blockColor = glm::vec4(0.95f, 0.55f, 0.1f, 0.95f); // Active
        else blockColor = glm::vec4(0.25f, 0.28f, 0.35f, 0.5f);                      // Pending

        drawRect(waveX + (w - 1) * (blockW + 6.0f), waveY + 5.0f, blockW, 14.0f, blockColor);
    }
}

void UI::drawOverlay(int screenWidth, int screenHeight, GameState state, int finalScore) {
    if (state == GameState::PLAYING) return;

    // Full screen translucent dark backdrop
    drawRect(0, 0, (float)screenWidth, (float)screenHeight, glm::vec4(0.04f, 0.06f, 0.10f, 0.82f));

    float cardW = 540.0f;
    float cardH = 260.0f;
    float cardX = (screenWidth - cardW) * 0.5f;
    float cardY = (screenHeight - cardH) * 0.5f;

    // Card frame
    drawRect(cardX - 4.0f, cardY - 4.0f, cardW + 8.0f, cardH + 8.0f, glm::vec4(0.15f, 0.20f, 0.30f, 0.95f));
    drawRect(cardX, cardY, cardW, cardH, glm::vec4(0.08f, 0.10f, 0.15f, 0.95f));

    if (state == GameState::START_MENU) {
        // Cyan Accent Stripe
        drawRect(cardX, cardY + cardH - 8.0f, cardW, 8.0f, glm::vec4(0.0f, 0.85f, 1.0f, 1.0f));

        // Start Banner Plate
        drawRect(cardX + 40.0f, cardY + cardH - 65.0f, cardW - 80.0f, 40.0f, glm::vec4(0.12f, 0.16f, 0.24f, 1.0f));
        // Deploy prompt button
        drawRect(cardX + 90.0f, cardY + 50.0f, cardW - 180.0f, 48.0f, glm::vec4(0.0f, 0.65f, 0.85f, 1.0f));
    }
    else if (state == GameState::PAUSED) {
        // Amber Accent Stripe
        drawRect(cardX, cardY + cardH - 8.0f, cardW, 8.0f, glm::vec4(1.0f, 0.65f, 0.1f, 1.0f));
        drawRect(cardX + 90.0f, cardY + 60.0f, cardW - 180.0f, 48.0f, glm::vec4(0.85f, 0.55f, 0.1f, 1.0f));
    }
    else if (state == GameState::GAME_OVER) {
        // Crimson Accent Stripe
        drawRect(cardX, cardY + cardH - 8.0f, cardW, 8.0f, glm::vec4(0.95f, 0.15f, 0.15f, 1.0f));
        // Score Display
        drawNumber(finalScore, cardX + cardW * 0.5f - 30.0f, cardY + cardH * 0.5f, 22.0f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));
        // Restart Button
        drawRect(cardX + 90.0f, cardY + 40.0f, cardW - 180.0f, 45.0f, glm::vec4(0.85f, 0.18f, 0.22f, 1.0f));
    }
    else if (state == GameState::VICTORY) {
        // Emerald Green & Gold Accent Stripe
        drawRect(cardX, cardY + cardH - 8.0f, cardW, 8.0f, glm::vec4(0.1f, 0.95f, 0.4f, 1.0f));
        drawNumber(finalScore, cardX + cardW * 0.5f - 30.0f, cardY + cardH * 0.5f, 22.0f, glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));
        // Play Again Button
        drawRect(cardX + 90.0f, cardY + 40.0f, cardW - 180.0f, 45.0f, glm::vec4(0.1f, 0.8f, 0.35f, 1.0f));
    }
}

} // namespace CyberStrike
