#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexturePosition;

out vec3 normal;
out vec3 fragPosition;
out vec2 texturePosition;

uniform mat4 transformMatrix;
uniform mat4 modelMatrix;

void main() {
    normal = mat3(transpose(inverse(modelMatrix))) * vertexNormal;
    fragPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    gl_Position = transformMatrix * vec4(vertexPosition, 1.0);
    texturePosition = vertexTexturePosition;
}