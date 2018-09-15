// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

// Define camera
const float cameraSpeed = 0.1;
const float cameraRotationSpeed = 0.03;
float cameraX = 8.77;
float cameraY = 5.34;
float cameraZ = 17.78;
float cameraPitch = -0.7;
float cameraYaw = 0.45;

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

	
	// Create the shader program
	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.frag");
	shader.attach("../gloom/shaders/simple.vert");
	shader.link();

	shader.activate();


	// Define vertex buffer for the scene geometry
	const int numberOfTriangles = 3;

	GLfloat vertices[] = {
		// Floor:
		-10, -10, -10,
		-10, -10, 10,
		10, -10, -10,
		10, -10, -10,
		-10, -10, 10,
		10, -10, 10,

		// Floating triangle:
		-0.8f, 0.5f, -4.0f,
		-0.8f, -0.5f, -4.0f,
		0.8f, 0.0f, -4.0f
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
		0, 1, 2,
		3, 4, 5,
		6, 7, 8
	};


	// Get attribute locations in shader
	GLint positionAttribute = glGetAttribLocation(shader.get(), "position");
	GLint colorAttribute = glGetAttribLocation(shader.get(), "vertexColor");

	// Create geometry, and link in vertex and color buffers to the attribute locations
	GLuint vao = createTrianglesVAO(
		vertices, numberOfTriangles * 3 * 3, // Three vertexes pr. triangle and three coordinates pr. vertex
		indices, numberOfTriangles * 3, // Three indices pr. triangle
		colors, numberOfTriangles * 3 * 4, // Three vertexes pr. triangle and four color channels pr. vertex
		positionAttribute,
		colorAttribute);

	// Get the transform matrix location
	GLuint transformMatrixLocation = glGetUniformLocation(shader.get(), "transformMatrix");


	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		printGLError();

		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Create view and projection matrices
		glm::mat4x4 projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);

		glm::mat4x4 viewMatrix = projectionMatrix *
			glm::rotate(-cameraPitch, glm::vec3(1, 0, 0)) *
			glm::rotate(-cameraYaw, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-cameraX, -cameraY, -cameraZ));

		glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);


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

	// Camera position
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		// Move forward
		cameraX -= cameraSpeed * sin(cameraYaw) * cos(cameraPitch);
		cameraZ -= cameraSpeed * cos(cameraYaw) * cos(cameraPitch);
		cameraY += cameraSpeed * sin(cameraPitch);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		// Move backward
		cameraX += cameraSpeed * sin(cameraYaw) * cos(cameraPitch);
		cameraZ += cameraSpeed * cos(cameraYaw) * cos(cameraPitch);
		cameraY -= cameraSpeed * sin(cameraPitch);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		// Strafe right
		cameraX -= cameraSpeed * sin(cameraYaw - glm::radians(90.0f));
		cameraZ -= cameraSpeed * cos(cameraYaw - glm::radians(90.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		// Strafe left
		cameraX -= cameraSpeed * sin(cameraYaw + glm::radians(90.0f));
		cameraZ -= cameraSpeed * cos(cameraYaw + glm::radians(90.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		// Crane up
		cameraY += cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		// Crane down
		cameraY -= cameraSpeed;
	}

	// Camera rotation
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		// Tilt view up
		cameraPitch += cameraRotationSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		// Tilt view down
		cameraPitch -= cameraRotationSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		// Pan view left
		cameraYaw += cameraRotationSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		// Pan view right
		cameraYaw -= cameraRotationSpeed;
	}
}
