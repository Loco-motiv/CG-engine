#version 460 core

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_uv;

out vec2 TexCoords;

uniform mat4 transformMatrix;

void main() {
    TexCoords = vertex_uv;
    gl_Position = transformMatrix * vec4(vertex_position, 0.0, 1.0);
}