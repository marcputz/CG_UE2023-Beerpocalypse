#include "Utils.h"
#include <sstream>
#include <iostream>
#include <stdio.h>

#include "GameObjects/GameObject.h"
#include "GameManager.h"
#include "MyShader.h"
#include "stb/stb_image.h"
#include "MyFPSCamera.h"
#include "MyModel.h"
#include "MyTextRenderer.h"
#include "MyTransform.h"
#include "MyAssetManager.h"
#include "PxPhysicsAPI.h"
#include "GameObjects/Cube/Cube.h"
#include "INIReader.h"
#include "Lights/DirectionalLight/MyDirectionalLight.h"
#include "Lights/PointLight/MyPointLight.h"
#include "Lights/SpotLight/MySpotLight.h"
#include "MyParticleGenerator.h"

using namespace physx;
using namespace std;


/* ------------------------- */
/*        PROTOTYPES         */
/* ------------------------- */

// read ini
void readINIFile();

// openGL
void static initOpenGL();
static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void error_callback(int errorCode, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void handleContinuousKeyboardInput(GLFWwindow* window);

// PhysX
void static stepPhysics(float deltaTime);
void static initPhysX();
void static destroyPhysX();

// Game-Logic and Rendering
void static renderHUD(MyTextRenderer textRenderer, MyShader textShader);
void static setUniformsOfLights(MyShader& shader);

/* ------------------------- */
/*     GLOBAL VARIABLES      */
/* ------------------------- */

// Default values if settings are not found
const unsigned int SCR_WIDTH_DEFAULT = 1280;
const unsigned int SCR_HEIGHT_DEFAULT = 720;
const unsigned int REFRESH_RATE_DEFAULT = 60;
const string WINDOW_TITLE_DEFAULT = "Beerpocalypse (CG SS2023)";
const bool START_FULLSCREEN_DEFAULT = false;

const float CAMERA_STARTING_FOV_DEFAULT = 60.0f;
const float CAMERA_NEAR_DEFAULT = 0.1f;
const float CAMERA_FAR_DEFAULT = 100.0f;

// Window-Attributes
GLFWwindow* window;
unsigned int screenWidth = 0;
unsigned int screenHeight = 0;
unsigned int refreshRate = 0;
string windowTitle = "";
bool startFullscreen = false;

// Toggle-Flags
bool enableWireframe = false;
bool enableBackfaceCulling = true;
bool enableHUD = true;
bool enableNormalMapping = false;
//bool enableFlashLight = true;

// Camera
MyFPSCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float cameraStartingFOV = 0.0f;
float cameraNear = 0.0f;
float cameraFar = 0.0f;
bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;

// Shaders
MyShader defaultShader;

// Game Logic
GameManager* gameManager;
float deltaTime = 0.0f;
float framesPerSecond = 0.0f;

// Light Positions
glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};

// PhysX
PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = nullptr;
PxPhysics* gPhysics = nullptr;
PxDefaultCpuDispatcher* gDispatcher = nullptr;
PxScene* physicsScene = nullptr;
PxMaterial* gMaterial = nullptr;
PxPvd* gPvd = nullptr;

/* ------------------------- */
/*           MAIN            */
/* ------------------------- */

int main(int argc, char** argv) {
	// tell stbi to load image y-flipped so its right for opengl texCoords
	stbi_set_flip_vertically_on_load(true);

	// Read values from settings.ini
	readINIFile();

	// Initialize OpenGL
	initOpenGL();
	std::cout << "OpenGL initialized" << std::endl;

	// Initialize PhysX Physics Engine
	initPhysX();
	std::cout << "PhysX initialized" << std::endl;

	// Prepare Text Renderer and Shader
	MyTextRenderer textRenderer("arial/arial.ttf");
	MyShader textShader = MyAssetManager::loadShader("text.vert", "text.frag", "textShader");
	{
		// set the "camera" to be used for text rendering (static fixed orthogonal view-projection)
		glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));
		textShader.use();
		textShader.setMat4("projection", textProjection);
	}


	// Load default shader for objects
	MyShader defaultShader = MyAssetManager::loadShader("blinn-phong.vert", "blinn-phong.frag", "blinnPhongShader");

	// Setup Game Objects
	gameManager = new GameManager(gPhysics, camera);

	// Init Player
	GameObjectInfo playerInfo;
	playerInfo.modelPath = "cube/brick_cube/cube.obj";
	playerInfo.location = PxVec3(0, 2, 5);
	playerInfo.staticFriction = playerInfo.dynamicFriction = 0.5;
	playerInfo.restitution = 0.05;
	playerInfo.actorType = TYPE_DYNAMIC;
	Player player(defaultShader, gPhysics, playerInfo, camera);

	gameManager->setPlayer(&player);

	camera.attachToSubject(&player);

	// Init Objects
	GameObjectInfo brickCubeInfo;
	brickCubeInfo.modelPath = "cube/brick_cube/cube.obj";
	brickCubeInfo.location = PxVec3(-2.0, 10, -2);
	brickCubeInfo.actorType = TYPE_DYNAMIC;
	brickCubeInfo.staticFriction = 0.5;
	brickCubeInfo.dynamicFriction = 0.5;
	brickCubeInfo.restitution = 0.5;
	Cube brickCube1(defaultShader, gPhysics, brickCubeInfo);
	brickCubeInfo.location = PxVec3(0, 2, 0);
	brickCubeInfo.actorType = TYPE_STATIC;
	Cube brickCube2(defaultShader, gPhysics, brickCubeInfo);

	GameObjectInfo cubeInfo;
	cubeInfo.staticFriction = 0.5;
	cubeInfo.dynamicFriction = 0.5;
	cubeInfo.restitution = 0.5;
	cubeInfo.modelPath = "cube/metal_cube/cube.obj";
	cubeInfo.location = PxVec3(-3, 3, -1.5);
	cubeInfo.actorType = TYPE_STATIC;
	Cube metalCube(defaultShader, gPhysics, cubeInfo);

	cubeInfo.modelPath = "cube/paving_cube/cube.obj";
	cubeInfo.location = PxVec3(0, -3, 0);
	cubeInfo.scale = PxVec3(4, 1, 6.5);
	Cube pavingCube(defaultShader, gPhysics, cubeInfo);

	// Add to game
	gameManager->addObject(&brickCube1);
	gameManager->addObject(&brickCube2);
	gameManager->addObject(&metalCube); 
	gameManager->addObject(&pavingCube);

	// Init lights
	MyDirectionalLight dirLight(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f),
		true, glm::vec3(-0.2f, -1.0f, 0.3f));
	dirLight.addLightToShader(defaultShader);

	MyPointLight pointLightOne(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, pointLightPositions[0],
		1.0f, 0.09f, 0.032f);
	pointLightOne.addLightToShader(defaultShader);

	MyPointLight pointLightTwo(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, pointLightPositions[1],
		1.0f, 0.09f, 0.032f);
	pointLightTwo.addLightToShader(defaultShader);

	MyPointLight pointLightThree(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, pointLightPositions[2],
		1.0f, 0.09f, 0.032f);
	pointLightThree.addLightToShader(defaultShader);

	MyPointLight pointLightFour(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, pointLightPositions[3],
		1.0f, 0.09f, 0.032f);
	pointLightFour.addLightToShader(defaultShader);

	MySpotLight flashLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, &camera);
	flashLight.addLightToShader(defaultShader);

	MySpotLight spotLightOne(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(-0.3f, 0.0f, -1.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, nullptr);
	spotLightOne.addLightToShader(defaultShader);

	// Add lights to the game
	gameManager->addLight(&dirLight);
	gameManager->addLight(&pointLightOne);
	gameManager->addLight(&pointLightTwo);
	gameManager->addLight(&pointLightThree);
	gameManager->addLight(&pointLightFour);
	gameManager->setPlayerFlashLight(&flashLight);
	gameManager->addLight(&spotLightOne);

	MyShader particleShader = MyAssetManager::loadShader("particle.vert", "particle.frag", "particle");
	My2DTexture particleTexture = MyAssetManager::loadTexture("assets/textures/particle.png", SPRITE, true, "particle");
	MyParticleGenerator particleGen(particleShader, particleTexture, camera, 500);



	// Setup lights shader
	MyShader myLightShader = MyAssetManager::loadShader("simpleLightSource.vert", "simpleLightSource.frag", "lightShader");

	// Prepare Light Cubes 
	unsigned int VBO, lightVAO;
	{
		// cube vertices with normals and texture coords
		float vertices[] = {
			// position			  // normal            // texture coords	
			// backface
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  // Left Bottom Back
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,	// Right Top Back
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,	// Right Bottom Back
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,	// Right Top Back
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,	// Left Bottom Back
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,	// Left Top Back
			// frontface
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,	// Left Bottom Front
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,	// Right Bottom Front
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,	// Right Top Front
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,	// Right Top Front
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,	// Left Top Front
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,	// Left Bottom Front
			// leftface
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// Left Top Front
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,	// Left Top Back
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// Left Bottom Back
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// Left Bottom Back
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,	// Left Bottom Front
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// Left Top Front
			// rightface
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// Right Top Front
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// Right Bottom Back
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,	// Right Top Back
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// Right Bottom Back
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// Right Top Front
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,	// Right Bottom Front
			 // bottomface
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,	// Left Bottom Back
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,	// Right Bottom Back
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,	// Right Bottom Front
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,	// Right Bottom Front
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,	// Left Bottom Front
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,	// Left Bottom Back
			// topface
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,	// Left Top Back
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,	// Right Top Front
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,	// Right Top Back
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,	// Right Top Front
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,	// left Top Back
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f 	// Left Top Front
		};

		// cube-light vao and vbo
		glGenVertexArrays(1, &lightVAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	// vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	

	/*---- RENDER LOOP -----*/
	float time = float(glfwGetTime());
	float lastTime = time;

	std::cout << "Render Loop START" << std::endl;
	while (!glfwWindowShouldClose(window)) {
		// Clear backbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Poll events
		glfwPollEvents();

		// Compute frame time
		time = float(glfwGetTime());
		deltaTime = time - lastTime;
		lastTime = time;
		framesPerSecond = 1.0f / deltaTime;

		// Set Shader Attributes
		{
			defaultShader.use();
			defaultShader.setVec3("viewPos", camera.position_);
			//defaultShader.setBool("enableSpotLight", enableFlashLight);
			defaultShader.setBool("enableNormalMapping", enableNormalMapping);
		}

		// Prepare Camera (view-projection matrix)
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov_), float(screenWidth) / float(screenHeight), cameraNear, cameraFar);
		glm::mat4 view = camera.getViewMatrix();
		defaultShader.setMat4("projection", projection);
		defaultShader.setMat4("view", view);

		// Update the game
		gameManager->handleKeyboardInput(window, deltaTime);
		gameManager->stepUpdate(deltaTime);
		gameManager->draw();

		particleShader.use();
		particleShader.setMat4("projection", projection);
		particleShader.setMat4("view", view);
		particleGen.update(deltaTime);
		particleGen.draw();


		// Render Light-Cubes
		{
			glBindVertexArray(lightVAO);

			myLightShader.use();
			myLightShader.setMat4("projection", projection);
			myLightShader.setMat4("view", view);

			glm::mat4 model = glm::mat4(1.0f);
			for (unsigned int i = 0; i < 4; i++) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f));
				myLightShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		renderHUD(textRenderer, textShader);

		// Swap buffers
		glfwSwapBuffers(window);
	}


	
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// Breakdown Bullet Physics Engine
	destroyPhysX();

	// Breakdown OpenGL
	destroyFramework();
	glfwTerminate();

	// Close Application with no error
	return 0;
}

/* ------------------------- */
/*        FUNCTIONS          */
/* ------------------------- */

void static renderHUD(MyTextRenderer textRenderer, MyShader textShader) {
	textRenderer.renderText(textShader, "FPS: " + std::to_string((int)framesPerSecond) + ", FOV: " + std::to_string((int)camera.fov_), 0.0f, (float)screenHeight - 12.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "F1 Wireframe: " + std::string(enableWireframe ? "on" : "off"), 0.0f, (float)screenHeight - 24.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "F2 Backface-culling: " + std::string(enableBackfaceCulling ? "on" : "off"), 0.0f, (float)screenHeight - 36.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "F3 HUD (not implemented): " + std::string(enableHUD ? "on" : "off"), 0.0f, (float)screenHeight - 48.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "F4 Normal mapping: " + std::string(enableNormalMapping ? "on" : "off"), 0.0f, (float)screenHeight - 60.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "WASD - Movement", 0.0f, (float)screenHeight - 72.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "F - Flashlight", 0.0f, (float)screenHeight - 84.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "Mouse - Look around", 0.0f, (float)screenHeight - 96.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
	textRenderer.renderText(textShader, "ESC - Exit Application", 0.0f, (float)screenHeight - 108.0f, 0.25f, glm::vec3(0.9f, 0.9f, 0.9f), enableWireframe);
}

void static setUniformsOfLights(MyShader &shader) {
	// directional light
	shader.setVec3("dirLight.direction", -0.2f, -1.0f, 0.3f);
	shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	shader.setBool("dirLight.enabled", true);

	// 4 point lights
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setBool("pointLights[0].enabled", true);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);

	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setBool("pointLights[1].enabled", true);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);

	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setBool("pointLights[2].enabled", true);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09f);
	shader.setFloat("pointLights[2].quadratic", 0.032f);

	shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	shader.setBool("pointLights[3].enabled", true);
	shader.setFloat("pointLights[3].constant", 1.0f);
	shader.setFloat("pointLights[3].linear", 0.09f);
	shader.setFloat("pointLights[3].quadratic", 0.032f);

	// spotlight
	shader.setVec3("spotLights[0].position", camera.position_);
	shader.setVec3("spotLights[0].direction", camera.front_);
	shader.setVec3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setBool("spotLights[0].enabled", true);
	shader.setFloat("spotLights[0].constant", 1.0f);
	shader.setFloat("spotLights[0].linear", 0.09f);
	shader.setFloat("spotLights[0].quadratic", 0.032f);
	shader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// F1 - Wireframe
	// F2 - Culling
	// F3 - Toggle HUD
	// F4 - Toggle normal mapping
	// F8 - Toggle view-frustum culling
	// Esc - Exit
	// E - Interact with Object
	// R - Reload
	// F - Toggle flashlight
	// +/- - increase/decrease illumination

	if (action != GLFW_PRESS) {
		return;
	}
	
	switch (key) {
		case GLFW_KEY_F1:
			enableWireframe = !enableWireframe;
			glPolygonMode(GL_FRONT_AND_BACK, enableWireframe ? GL_LINE : GL_FILL);
			break;
		case GLFW_KEY_F2:
			enableBackfaceCulling = !enableBackfaceCulling;
			if (enableBackfaceCulling) {
				glEnable(GL_CULL_FACE);
			} else {
				glDisable(GL_CULL_FACE);
			}
			break;
		case GLFW_KEY_F3:
			// toggle hud
			enableHUD = !enableHUD;
			break;
		case GLFW_KEY_F4:
			// toggle normal mapping
			enableNormalMapping = !enableNormalMapping;
			break;
		case GLFW_KEY_F8:
			// toggle view-frustum culling
			std::cout << "Toggle view-frustum culling" << std::endl;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_E:
			// interact
			std::cout << "Press E" << std::endl;
			break;
		case GLFW_KEY_R:
			// reload
			std::cout << "Press R" << std::endl;
			break;
		case GLFW_KEY_F:
			//enableFlashLight = !enableFlashLight;
			gameManager->toggleFlashlight();
			break;
		case GLFW_KEY_KP_ADD:
			// increase illumination
			std::cout << "Press +" << std::endl;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			// decrease illumination
			std::cout << "Press -" << std::endl;
			break;
		case GLFW_KEY_T:
			camera.detach();
			break;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// nothing for now
	/*
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
	*/

	//std::cout << "Mouse button event" << std::endl;
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	float x = static_cast<float>(xpos);
	float y = static_cast<float>(ypos);

	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xOffset = x - lastX;
	float yOffset = lastY - y;
	lastX = x;
	lastY = y;

	gameManager->handleMouseInput(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.handleMouseScrolling(yoffset);

	//std::cout << "Mouse scroll event" << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	//std::cout << "Viewport event" << std::endl;
}

void error_callback(int errorCode, const char* description) {
	std::cout << "[GLFW Error " << errorCode << "]: " << description << std::endl;
}

void readINIFile() {
	INIReader iniReader = INIReader::INIReader("settings.ini");

	screenWidth = iniReader.GetInteger("window", "width", SCR_WIDTH_DEFAULT);
	screenHeight = iniReader.GetInteger("window", "height", SCR_HEIGHT_DEFAULT);
	refreshRate = iniReader.GetInteger("window", "refresh_rate", REFRESH_RATE_DEFAULT);
	windowTitle = iniReader.Get("window", "title", WINDOW_TITLE_DEFAULT);
	startFullscreen = iniReader.GetBoolean("window", "fullscreen", START_FULLSCREEN_DEFAULT);

	cameraStartingFOV = iniReader.GetReal("camera", "fov", CAMERA_STARTING_FOV_DEFAULT);
	cameraNear = iniReader.GetReal("camera", "near", CAMERA_NEAR_DEFAULT);
	cameraFar = iniReader.GetReal("camera", "far", CAMERA_FAR_DEFAULT);

	camera.fov_ = cameraStartingFOV;
	lastX = screenWidth / 2.0f;
	lastY = screenHeight / 2.0f;
}

void static initOpenGL() {
	
	/* --------------------------------------------- */
	// Init GLFW
	/* --------------------------------------------- */

	//glfwSetErrorCallback([](int error, const char* description) { std::cout << "[GLFW Error " << error << "]: " << description << std::endl; });
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		glfwTerminate();
		std::cout << "[GLFW Error]: Failed to init GLFW" << std::endl;
		exit(101);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, refreshRate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	GLFWmonitor* monitor = nullptr;

	if (startFullscreen) {
		monitor = glfwGetPrimaryMonitor();

		int xP, yP, w, h;
		glfwGetMonitorWorkarea(monitor, &xP, &yP, &w, &h);
		window = glfwCreateWindow(w, h, windowTitle.c_str(), monitor, nullptr);
	} else {
		window = glfwCreateWindow(screenWidth, screenHeight, windowTitle.c_str(), monitor, nullptr);
	}



	if (!window) {
		glfwTerminate();
		std::cout << "[GLFW Error]: Failed to create window" << std::endl;
		exit(102);
	}

	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	/* --------------------------------------------- */
	// Init GLEW
	/* --------------------------------------------- */

	glewExperimental = true;
	GLenum err = glewInit();

	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		glfwTerminate();
		std::cout << "[GLEW Error]: Failed to init GLEW" << glewGetErrorString(err) << std::endl;
		exit(103);
	}

	// Debug callback
	if (glDebugMessageCallback != NULL) {
		// Register your callback function.

		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	if (!initFramework()) {
		glfwTerminate();
		std::cout << "[Framework Error]: Failed to init framework" << std::endl;
		exit(104);
	}

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// capture mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set standard values for OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void static initPhysX() {
	// Set-up PhysX foundation
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	if (!gFoundation) {
		std::cout << "[PhysX Error]: Failed to init PhysX foundation" << std::endl;
		exit(301);
	}

	// Set-up PhysX Visual Debugger (optional)
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	// Create PhysX engine
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	if (!gPhysics) {
		std::cout << "[PhysX Error]: Failed to init PhysX engine" << std::endl;
		exit(302);
	}
}

void static destroyPhysX() {
	//PX_UNUSED(interactive);
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) { // ignore performance warnings from nvidia
		return;
	}
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
		case GL_DEBUG_CATEGORY_API_ERROR_AMD:
		case GL_DEBUG_SOURCE_API: {
			sourceString = "API";
			break;
		}
		case GL_DEBUG_CATEGORY_APPLICATION_AMD:
		case GL_DEBUG_SOURCE_APPLICATION: {
			sourceString = "Application";
			break;
		}
		case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
			sourceString = "Window System";
			break;
		}
		case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {
			sourceString = "Shader Compiler";
			break;
		}
		case GL_DEBUG_SOURCE_THIRD_PARTY: {
			sourceString = "Third Party";
			break;
		}
		case GL_DEBUG_CATEGORY_OTHER_AMD:
		case GL_DEBUG_SOURCE_OTHER: {
			sourceString = "Other";
			break;
		}
		default: {
			sourceString = "Unknown";
			break;
		}
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: {
			typeString = "Error";
			break;
		}
		case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			typeString = "Deprecated Behavior";
			break;
		}
		case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			typeString = "Undefined Behavior";
			break;
		}
		case GL_DEBUG_TYPE_PORTABILITY_ARB: {
			typeString = "Portability";
			break;
		}
		case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
		case GL_DEBUG_TYPE_PERFORMANCE: {
			typeString = "Performance";
			break;
		}
		case GL_DEBUG_CATEGORY_OTHER_AMD:
		case GL_DEBUG_TYPE_OTHER: {
			typeString = "Other";
			break;
		}
		default: {
			typeString = "Unknown";
			break;
		}
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: {
			severityString = "High";
			break;
		}
		case GL_DEBUG_SEVERITY_MEDIUM: {
			severityString = "Medium";
			break;
		}
		case GL_DEBUG_SEVERITY_LOW: {
			severityString = "Low";
			break;
		}
		default: {
			severityString = "Unknown";
			break;
		}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}