#version 300 es

precision highp float;

// Vertex Attributes
layout (location = 0) in vec3 aPos;       // Local Position
layout (location = 1) in vec3 aNormal;    // Surface Normal
layout (location = 2) in vec2 aTexCoords; // Texture UV Coordinates
layout (location = 3) in vec3 aColor;     // Vertex Color

// Outputs to Fragment Shader
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 vColor;

// Transformation Uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec2 uTexScale; // Allows UV tiling for large surfaces like ground

void main() {
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(uModel))) * aNormal;

    vec2 scale = uTexScale;
    if (scale.x == 0.0) scale = vec2(1.0, 1.0);
    TexCoords = aTexCoords * scale;

    vColor = aColor;
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
