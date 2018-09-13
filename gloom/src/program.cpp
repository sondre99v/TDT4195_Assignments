// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

GLuint createTrianglesVAO(
	GLfloat vertices[], int verticesLength, 
	GLuint indices[], int indicesLength, 
	GLfloat rgbaColors[], int colorsLength,
	GLuint positionAttributeLocation,
	GLuint colorAttributeLocation)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(positionAttributeLocation);
	glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint colorBufferObject;
	glGenBuffers(1, &colorBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, colorsLength * sizeof(GLfloat), rgbaColors, GL_STATIC_DRAW);

	glEnableVertexAttribArray(colorAttributeLocation);
	glVertexAttribPointer(colorAttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(GLuint), indices, GL_STATIC_DRAW);

	return vao;
}

void runProgram(GLFWwindow* window)
{
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set default colour after clearing the colour buffer
	glClearColor(0.39f, 0.58f, 0.92f, 1.0f);

	// Set up your scene here (create Vertex Array Objects, etc.)


	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.frag");
	shader.attach("../gloom/shaders/simple.vert");
	shader.link();

	shader.activate();

	// Create and fill vertex buffer for six triangles
	const int numberOfTriangles = 3;

	GLfloat vertices[] = {
		-0.5f, 0.5f, -0.5f,
		0.0f, -0.2f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.2f, 0.0f,
		-0.8f, 0.2f, 0.5f,
		-0.8f, -0.2f, 0.5f,
		0.8f, 0.0f, 0.5f
	};
	
	GLfloat colors[] = {
		1, 0, 0, 0.5f,
		1, 0, 0, 0.5f,
		1, 0, 0, 0.5f,
		0, 1, 0, 0.5f,
		0, 1, 0, 0.5f,
		0, 1, 0, 0.5f,
		0, 0, 1, 0.5f,
		0, 0, 1, 0.5f,
		0, 0, 1, 0.5f
	};
	
	GLuint indices[] = {
		6, 7, 8,
		3, 4, 5,
		0, 1, 2
	};


	GLint positionAttribute = glGetAttribLocation(shader.get(), "position");
	GLint colorAttribute = glGetAttribLocation(shader.get(), "vertexColor");

	GLuint vao = createTrianglesVAO(
		vertices, numberOfTriangles * 3 * 3,
		indices, numberOfTriangles * 3,
		colors, numberOfTriangles * 3 * 4,
		positionAttribute,
		colorAttribute);

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		printGLError();

		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw triangles
		glDrawElements(GL_TRIANGLES, numberOfTriangles * 3, GL_UNSIGNED_INT, 0);

		// Handle other events
		glfwPollEvents();
		handleKeyboardInput(window);

		// Flip buffers
		glfwSwapBuffers(window);
	}
}


void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}
