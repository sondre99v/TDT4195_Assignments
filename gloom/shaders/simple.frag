#version 430 core

out vec4 color;
in vec4 fragmentColor;

void main()
{
    color = fragmentColor;
}
