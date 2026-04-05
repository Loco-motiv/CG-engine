#version 460 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;

out vec2 TexCoords;

uniform mat4 transformMatrix;

void main() {
    TexCoords = vertexUV;
    gl_Position = transformMatrix * vec4(vertexPosition, 0.0, 1.0);
}