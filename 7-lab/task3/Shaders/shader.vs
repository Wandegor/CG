#version 330 core
layout (location = 0) in vec3 aPos; // Это наши u и v в диапазоне [0, 1]

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uMorphTime; // Значение от 0.0 до 1.0 (0 - сфера, 1 - тор)

const float PI = 3.14159265359;

void main() {
    float u = aPos.x * 2.0 * PI; // [0, 2PI]
    float v = aPos.y * PI;       // [0, PI] для сферы

    // 1. Координаты сферы
    float R = 1.0;
    vec3 spherePos;
    spherePos.x = R * cos(u) * sin(v);
    spherePos.y = R * sin(u) * sin(v);
    spherePos.z = R * cos(v);

    // 2. Координаты тора
    // Для тора v тоже должен быть до 2PI, пересчитаем локально
    float vTorus = aPos.y * 2.0 * PI;
    float rTorus = 0.3; // внутренний радиус
    float RTorus = 0.7; // внешний радиус

    vec3 torusPos;
    torusPos.x = (RTorus + rTorus * cos(vTorus)) * cos(u);
    torusPos.y = (RTorus + rTorus * cos(vTorus)) * sin(u);
    torusPos.z = rTorus * sin(vTorus);

    // МОРФИНГ
    vec3 finalPos = mix(spherePos, torusPos, uMorphTime);

    gl_Position = projection * view * model * vec4(finalPos, 1.0);
}