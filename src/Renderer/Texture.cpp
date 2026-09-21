#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Renderer/Texture.h"

namespace CyberStrike {

Texture::Texture()
    : m_textureID(0), m_width(0), m_height(0), m_channels(0) {
}

Texture::~Texture() {
    destroy();
}

void Texture::destroy() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
}

bool Texture::loadFromFile(const std::string& filePath, bool flipVertically) {
    destroy();

    stbi_set_flip_vertically_on_load(flipVertically);

    unsigned char* data = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data) {
        std::cerr << "[ERROR] Failed to load texture image from: " << filePath 
                  << " (" << stbi_failure_reason() << ")" << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    if (m_channels == 1) format = GL_RED;
    else if (m_channels == 3) format = GL_RGB;
    else if (m_channels == 4) format = GL_RGBA;

    // Generate and bind OpenGL 2D Texture
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Upload pixel data to GPU VRAM
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate Mipmaps for anti-aliased distance viewing
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture wrapping parameters (GL_REPEAT for tiling ground/walls)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters (Bilinear with Mipmapping)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Free CPU image buffer
    stbi_image_free(data);

    std::cout << "[INFO] Texture loaded successfully: " << filePath 
              << " (" << m_width << "x" << m_height << ", " << m_channels << " channels, ID: " 
              << m_textureID << ")" << std::endl;
    return true;
}

void Texture::bind(unsigned int unit) const {
    if (m_textureID != 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace CyberStrike
