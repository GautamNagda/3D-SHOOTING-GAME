#version 300 es

precision mediump float;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 vColor;

out vec4 FragColor;

// Light structures
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

// Uniforms
uniform vec3 uViewPos;
uniform DirLight uDirLight;
uniform PointLight uPointLight;

// Material properties
uniform vec3 uColor;
uniform int uUseUniformColor;
uniform float uShininess;

// Texture Uniforms
uniform sampler2D uDiffuseMap;
uniform int uUseTexture; // 0 = procedural/solid, 1 = textured

// Function Prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo);

void main() {
    // 1. Calculate Base Surface Albedo
    vec3 albedo = vColor;
    if (uUseTexture == 1) {
        vec4 texColor = texture(uDiffuseMap, TexCoords);
        albedo = texColor.rgb;
        if (uUseUniformColor == 1) {
            albedo *= uColor; // Modulate texture with color tint
        }
    } else if (uUseUniformColor == 1) {
        albedo = uColor;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);

    // 2. Directional Sunlight
    vec3 result = CalcDirLight(uDirLight, norm, viewDir, albedo);

    // 3. Dynamic Omnidirectional Point Light
    result += CalcPointLight(uPointLight, norm, FragPos, viewDir, albedo);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = light.ambient * albedo;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uShininess);
    vec3 specular = light.specular * spec;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(light.position - fragPos);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * albedo * attenuation;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo * attenuation;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uShininess);
    vec3 specular = light.specular * spec * attenuation;

    return ambient + diffuse + specular;
}
