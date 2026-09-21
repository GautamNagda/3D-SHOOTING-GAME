#pragma once

#include "Core/GLCommon.h"
#include <glm/glm.hpp>
#include <vector>

namespace CyberStrike {

/**
 * @brief Represents a 3D colored cube mesh rendered using OpenGL VAO and VBO.
 * 
 * Computer Graphics Concept:
 * - Vertex Buffer Object (VBO):
 *   Allocates high-speed video memory (VRAM) on the GPU to hold raw vertex coordinates and attributes.
 * - Vertex Array Object (VAO):
 *   Encapsulates all vertex attribute configurations and buffer bindings into a single state container.
 * - Vertex Attribute Pointer (glVertexAttribPointer):
 *   Instructs the GPU rasterizer how to interpret the byte stream in the VBO (stride, offset, data type).
 * - Primitive Assembly & Triangulation:
 *   Every 3D polygon is decomposed into triangles (GL_TRIANGLES) for rasterization.
 */
class CubeMesh {
public:
    CubeMesh();
    ~CubeMesh();

    // Prevent copying
    CubeMesh(const CubeMesh&) = delete;
    CubeMesh& operator=(const CubeMesh&) = delete;

    /**
     * @brief Allocates GPU buffers (VAO, VBO) and uploads vertex data into VRAM.
     */
    void init();

    /**
     * @brief Binds the VAO and dispatches the draw call (glDrawArrays).
     */
    void draw() const;

    /**
     * @brief Releases GPU memory.
     */
    void destroy();

private:
    GLuint m_vao;
    GLuint m_vbo;
};

} // namespace CyberStrike
