#version 430 core

in vec3 position;
in vec4 vertexColor;
out vec4 fragmentColor;

mat4x4 matrixVar;


void main()
{
	mat4 transposedMatrix = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	matrixVar = transpose(transposedMatrix);

	fragmentColor = vertexColor;
    gl_Position = matrixVar * vec4(position, 1.0f);
}
