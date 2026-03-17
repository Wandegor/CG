#version 460 core

layout (location = 0) in vec2 aPos;

uniform mat3 model;

uniform float scr_aspect;

void main() {
    vec3 pos = model * vec3(aPos, 1.0);
    gl_Position = vec4(pos.x * scr_aspect, pos.y, 0.0, 1.0);
}