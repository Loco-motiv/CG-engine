#version 460 core

layout(location = 0) in vec3 vertex_position;

uniform mat4 transformMatrix;

void main() {
    gl_Position = transformMatrix * vec4(vertex_position, 1.0);
}