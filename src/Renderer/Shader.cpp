#include "Renderer/Shader.h"

namespace CyberStrike {

Shader::Shader() : m_programID(0) {
}

Shader::~Shader() {
    destroy();
}

void Shader::destroy() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
}

std::string Shader::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open shader source file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& source, const std::string& shaderName) {
    GLuint shader = glCreateShader(type);
    const char* srcPtr = source.c_str();
    glShaderSource(shader, 1, &srcPtr, nullptr);
    glCompileShader(shader);

    if (!checkCompileErrors(shader, shaderName)) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[ERROR] Shader Compilation Failed (" << type << "):\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

bool Shader::checkLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[ERROR] Shader Program Linking Failed:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    destroy(); // Clean up existing program if reloaded

    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "[ERROR] One or more shader source files were empty or not found." << std::endl;
        return false;
    }

    // Compile Vertex Shader
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode, "VERTEX");
    if (vertexShader == 0) return false;

    // Compile Fragment Shader
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode, "FRAGMENT");
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Create Shader Program & Attach Shaders
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);

    bool linkOk = checkLinkErrors(m_programID);

    // Delete intermediate shader objects once linked into the final program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!linkOk) {
        destroy();
        return false;
    }

    std::cout << "[INFO] Shader program successfully linked! (ID: " << m_programID << ")" << std::endl;
    return true;
}

void Shader::use() const {
    if (m_programID != 0) {
        glUseProgram(m_programID);
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const {
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setInt(const std::string& name, int value) const {
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    }
}

} // namespace CyberStrike
