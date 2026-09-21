#include "Core/Window.h"

namespace CyberStrike {

Window::Window(int width, int height, const std::string& title)
    : m_width(width),
      m_height(height),
      m_title(title),
      m_window(nullptr),
      m_cursorLocked(true),
      m_firstMouse(true),
      m_lastX(width / 2.0f),
      m_lastY(height / 2.0f) {
}

Window::~Window() {
    destroy();
}

bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW!" << std::endl;
        return false;
    }

#ifndef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#else
    // WebGL2 context hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "[ERROR] Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);

    // Register callbacks
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);

    // Capture and hide the mouse cursor for first-person camera
    if (m_cursorLocked) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

#ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] Failed to initialize GLAD!" << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
        return false;
    }
#endif

    glViewport(0, 0, m_width, m_height);
    glfwSwapInterval(1); // Enable V-Sync

    std::cout << "[INFO] Window and OpenGL Context successfully initialized!" << std::endl;
    return true;
}

bool Window::shouldClose() const {
    return m_window ? glfwWindowShouldClose(m_window) : true;
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
}

void Window::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::printGLInfo() const {
    std::cout << "\n================ GPU HARDWARE & OPENGL INFO ================" << std::endl;
    std::cout << " GPU Vendor   : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << " GPU Renderer : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << " OpenGL Ver   : " << glGetString(GL_VERSION) << std::endl;
    std::cout << " GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << " Window Size  : " << m_width << " x " << m_height << std::endl;
    std::cout << "============================================================\n" << std::endl;
}

bool Window::isKeyPressed(int key) const {
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Window::setCursorLocked(bool locked) {
    m_cursorLocked = locked;
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (locked) {
            m_firstMouse = true; // reset so camera doesn't jump
        }
    }
}

void Window::destroy() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->m_width = width;
        self->m_height = height;
    }
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Toggle cursor lock with ESC or close if already unlocked
        if (self->m_cursorLocked) {
            self->setCursorLocked(false);
            std::cout << "[INPUT] Cursor unlocked. Press [ESC] again to exit, or click window to recapture.\n";
        } else {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    // Recapture mouse when user clicks inside the window
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        self->setCursorLocked(!self->m_cursorLocked);
    }
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->m_cursorLocked) return;

    float xposF = static_cast<float>(xpos);
    float yposF = static_cast<float>(ypos);

    if (self->m_firstMouse) {
        self->m_lastX = xposF;
        self->m_lastY = yposF;
        self->m_firstMouse = false;
    }

    float xoffset = xposF - self->m_lastX;
    float yoffset = self->m_lastY - yposF; // Reversed: Y ranges bottom to top in standard OpenGL

    self->m_lastX = xposF;
    self->m_lastY = yposF;

    if (self->m_mouseCallback) {
        self->m_mouseCallback(xoffset, yoffset);
    }
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_scrollCallback) {
        self->m_scrollCallback(static_cast<float>(yoffset));
    }
}

} // namespace CyberStrike
