#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform float ambient;

void main() {
    vec3 ambientResult = ambient * objectColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseResult = diff * objectColor;

    vec3 result = ambientResult + diffuseResult;
    FragColor = vec4(result, 1.0);
}