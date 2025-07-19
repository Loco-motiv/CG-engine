#version 460 core

in vec3 colour;

out vec4 frag_colour;

uniform float alpha;

void main() {
    frag_colour = vec4(colour, alpha);
}