#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 u_Projection;

void main() {
    // Z = 0.0 et W = 1.0 pour de la 2D
    gl_Position = u_Projection * vec4(aPos, 0.0, 1.0);
}