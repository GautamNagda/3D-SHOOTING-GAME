#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace CyberStrike {

/**
 * @brief Encapsulates a GLSL shader program (compilation, linking, and uniform updates).
 * 
 * Computer Graphics Concept:
 * - Programmable Pipeline:
 *   Replaces the legacy OpenGL 1.x fixed-function pipeline with custom vertex
 *   and fragment processing executed directly in parallel on GPU shader cores.
 */
class Shader {
public:
    Shader();
    ~Shader();

    // Prevent copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /**
     * @brief Reads, compiles, and links vertex and fragment shaders from files.
     */
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Activates this shader program in the OpenGL state machine (glUseProgram).
     */
    void use() const;

    /**
     * @brief Uniform setters to pass CPU variables into GPU shader registers.
     */
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    GLuint getID() const { return m_programID; }
    void destroy();

private:
    GLuint m_programID;

    // Helper functions for shader compilation and link error checking
    std::string readFile(const std::string& filePath);
    GLuint compileShader(GLenum type, const std::string& source, const std::string& shaderName);
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors(GLuint program);
};

} // namespace CyberStrike
