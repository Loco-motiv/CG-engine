#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexturePosition;
layout(location = 3) in vec3 vertexTangent;

out vec3 normal;
out vec3 fragPosition;
out vec2 texturePosition;
out mat3 TBN;

uniform mat4 transformMatrix;
uniform mat4 modelMatrix;

void main() {
    normal = mat3(transpose(inverse(modelMatrix))) * vertexNormal;
    vec3 T = normalize(vec3(modelMatrix * vec4(vertexTangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0)));
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    fragPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    gl_Position = transformMatrix * vec4(vertexPosition, 1.0);
    texturePosition = vertexTexturePosition;
}