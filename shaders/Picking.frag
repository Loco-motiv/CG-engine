#version 460 core

uniform uint objectID; 

out uint FragColor;

void main()
{
    FragColor = objectID;
}