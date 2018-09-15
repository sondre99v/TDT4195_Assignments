#version 430 core

in vec3 position;
in vec4 vertexColor;
out vec4 fragmentColor;

uniform mat4x4 transformMatrix;


void main()
{
	fragmentColor = vertexColor;
    gl_Position = transformMatrix * vec4(position, 1.0f);
}
