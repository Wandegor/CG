#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform float ambient;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 fillDir = normalize(vec3(-1.0, 0.0, 1.0));
    float fill = max(dot(norm, fillDir), 0.0) * 0.2;

    vec3 result = (ambient + diff + fill) * objectColor;
    FragColor = vec4(result, 1.0);
}