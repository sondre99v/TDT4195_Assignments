#version 430 core

in vec4 position;
in vec4 vertexColor;
out vec4 fragmentColor;

uniform mat4x4 transformMatrix;


void main()
{
	fragmentColor = vertexColor;
    gl_Position = transformMatrix * position;
}
