#version 460 core

layout(location = 0) in vec3 vertex_position;

out vec3 colour;

uniform mat4 transformMatrix;
uniform vec3 lightColor;

void main() {
    colour = lightColor;
    gl_Position = transformMatrix * vec4(vertex_position, 1.0);
}