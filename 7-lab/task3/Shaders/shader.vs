#version 330 core
layout (location = 0) in vec3 aPos; // u и v [0, 1]

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uMorphTime; // [0, 1] (0 - сфера, 1 - тор)

const float PI = 3.14159265359;

out vec3 FragPos;
out vec3 Normal;

void main() {
    float u = aPos.x * 2.0 * PI; // [0, 2PI]
    float v = aPos.y * PI;       // [0, PI] для сферы

    // Сфера
    float R = 1.0;
    vec3 spherePos;
    spherePos.x = R * cos(u) * sin(v);
    spherePos.y = R * sin(u) * sin(v);
    spherePos.z = R * cos(v);
    vec3 sphereNormal = normalize(spherePos);

    // Тор
    float vTorus = aPos.y * 2.0 * PI;
    float rTorus = 0.3; // внутренний радиус
    float RTorus = 1.0; // внешний радиус

    vec3 torusPos;
    torusPos.x = (RTorus + rTorus * cos(vTorus)) * cos(u);
    torusPos.y = (RTorus + rTorus * cos(vTorus)) * sin(u);
    torusPos.z = rTorus * sin(vTorus);

    vec3 centerOfTube = vec3(RTorus * cos(u), RTorus * sin(u), 0.0);
    vec3 torusNormal = normalize(torusPos - centerOfTube);

    // Морфинг
    vec3 finalPos = mix(spherePos, torusPos, uMorphTime);
    vec3 norm = mix(sphereNormal, torusNormal, uMorphTime);

    FragPos = vec3(model * vec4(finalPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * norm;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}