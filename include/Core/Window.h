#pragma once

#include "Core/GLCommon.h"

#include <string>
#include <iostream>
#include <functional>

namespace CyberStrike {

/**
 * @brief Window class encapsulates GLFW window management, input callbacks,
 *        OpenGL context creation, and GLAD function pointer loading.
 */
class Window {
public:
    using MouseMoveCallback = std::function<void(float xoffset, float yoffset)>;
    using MouseScrollCallback = std::function<void(float yoffset)>;

    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool init();
    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();
    void clear(float r = 0.08f, float g = 0.11f, float b = 0.16f, float a = 1.0f);
    void printGLInfo() const;
    void destroy();

    // Input helpers
    bool isKeyPressed(int key) const;
    void setCursorLocked(bool locked);
    bool isCursorLocked() const { return m_cursorLocked; }

    // Callback setters
    void setMouseMoveCallback(MouseMoveCallback cb) { m_mouseCallback = cb; }
    void setMouseScrollCallback(MouseScrollCallback cb) { m_scrollCallback = cb; }

    // Getters
    GLFWwindow* getNativeWindow() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_window;

    bool m_cursorLocked;
    bool m_firstMouse;
    float m_lastX;
    float m_lastY;

    MouseMoveCallback m_mouseCallback;
    MouseScrollCallback m_scrollCallback;

    // GLFW callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

} // namespace CyberStrike
