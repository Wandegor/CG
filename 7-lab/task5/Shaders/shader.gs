#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 65) out;

uniform float uRadius;
uniform float uAspect;

const float PI = 3.14159265359;

void main() {
    vec4 center = gl_in[0].gl_Position;

    int segments = 20;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0 * PI / segments;

        float dx = cos(angle) * uRadius;
        float dy = sin(angle) * uRadius;

        dx = dx / uAspect;

        gl_Position = center + vec4(dx, dy, 0.0, 0.0);
        EmitVertex(); // Отправка вершины дальше по конвейеру
    }

    EndPrimitive();
}