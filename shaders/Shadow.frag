#version 460 core
layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 3) uniform sampler2D alphaMap;
in vec4 FragPos;
in vec2 FragTexCoords;

uniform bool hasDiffuseTexture;
uniform bool hasAlphaTexture;
uniform vec3 lightPos;
uniform float farPlane;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos);
    float dAlpha = 1.0;
    float mAlpha = 1.0;
    if (hasDiffuseTexture) {
        dAlpha = texture(diffuseMap, FragTexCoords).a;
    }
    if (hasAlphaTexture) {
        mAlpha = texture(alphaMap, FragTexCoords).r;
    }

    if (dAlpha < 0.5 || mAlpha < 0.5) {
        discard;
    }
    lightDistance = lightDistance / farPlane;
    
    gl_FragDepth = lightDistance;
}