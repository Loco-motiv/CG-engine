#version 460 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 texCoords;
uniform mat4 modelMatrix;

out vec2 vTexCoords;

void main() {
    vTexCoords = texCoords;
    gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}