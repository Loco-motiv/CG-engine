#version 460 core

layout(binding = 1) uniform sampler2D tex;

in vec2 TexCoords;
out vec4 fragColour;

uniform vec4 colour;
uniform bool useTexture;

void main() {
    if (useTexture)
    {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);
        fragColour = sampled * colour;
    } 
    else 
    {
        fragColour = colour;
    }
}