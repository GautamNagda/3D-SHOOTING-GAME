#pragma once

#include "Core/GLCommon.h"
#include <string>
#include <iostream>

namespace CyberStrike {

/**
 * @brief Manages 2D OpenGL texture loading, filtering, mipmapping, and GPU binding.
 * 
 * Computer Graphics Concept:
 * - Texture Mapping & UV Coordinates:
 *   Maps 2D image coordinates (U, V) ranging from [0.0, 1.0] onto 3D polygon surfaces.
 * - Texture Filtering:
 *   GL_LINEAR interpolates between neighboring texels (bilinear filtering) to eliminate pixelation.
 * - Mipmapping:
 *   Pre-generates progressively downscaled versions of the texture (1/2, 1/4, 1/8...)
 *   to eliminate aliasing (Moiré patterns) and conserve memory bandwidth when viewed from a distance.
 */
class Texture {
public:
    Texture();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    /**
     * @brief Loads an image from disk using stb_image and creates a 2D OpenGL texture.
     */
    bool loadFromFile(const std::string& filePath, bool flipVertically = true);

    /**
     * @brief Binds this texture to the specified texture unit (e.g. GL_TEXTURE0).
     */
    void bind(unsigned int unit = 0) const;

    /**
     * @brief Unbinds texture.
     */
    void unbind() const;

    void destroy();

    GLuint getID() const { return m_textureID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLuint m_textureID;
    int m_width;
    int m_height;
    int m_channels;
};

} // namespace CyberStrike
