#version 460 core

layout(binding = 1) uniform sampler2D tex;

in vec2 TexCoords;
out vec4 fragColour;

uniform vec4 colour;
uniform vec4 outlineColour;
uniform float outlineThickness; 
uniform vec2 dimensions;      
uniform float cornerRadius;   
uniform bool useTexture;

void main() {
    if (useTexture)
    {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);
        fragColour = sampled * colour;
    } 
    else{
        vec2 pixelPos = (TexCoords - 0.5) * dimensions;
    
        vec2 innerSize = (dimensions * 0.5) - cornerRadius;
    
        float dist = length(max(abs(pixelPos) - innerSize, 0.0)) - cornerRadius;

        float shapeAlpha = 1.0 - smoothstep(-0.5, 0.5, dist); //* eliminate edges on rounded corners

        if (shapeAlpha <= 0.0) {
            discard;
        }

        vec4 finalColour = colour;
        if (outlineThickness > 0.0) {
            float outlineDist = dist + outlineThickness;
            
            float outlineFactor = 1.0 - smoothstep(-0.5, 0.5, outlineDist);
            
            finalColour = mix(outlineColour, colour, outlineFactor);
        }

        fragColour = vec4(finalColour.rgb, finalColour.a * shapeAlpha);
    }
}