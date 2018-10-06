// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OBJLoader.hpp"
#include "sceneGraph.hpp"
#include "toolbox.hpp"


// Shader attribute and uniform locations
GLint positionAttribute;
GLint colorAttribute;
GLuint transformMatrixLocation;


// Camera parameters
const float cameraSpeed = 0.3;
const float cameraRotationSpeed = 0.03;
float cameraX = 120;
float cameraY = 110;
float cameraZ = 160;
float cameraPitch = -0.66;
float cameraYaw = 0.52;


GLuint createVaoFromMesh(Mesh mesh)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float4), &mesh.vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint colorBufferObject;
	glGenBuffers(1, &colorBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, mesh.colours.size() * sizeof(float4), &mesh.colours[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

	return vao;
}

glm::vec3 glmVec3FromFloat3(float3 f3) { return glm::vec3(f3.x, f3.y, f3.z); }

void visitSceneNode(SceneNode* node, glm::mat4 currentTransformation)
{
	// Update the transformation
	node->currentTransformationMatrix = currentTransformation * 
		glm::translate(glmVec3FromFloat3(node->position)) *
		glm::translate(glmVec3FromFloat3(node->referencePoint)) *
		glm::rotate(node->rotation.x, glm::vec3(1, 0, 0)) *
		glm::rotate(node->rotation.y, glm::vec3(0, 1, 0)) *
		glm::rotate(node->rotation.z, glm::vec3(0, 0, 1)) *
		glm::translate(-glmVec3FromFloat3(node->referencePoint));


	// Render the node
	if (node->VAOIndexCount > 0)
	{
		glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, &node->currentTransformationMatrix[0][0]);
		glBindVertexArray(node->vertexArrayObjectID);
		glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, 0);
	}

	// Render the node's children
	for (int i = 0; i < node->children.size(); i++)
	{
		visitSceneNode(node->children[i], node->currentTransformationMatrix);
	}
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


	// Create the shader program
	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.frag");
	shader.attach("../gloom/shaders/simple.vert");
	shader.link();

	shader.activate();

	// Get attribute locations in shader
	positionAttribute = glGetAttribLocation(shader.get(), "position");
	colorAttribute = glGetAttribLocation(shader.get(), "vertexColor");

	// Get the transform matrix location
	transformMatrixLocation = glGetUniformLocation(shader.get(), "transformMatrix");


	// Setup scene geometry
	int chessboardScale = 20;

	// Load meshes
	MinecraftCharacter steve = loadMinecraftCharacterModel("steve.obj");
	Mesh chessboardMesh = generateChessboard(7, 5, chessboardScale, float4(1, 1, 1, 1), float4(0.2, 0.2, 0.2, 1));

	// Create scene graph
	SceneNode* nodeRoot = createSceneNode();

	SceneNode* nodeGround = createSceneNode();
	nodeGround->vertexArrayObjectID = createVaoFromMesh(chessboardMesh);
	nodeGround->VAOIndexCount = chessboardMesh.indices.size();

	SceneNode* nodeSteveTorso = createSceneNode();
	nodeSteveTorso->vertexArrayObjectID = createVaoFromMesh(steve.torso);
	nodeSteveTorso->VAOIndexCount = steve.torso.indices.size();
	nodeSteveTorso->referencePoint = float3(0, 0, 0);

	SceneNode* nodeSteveHead = createSceneNode();
	nodeSteveHead->vertexArrayObjectID = createVaoFromMesh(steve.head);
	nodeSteveHead->VAOIndexCount = steve.head.indices.size();
	nodeSteveHead->referencePoint = float3(0, 24, 0);

	SceneNode* nodeSteveArmL = createSceneNode();
	nodeSteveArmL->vertexArrayObjectID = createVaoFromMesh(steve.leftArm);
	nodeSteveArmL->VAOIndexCount = steve.leftArm.indices.size();
	nodeSteveArmL->referencePoint = float3(-4, 22, 0);

	SceneNode* nodeSteveArmR = createSceneNode();
	nodeSteveArmR->vertexArrayObjectID = createVaoFromMesh(steve.rightArm);
	nodeSteveArmR->VAOIndexCount = steve.rightArm.indices.size();
	nodeSteveArmR->referencePoint = float3(4, 22, 0);

	SceneNode* nodeSteveLegL = createSceneNode();
	nodeSteveLegL->vertexArrayObjectID = createVaoFromMesh(steve.leftLeg);
	nodeSteveLegL->VAOIndexCount = steve.leftLeg.indices.size();
	nodeSteveLegL->referencePoint = float3(-2, 12, 0);

	SceneNode* nodeSteveLegR = createSceneNode();
	nodeSteveLegR->vertexArrayObjectID = createVaoFromMesh(steve.rightLeg);
	nodeSteveLegR->VAOIndexCount = steve.rightLeg.indices.size();
	nodeSteveLegR->referencePoint = float3(2, 12, 0);

	addChild(nodeRoot, nodeGround);
	addChild(nodeRoot, nodeSteveTorso);
	addChild(nodeSteveTorso, nodeSteveHead);
	addChild(nodeSteveTorso, nodeSteveArmL);
	addChild(nodeSteveTorso, nodeSteveArmR);
	addChild(nodeSteveTorso, nodeSteveLegL);
	addChild(nodeSteveTorso, nodeSteveLegR);


	// Animation parameters
	const double limbSwingSpeed = 3.3;
	const double legSwingAmplitude = 0.9;
	const double armSwingAmplitude = 0.7;
	double currentTime = 0.0;
	double walkingSpeed = 20.0;

	Path walkingPath = Path("coordinates_0.txt");

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		// Update animations
		double deltaTime = getTimeDeltaSeconds();
		currentTime += deltaTime;

		nodeSteveArmR->rotation.x = armSwingAmplitude * sin(limbSwingSpeed * currentTime);
		nodeSteveArmL->rotation.x = armSwingAmplitude * -sin(limbSwingSpeed * currentTime);
		nodeSteveLegR->rotation.x = legSwingAmplitude * -sin(limbSwingSpeed * currentTime);
		nodeSteveLegL->rotation.x = legSwingAmplitude * sin(limbSwingSpeed * currentTime);

		float2 walkingDir = walkingPath.getCurrentWaypoint(chessboardScale) -
			float2(nodeSteveTorso->position.x, nodeSteveTorso->position.z);
		walkingDir /= sqrt(walkingDir.x * walkingDir.x + walkingDir.y * walkingDir.y);

		nodeSteveTorso->position += float3(walkingDir.x, 0, walkingDir.y) * walkingSpeed * deltaTime;
		// (This rotation is not smooth, and it looks a bit wonky when Steve suddenly snaps in a
		// different direction. Making him turn smoothly isn't that hard, but making him always
		// turn in the correct direction would be much simpler if using quaternions for rotation, so
		// this is left as a future optimization.)
		nodeSteveTorso->rotation.y = atan2(walkingDir.x, walkingDir.y);

		if (walkingPath.hasWaypointBeenReached(float2(nodeSteveTorso->position.x, nodeSteveTorso->position.z), chessboardScale))
		{
			walkingPath.advanceToNextWaypoint();
		}

		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create view and projection matrices
		glm::mat4x4 projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth / windowHeight, 0.1f, 1000.0f);

		glm::mat4x4 viewMatrix = projectionMatrix *
			glm::rotate(-cameraPitch, glm::vec3(1, 0, 0)) *
			glm::rotate(-cameraYaw, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-cameraX, -cameraY, -cameraZ));

		// Render the scene graph
		visitSceneNode(nodeRoot, viewMatrix);

		// Flip buffers
		glfwSwapBuffers(window);
		
		printGLError();

		// Handle other events
		glfwPollEvents();
		handleKeyboardInput(window);
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
