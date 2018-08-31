// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

GLuint createTrianglesVAO(GLfloat vertices[], int verticesLength, GLuint indices[], int indicesLength)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

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

    // Set default colour after clearing the colour buffer
    glClearColor(0.39f, 0.58f, 0.92f, 1.0f);

    // Set up your scene here (create Vertex Array Objects, etc.)

	const int numberOfTriangles = 6;


	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 2, 3
	};

	GLuint vao = createTrianglesVAO(vertices, 12, indices, 4);

	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.frag");
	shader.attach("../gloom/shaders/simple.vert");
	shader.link();

	shader.activate();

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shader.get(), "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);


    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
		fprintf(stderr, "1");
		printGLError();

        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw a triangle from the 3 vertices
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);


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
