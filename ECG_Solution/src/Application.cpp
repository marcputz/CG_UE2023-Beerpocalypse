#include "Utils.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>

#include "MyShader.h"
#include "stb/stb_image.h"
#include "MyModel.h"
#include "MyTextRenderer.h"
#include "MyGUIRenderer.h"
#include "MyAssetManager.h"
#include "PxPhysicsAPI.h"
#include "INIReader.h"
#include "Lights/DirectionalLight/MyDirectionalLight.h"
#include "Lights/PointLight/MyPointLight.h"
#include "Lights/SpotLight/MySpotLight.h"
#include "MyParticleGenerator.h"
#include "MyAnimator.h"
#include "GameObjects/Vampire/Vampire.h"
#include "Scene.h"
#include "GameObjects/Cube/StaticCube.h"
#include "GameObjects/Cube/DynamicCube.h"
#include "GameObjects/Player/Player.h"
#include "GameObjects/Beer/Beer.h"
#include "GameObjects/Ground/Ground.h"
#include "GameObjects/Zombie/Zombie.h"
#include "GameObjects/Button/Button.h"
#include "GameObjects/Roof/Roof.h"

using namespace physx;
using std::cout;
using std::endl;

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
void renderBloomQuad();
void setupBloomBuffers();
void adjustBloomBuffers();
void createTextProjection();
void initLevel(MyParticleGenerator& particleGenerator);

// PhysX
void static initPhysX();
void static destroyPhysX();

// irrKlang
// load all necessary sounds in the asset manager
void static initIrrKlang();

// Game-Logic and Rendering
void static renderHUD(MyTextRenderer textRenderer, MyShader textShader);

/* ------------------------- */
/*     GLOBAL VARIABLES      */
/* ------------------------- */

// Default values if settings are not found
const unsigned int SCR_WIDTH_DEFAULT = 1280;
const unsigned int SCR_HEIGHT_DEFAULT = 720;
const unsigned int REFRESH_RATE_DEFAULT = 60;
const string WINDOW_TITLE_DEFAULT = "Beerpocalypse (CG SS2023)";
const bool START_FULLSCREEN_DEFAULT = false;

const float CAMERA_FOV_DEFAULT = 60.0f;
const float CAMERA_NEAR_DEFAULT = 0.1f;
const float CAMERA_FAR_DEFAULT = 100.0f;

const float AUDIO_VOLUME_DEFAULT = 0.5f;

// Window-Attributes
GLFWwindow* window;
unsigned int screenWidth = 0;
unsigned int screenHeight = 0;
unsigned int refreshRate = 0;
string windowTitle = "";
bool startFullscreen = false;

// Camera
float cameraFov = 0.0f;
float cameraNear = 0.0f;
float cameraFar = 0.0f;

// audio
float audioVolume = 0.0f;

// bloom
unsigned int hdrFrameBuffer;
unsigned int colorBuffers[2];

unsigned int renderBufferDepth;
unsigned int attachments[2];

unsigned int pingPongFrameBuffer[2];
unsigned int pingPongColorBuffers[2];

unsigned int bloomQuadVAO = 0;
unsigned int bloomQuadVBO;

// Shaders
MyShader bloomBlurShader;
MyShader bloomCombineShader;
MyShader textShader;
MyShader guiShader;
MyShader defaultShader;
MyShader animationShader;
MyShader particleShader;
MyShader lightSourceShader;

// text renderer projection matrix
glm::mat4 textProjection;

// Game Logic
Scene* scene;
Player* player = nullptr;
MySpotLight* playerFlashLight = nullptr;

glm::vec3 pointLightOnePosition;
glm::vec3 pointLightTwoPosition;
glm::vec3 pointLightThreePosition;
glm::vec3 pointLightFourPosition;

const int maxScore = 5;
int score = 0;

const int maxBullets = 4;
int bullets = maxBullets;

bool isPaused = false;
bool isGameLost = false;
bool isGameWon = false;
bool enableWireframe = false;
bool enableBackfaceCulling = true;
#if _DEBUG
bool enableDebugHUD = true;
bool freezeZombies = true;
#else
bool enableDebugHUD = false;
bool freezeZombies = false;
#endif
bool enableNormalMapping = true;
bool enableShowNormals = false;
bool enableGUIHUD = true;
bool enableBloom = true;
float gamma = 1.0f;
float exposure = 1.0f;

// Frame Processing
float deltaTime = 0.0f;
float framesPerSecond = 0.0f;

// PhysX
PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = nullptr;
PxPhysics* gPhysics = nullptr;
PxPvd* gPvd = nullptr;

std::vector<GameObject*> roomOne;
std::vector<GameObject*> roomTwo;
std::vector<GameObject*> roomThree;
std::vector<GameObject*> roomFour;
std::vector<GameObject*> roomFive;
std::vector<GameObject*> corridors;
std::vector<int> beerIdx;

/* ------------------------- */
/*           MAIN            */
/* ------------------------- */

int main(int argc, char** argv) {
	// tell stbi to load image y-flipped so its right for opengl texCoords
	// disregard comment on top, we need to load textures non-flipped
	stbi_set_flip_vertically_on_load(false);

	// Read values from settings.ini
	readINIFile();

	// Initialize OpenGL
	initOpenGL();
	std::cout << "OpenGL initialized" << std::endl;

	// Initialize PhysX Physics Engine
	initPhysX();
	std::cout << "PhysX initialized" << std::endl;

	// Initialize sounds that will be used
	initIrrKlang();

	// setup for bloom
	setupBloomBuffers();

	bloomBlurShader = MyAssetManager::loadShader("bloomBlur.vert", "bloomBlur.frag", "bloomBlur");
	bloomBlurShader.use();
	bloomBlurShader.setInt("imageToBlur", 0);

	bloomCombineShader = MyAssetManager::loadShader("bloomCombine.vert", "bloomCombine.frag", "bloomCombine");
	bloomCombineShader.use();
	bloomCombineShader.setInt("scene", 0);
	bloomCombineShader.setInt("bloomBlur", 1);

	// Load default shader for objects
	defaultShader = MyAssetManager::loadShader("blinn-phong.vert", "blinn-phong.frag", "blinnPhongShader");

	// Load shader for vertex skinned models
	animationShader = MyAssetManager::loadShader("vertex-skinning.vert", "vertex-skinning.frag", "skinning");

	// Prepare Text Renderer and Shader
	MyTextRenderer textRenderer("arial/arial.ttf");
	textShader = MyAssetManager::loadShader("text.vert", "text.frag", "textShader");
	// set the "camera" to be used for text rendering (static fixed orthogonal view-projection)
	createTextProjection();

	// Prepare GUI Renderer and shader
	MyGUIRenderer guiRenderer;
	guiShader = MyAssetManager::loadShader("gui.vert", "gui.frag", "guiShader");
	guiShader.use();
	guiShader.setMat4("projection", textProjection);

	// Setup Scene
	scene = new Scene(gPhysics);

	// Init Player
	player = new Player{ &defaultShader, gPhysics };
	player->setLocalPosition(glm::vec3(0.0f, 0.5f, -16.0f));
	player->setRespawnPoint(glm::vec3(0.0f, 0.5f, -16.0f));
	player->setScale(glm::vec3(0.5, 1, 0.5), true);
	player->setCameraFOVs(cameraFov);
	scene->addObject(player);

	// Init particle generator
	particleShader = MyAssetManager::loadShader("particle.vert", "particle.frag", "particle");
	MyParticleGenerator particleGen(particleShader/*, particleTexture*/, player, 50);
	scene->setParticleGenerator(&particleGen);

	initLevel(particleGen);

	for (int i = 0; i < roomOne.size(); i++) {
		scene->addObject(roomOne[i]);
	}

	for (int i = 0; i < roomTwo.size(); i++) {
		scene->addObject(roomTwo[i]);
	}

	for (int i = 0; i < roomThree.size(); i++) {
		scene->addObject(roomThree[i]);
	}

	for (int i = 0; i < roomFour.size(); i++) {
		scene->addObject(roomFour[i]);
	}

	for (int i = 0; i < roomFive.size(); i++) {
		scene->addObject(roomFive[i]);
	}

	for (int i = 0; i < corridors.size(); i++) {
		scene->addObject(corridors[i]);
	}

	// Init Zombies
	Zombie zombieOne{ &animationShader, gPhysics };
	scene->addObject(&zombieOne, true);
	zombieOne.setLocalPosition(glm::vec3(2, 1.5, 2));

	MyAnimation zombieIdleAnim("assets/models/zombie/animations/ZombieIdle.dae", zombieOne.getModel());
	MyAnimation zombieAttackAnim("assets/models/zombie/animations/ZombieAttack.dae", zombieOne.getModel());
	MyAnimation zombieWalkAnim("assets/models/zombie/animations/ZombieWalk.dae", zombieOne.getModel());

	MyAnimator zombieOneAnimator{};
	zombieOneAnimator.addAnimation(Animation_Enum::IDLE, &zombieIdleAnim);
	zombieOneAnimator.addAnimation(Animation_Enum::WALKING, &zombieWalkAnim);
	zombieOneAnimator.addAnimation(Animation_Enum::ATTACKING, &zombieAttackAnim);
	zombieOne.setAnimator(zombieOneAnimator);

	Zombie zombieTwo{ &animationShader, gPhysics };
	scene->addObject(&zombieTwo, true);
	zombieTwo.setLocalPosition(glm::vec3(5, 0.5, 8));

	MyAnimator zombieTwoAnimator{};
	zombieTwoAnimator.addAnimation(Animation_Enum::IDLE, &zombieIdleAnim);
	zombieTwoAnimator.addAnimation(Animation_Enum::WALKING, &zombieWalkAnim);
	zombieTwoAnimator.addAnimation(Animation_Enum::ATTACKING, &zombieAttackAnim);
	//zombieTwoAnimator.setAnimationSpeedMultiplier(2.0f);
	zombieTwo.setAnimator(zombieTwoAnimator);



	/*
	Vampire vampire(&animationShader, gPhysics);
	vampire.setLocalPosition(glm::vec3(0, -1.5, 0));
	scene->addObject(&vampire, false);
	*/

	// Init lights
	MyDirectionalLight dirLight(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.3f, 0.3f, 0.3f),
		true, glm::vec3(-0.2f, -1.0f, 0.3f));
	dirLight.addLightToShader(defaultShader);
	dirLight.addLightToShader(animationShader);

	MyPointLight pointLightOne(glm::vec3(0.00f, 0.00f, 0.00f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, pointLightOnePosition,
		1.0f, 0.09f, 0.032f);
	pointLightOne.addLightToShader(defaultShader);
	pointLightOne.addLightToShader(animationShader);

	MyPointLight pointLightTwo(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		false, glm::vec3(2.3f, -3.3f, -4.0f),
		1.0f, 0.09f, 0.032f);
	pointLightTwo.addLightToShader(defaultShader);
	pointLightTwo.addLightToShader(animationShader);

	MyPointLight pointLightThree(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		false, glm::vec3(-4.0f, 2.0f, -12.0f),
		1.0f, 0.09f, 0.032f);
	pointLightThree.addLightToShader(defaultShader);
	pointLightThree.addLightToShader(animationShader);

	MyPointLight pointLightFour(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
		false, glm::vec3(0.0f, 0.0f, -3.0f),
		1.0f, 0.09f, 0.032f);
	pointLightFour.addLightToShader(defaultShader);
	pointLightFour.addLightToShader(animationShader);

	playerFlashLight = new MySpotLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		17.5f, 20.0f, player);
	playerFlashLight->addLightToShader(defaultShader);
	playerFlashLight->addLightToShader(animationShader);

	MySpotLight spotLightOne(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true,
		roomOne[beerIdx[0]]->getWorldPosition() + glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, nullptr, roomOne[beerIdx[0]]);
	spotLightOne.addLightToShader(defaultShader);
	spotLightOne.addLightToShader(animationShader);

	MySpotLight spotLightTwo(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true,
		roomTwo[beerIdx[1]]->getWorldPosition() + glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, nullptr, roomTwo[beerIdx[1]]);
	spotLightTwo.addLightToShader(defaultShader);
	spotLightTwo.addLightToShader(animationShader);

	MySpotLight spotLightThree(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true,
		roomThree[beerIdx[2]]->getWorldPosition() + glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, nullptr, roomThree[beerIdx[2]]);
	spotLightThree.addLightToShader(defaultShader);
	spotLightThree.addLightToShader(animationShader);

	MySpotLight spotLightFour(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true,
		roomFour[beerIdx[3]]->getWorldPosition() + glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, nullptr, roomFour[beerIdx[3]]);
	spotLightFour.addLightToShader(defaultShader);
	spotLightFour.addLightToShader(animationShader);

	MySpotLight spotLightFive(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		true,
		roomFive[beerIdx[4]]->getWorldPosition() + glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		1.0f, 0.09f, 0.032f,
		12.5f, 15.0f, nullptr, roomFive[beerIdx[4]]);
	spotLightFive.addLightToShader(defaultShader);
	spotLightFive.addLightToShader(animationShader);


	// Add lights to the game
	// dir
	scene->addLight(&dirLight);
	// point
	scene->addLight(&pointLightOne);
	scene->addLight(&pointLightTwo);
	scene->addLight(&pointLightThree);
	scene->addLight(&pointLightFour);
	// spot
	scene->addLight(playerFlashLight);
	scene->addLight(&spotLightOne);
	scene->addLight(&spotLightTwo);
	scene->addLight(&spotLightThree);
	scene->addLight(&spotLightFour);
	scene->addLight(&spotLightFive);

	// Setup lights shader
	lightSourceShader = MyAssetManager::loadShader("simpleLightSource.vert", "simpleLightSource.frag", "lightShader");

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
		// 1. render scene into floating point frameBuffer for bloom
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer);

		// Clear backbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Poll events
		glfwPollEvents();

		// Compute frame time
		time = float(glfwGetTime());
		deltaTime = time - lastTime;
		lastTime = time;
		framesPerSecond = 1.0f / deltaTime;

		// Prepare Camera (view-projection matrix)
		glm::mat4 projection = player->getActiveCamera()->getProjectionMatrix();
		glm::mat4 view = player->getActiveCamera()->getViewMatrix();

		// Set Shader Attributes
		defaultShader.use();
		defaultShader.setVec3("viewPos", player->getActiveCamera()->getPosition());
		defaultShader.setBool("enableNormalMapping", enableNormalMapping);
		defaultShader.setBool("enableShowNormals", enableShowNormals);
		defaultShader.setMat4("projection", projection);
		defaultShader.setMat4("view", view);

		animationShader.use();
		animationShader.setVec3("viewPos", player->getActiveCamera()->getPosition());
		animationShader.setBool("enableNormalMapping", enableNormalMapping);
		animationShader.setBool("enableShowNormals", enableShowNormals);
		animationShader.setMat4("projection", projection);
		animationShader.setMat4("view", view);

		particleShader.use();
		particleShader.setMat4("projection", projection);
		particleShader.setMat4("view", view);

		// Update the game
		if (isPaused == false && isGameLost == false && isGameWon == false) {
			scene->handleKeyboardInput(window, deltaTime);
			scene->step(deltaTime);
		}

		scene->draw();

		// Fail Condition
		if (player->getHealth() <= 0) {
			isGameLost = true;
			isPaused = true;
			//exit(100);
		}

		// win condition
		if (score == maxScore) {
			isGameWon = true;
			isPaused = true;
		}

		// Render Light-Cubes
		//glBindVertexArray(lightVAO);

		/*lightSourceShader.use();
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		for (unsigned int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			myLightSourceShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}*/

		renderHUD(textRenderer, textShader);

		if (enableGUIHUD) {
			guiRenderer.renderGUI(guiShader, score, maxScore, bullets, maxBullets, player->getHealth(), player->maxHealth);
		}

		
		// 2. blur bright fragments with two-pass Gaussian Blur
		bool horizontal = true, firstIteration = true;
		unsigned int amount = 10;
		bloomBlurShader.use();
		for (unsigned int i = 0; i < amount; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingPongFrameBuffer[horizontal]);
			bloomBlurShader.setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingPongFrameBuffer[horizontal]);
			renderBloomQuad();
			horizontal = !horizontal;
			if (firstIteration) {
				firstIteration = false;
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 3. render floating point color buffer to 2D quad and tonemap HDR colors to the default frameBuffer's color range
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bloomCombineShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingPongColorBuffers[!horizontal]);
		bloomCombineShader.setInt("bloom", enableBloom);
		bloomCombineShader.setFloat("exposure", exposure);
		bloomCombineShader.setFloat("gamma", gamma);
		renderBloomQuad();
		

		// Swap buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// Breakdown shaders, textures and irrklang loaded via the MyAssetManager
	MyAssetManager::cleanUp();

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

void initLevel(MyParticleGenerator& particleGenerator) {
	// first room
	// floor
	Ground* groundRoomOne = new Ground{ &defaultShader, gPhysics };
	groundRoomOne->setLocalPosition(glm::vec3(0.0f, -0.5f, 0.0f));
	groundRoomOne->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);
	// right wall
	StaticCube* rightWallRoomOne = new StaticCube{ &defaultShader, gPhysics };
	rightWallRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(-10.0f, 2.5f, 0.0f));
	rightWallRoomOne->setScale(glm::vec3(1.0f, 2.0f, 10.0f), true);
	// left wall
	StaticCube* leftWallRoomOne = new StaticCube{ &defaultShader, gPhysics };
	leftWallRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(10.0f, 2.5f, 0.0f));
	leftWallRoomOne->setScale(glm::vec3(1.0f, 2.0f, 10.0f), true);
	// behind wall
	StaticCube* behindWallRightExitRoomOne = new StaticCube{ &defaultShader, gPhysics };
	behindWallRightExitRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(-6.0f, 2.5f, -10.0f));
	behindWallRightExitRoomOne->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// behind wall - left part of exit
	StaticCube* behindWallLeftExitRoomOne = new StaticCube{ &defaultShader, gPhysics };
	behindWallLeftExitRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(6.0f, 2.5f, -10.0f));
	behindWallLeftExitRoomOne->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// front wall - right part of exit
	StaticCube* frontWallRightExitRoomOne = new StaticCube{ &defaultShader, gPhysics };
	frontWallRightExitRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(-6.0f, 2.5f, 10.0f));
	frontWallRightExitRoomOne->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// front wall - left part of exit
	StaticCube* frontWallLeftExitRoomOne = new StaticCube{ &defaultShader, gPhysics };
	frontWallLeftExitRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(6.0f, 2.5f, 10.0f));
	frontWallLeftExitRoomOne->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// roof
	Roof* roofRoomOne = new Roof{ &defaultShader, gPhysics };
	roofRoomOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(0.0f, 5.5f, 0.0f));
	roofRoomOne->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);

	Beer* beerOne = new Beer{ &defaultShader, gPhysics };
	beerOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(-5, 1.5f, 0));
	// sparkles never run out of life, when life is zero their life is reset and their direction inverted
	particleGenerator.createParticles(beerOne->getWorldPosition(), glm::vec3(0.0f, 0.25f, 0.0f), ParticleType::BEER_SPARKLE, 3.0f, 10, false, beerOne);

	DynamicCube* dynCubeOne = new DynamicCube{ &defaultShader, gPhysics };
	dynCubeOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(1.25f, 1.5f, 10.0f));
	dynCubeOne->setScale(glm::vec3(0.5, 0.5, 0.5));

	DynamicCube* dynCubeTwo = new DynamicCube{ &defaultShader, gPhysics };
	dynCubeTwo->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(-1.25f, 1.5f, 10.0f));
	dynCubeTwo->setScale(glm::vec3(0.5, 0.5, 0.5));

	DynamicCube* dynCubeThree = new DynamicCube{ &defaultShader, gPhysics };
	dynCubeThree->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(0.0f, 1.5f, 10.0f));
	dynCubeThree->setScale(glm::vec3(0.5, 0.5, 0.5));

	DynamicCube* dynCubeFour = new DynamicCube{ &defaultShader, gPhysics };
	dynCubeFour->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(1.25f, 2.5f, 10.0f));
	dynCubeFour->setScale(glm::vec3(0.5, 0.5, 0.5));

	DynamicCube* dynCubeFive = new DynamicCube{ &defaultShader, gPhysics };
	dynCubeFive->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(-1.25f, 2.5f, 10.0f));
	dynCubeFive->setScale(glm::vec3(0.5, 0.5, 0.5));

	DynamicCube* dynCubeSix = new DynamicCube{ &defaultShader, gPhysics };
	dynCubeSix->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(0.0f, 2.5f, 10.0f));
	dynCubeSix->setScale(glm::vec3(0.5, 0.5, 0.5));

	Button* buttonOne = new Button{ &defaultShader, gPhysics, nullptr };
	buttonOne->setLocalPosition(groundRoomOne->getWorldPosition() + glm::vec3(7.0f, 1.5f, 7.0f));
	buttonOne->setScale(glm::vec3(0.5, 0.5, 0.5));
	buttonOne->setLocalRotation(glm::quat(glm::vec3(glm::radians(-90.0f), 0, 0)));

	roomOne.push_back(dynCubeOne);
	roomOne.push_back(dynCubeTwo);
	roomOne.push_back(dynCubeThree);
	roomOne.push_back(dynCubeFour);
	roomOne.push_back(dynCubeFive);
	roomOne.push_back(dynCubeSix);
	beerIdx.push_back(roomOne.size());
	roomOne.push_back(beerOne);
	roomOne.push_back(buttonOne);
	roomOne.push_back(rightWallRoomOne);
	roomOne.push_back(leftWallRoomOne);
	roomOne.push_back(behindWallRightExitRoomOne);
	roomOne.push_back(behindWallLeftExitRoomOne);
	roomOne.push_back(frontWallRightExitRoomOne);
	roomOne.push_back(frontWallLeftExitRoomOne);
	roomOne.push_back(groundRoomOne);
	roomOne.push_back(roofRoomOne);

	// second room (front/center)
	// ground
	Ground* groundRoomTwo = new Ground{ &defaultShader, gPhysics };
	groundRoomTwo->setLocalPosition(glm::vec3(0.0f, -0.5f, 35.0f));
	groundRoomTwo->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);
	// right wall - front part of exit
	StaticCube* rightWallFrontExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	rightWallFrontExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(-10.0f, 2.5f, 6.0f));
	rightWallFrontExitRoomTwo->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// right wall - behind part of exit
	StaticCube* rightWallBehindExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	rightWallBehindExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(-10.0f, 2.5f, -6.0f));
	rightWallBehindExitRoomTwo->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// left wall - front part of exit
	StaticCube* leftWallFrontExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	leftWallFrontExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(10.0f, 2.5f, 6.0f));
	leftWallFrontExitRoomTwo->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// left wall - behind part of exit 
	StaticCube* leftWallBehindExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	leftWallBehindExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(10.0f, 2.5f, -6.0f));
	leftWallBehindExitRoomTwo->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// behind wall - right part of exit
	StaticCube* behindWallRightExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	behindWallRightExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(-6.0f, 2.5f, -10.0f));
	behindWallRightExitRoomTwo->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// behind wall - left part of exit
	StaticCube* behindWallLeftExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	behindWallLeftExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(6.0f, 2.5f, -10.0f));
	behindWallLeftExitRoomTwo->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// front wall - right part of exit
	StaticCube* frontWallRightExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	frontWallRightExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(-6.0f, 2.5f, 10.0f));
	frontWallRightExitRoomTwo->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// front wall - left part of exit
	StaticCube* frontWallLeftExitRoomTwo = new StaticCube{ &defaultShader, gPhysics };
	frontWallLeftExitRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(6.0f, 2.5f, 10.0f));
	frontWallLeftExitRoomTwo->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// roof
	Roof* roofRoomTwo = new Roof{ &defaultShader, gPhysics };
	roofRoomTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(0.0f, 5.5f, 0.0f));
	roofRoomTwo->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);

	Beer* beerTwo = new Beer{ &defaultShader, gPhysics };
	beerTwo->setLocalPosition(groundRoomTwo->getWorldPosition() + glm::vec3(-5, 1.5f, 2));
	// sparkles never run out of life, when life is zero their life is reset and their direction inverted
	particleGenerator.createParticles(beerTwo->getWorldPosition(), glm::vec3(0.0f, 0.25f, 0.0f), ParticleType::BEER_SPARKLE, 3.0f, 10, false, beerTwo);

	pointLightOnePosition = groundRoomTwo->getWorldPosition() + glm::vec3(0.0f, 2.0f, 0.0f);

	beerIdx.push_back(roomTwo.size());
	roomTwo.push_back(beerTwo);
	roomTwo.push_back(rightWallFrontExitRoomTwo);
	roomTwo.push_back(rightWallBehindExitRoomTwo);
	roomTwo.push_back(leftWallFrontExitRoomTwo);
	roomTwo.push_back(leftWallBehindExitRoomTwo);
	roomTwo.push_back(behindWallRightExitRoomTwo);
	roomTwo.push_back(behindWallLeftExitRoomTwo);
	roomTwo.push_back(frontWallRightExitRoomTwo);
	roomTwo.push_back(frontWallLeftExitRoomTwo);
	roomTwo.push_back(groundRoomTwo);
	roomTwo.push_back(roofRoomTwo);

	// third room (left)
	// floor
	Ground* groundRoomThree = new Ground{ &defaultShader, gPhysics };
	groundRoomThree->setLocalPosition(glm::vec3(35.0f, -0.5f, 35.0f));
	groundRoomThree->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);
	// right wall - front part of exit
	StaticCube* rightWallFrontExitRoomThree = new StaticCube{ &defaultShader, gPhysics };
	rightWallFrontExitRoomThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(-10.0f, 2.5f, 6.0f));
	rightWallFrontExitRoomThree->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// right wall - behind part of exit
	StaticCube* rightWallBehindExitRoomThree = new StaticCube{ &defaultShader, gPhysics };
	rightWallBehindExitRoomThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(-10.0f, 2.5f, -6.0f));
	rightWallBehindExitRoomThree->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// left wall
	StaticCube* leftWallRoomThree = new StaticCube{ &defaultShader, gPhysics };
	leftWallRoomThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(10.0f, 2.5f, 0.0f));
	leftWallRoomThree->setScale(glm::vec3(1.0f, 2.0f, 10.0f), true);
	// behind wall
	StaticCube* behindWallRoomThree = new StaticCube{ &defaultShader, gPhysics };
	behindWallRoomThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(0.0f, 2.5f, -10.0f));
	behindWallRoomThree->setScale(glm::vec3(10.0f, 2.0f, 1.0f), true);
	// front wall
	StaticCube* frontWallRoomThree = new StaticCube{ &defaultShader, gPhysics };
	frontWallRoomThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(0.0f, 2.5f, 10.0f));
	frontWallRoomThree->setScale(glm::vec3(10.0f, 2.0f, 1.0f), true);
	// roof
	Roof* roofRoomThree = new Roof{ &defaultShader, gPhysics };
	roofRoomThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(0.0f, 5.5f, 0.0f));
	roofRoomThree->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);

	Beer* beerThree = new Beer{ &defaultShader, gPhysics };
	beerThree->setLocalPosition(groundRoomThree->getWorldPosition() + glm::vec3(-5, 1.5f, 4));
	// sparkles never run out of life, when life is zero their life is reset and their direction inverted
	particleGenerator.createParticles(beerThree->getWorldPosition(), glm::vec3(0.0f, 0.25f, 0.0f), ParticleType::BEER_SPARKLE, 3.0f, 10, false, beerThree);

	beerIdx.push_back(roomThree.size());
	roomThree.push_back(beerThree);
	roomThree.push_back(rightWallFrontExitRoomThree);
	roomThree.push_back(rightWallBehindExitRoomThree);
	roomThree.push_back(leftWallRoomThree);
	roomThree.push_back(behindWallRoomThree);
	roomThree.push_back(frontWallRoomThree);
	roomThree.push_back(groundRoomThree);
	roomThree.push_back(roofRoomThree);

	// fourth room (right)
	// floor
	Ground* groundRoomFour = new Ground{ &defaultShader, gPhysics };
	groundRoomFour->setLocalPosition(glm::vec3(-35.0f, -0.5f, 35.0f));
	groundRoomFour->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);
	// right wall
	StaticCube* rightWallRoomFour = new StaticCube{ &defaultShader, gPhysics };
	rightWallRoomFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(-10.0f, 2.5f, 0.0f));
	rightWallRoomFour->setScale(glm::vec3(1.0f, 2.0f, 10.0f), true);
	// left wall - front part of exit
	StaticCube* leftWallFrontExitRoomFour = new StaticCube{ &defaultShader, gPhysics };
	leftWallFrontExitRoomFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(10.0f, 2.5f, 6.0f));
	leftWallFrontExitRoomFour->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// left wall - behind part of exit 
	StaticCube* leftWallBehindExitRoomFour = new StaticCube{ &defaultShader, gPhysics };
	leftWallBehindExitRoomFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(10.0f, 2.5f, -6.0f));
	leftWallBehindExitRoomFour->setScale(glm::vec3(1.0f, 2.0f, 4.0f), true);
	// behind wall
	StaticCube* behindWallRoomFour = new StaticCube{ &defaultShader, gPhysics };
	behindWallRoomFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(0.0f, 2.5f, -10.0f));
	behindWallRoomFour->setScale(glm::vec3(10.0f, 2.0f, 1.0f), true);
	// front wall
	StaticCube* frontWallRoomFour = new StaticCube{ &defaultShader, gPhysics };
	frontWallRoomFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(0.0f, 2.5f, 10.0f));
	frontWallRoomFour->setScale(glm::vec3(10.0f, 2.0f, 1.0f), true);
	// roof
	Roof* roofRoomFour = new Roof{ &defaultShader, gPhysics };
	roofRoomFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(0.0f, 5.5f, 0.0f));
	roofRoomFour->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);

	Beer* beerFour = new Beer{ &defaultShader, gPhysics };
	beerFour->setLocalPosition(groundRoomFour->getWorldPosition() + glm::vec3(-5, 1.5f, 6));
	// sparkles never run out of life, when life is zero their life is reset and their direction inverted
	particleGenerator.createParticles(beerFour->getWorldPosition(), glm::vec3(0.0f, 0.25f, 0.0f), ParticleType::BEER_SPARKLE, 3.0f, 10, false, beerFour);

	beerIdx.push_back(roomFour.size());
	roomFour.push_back(beerFour);
	roomFour.push_back(rightWallRoomFour);
	roomFour.push_back(leftWallFrontExitRoomFour);
	roomFour.push_back(leftWallBehindExitRoomFour);
	roomFour.push_back(behindWallRoomFour);
	roomFour.push_back(frontWallRoomFour);
	roomFour.push_back(groundRoomFour);
	roomFour.push_back(roofRoomFour);

	// fifth room (front front)
	// floor
	Ground* groundRoomFive = new Ground{ &defaultShader, gPhysics };
	groundRoomFive->setLocalPosition(glm::vec3(0.0f, -0.5f, 70.0f));
	groundRoomFive->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);
	// right wall
	StaticCube* rightWallRoomFive = new StaticCube{ &defaultShader, gPhysics };
	rightWallRoomFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(-10.0f, 2.5f, 0.0f));
	rightWallRoomFive->setScale(glm::vec3(1.0f, 2.0f, 10.0f), true);
	// left wall
	StaticCube* leftWallRoomFive = new StaticCube{ &defaultShader, gPhysics };
	leftWallRoomFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(10.0f, 2.5f, 0.0f));
	leftWallRoomFive->setScale(glm::vec3(1.0f, 2.0f, 10.0f), true);
	// behind wall - right part of exit
	StaticCube* behindWallRightExitRoomFive = new StaticCube{ &defaultShader, gPhysics };
	behindWallRightExitRoomFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(-6.0f, 2.5f, -10.0f));
	behindWallRightExitRoomFive->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// behind wall - left part of exit
	StaticCube* behindWallLeftExitRoomFive = new StaticCube{ &defaultShader, gPhysics };
	behindWallLeftExitRoomFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(6.0f, 2.5f, -10.0f));
	behindWallLeftExitRoomFive->setScale(glm::vec3(4.0f, 2.0f, 1.0f), true);
	// front wall
	StaticCube* frontWallRoomFive = new StaticCube{ &defaultShader, gPhysics };
	frontWallRoomFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(0.0f, 2.5f, 10.0f));
	frontWallRoomFive->setScale(glm::vec3(10.0f, 2.0f, 1.0f), true);
	// roof
	Roof* roofRoomFive = new Roof{ &defaultShader, gPhysics };
	roofRoomFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(0.0f, 5.5f, 0.0f));
	roofRoomFive->setScale(glm::vec3(10.0f, 1.0f, 10.0f), true);

	Beer* beerFive = new Beer{ &defaultShader, gPhysics };
	beerFive->setLocalPosition(groundRoomFive->getWorldPosition() + glm::vec3(-5, 1.0f, 8));
	// sparkles never run out of life, when life is zero their life is reset and their direction inverted
	particleGenerator.createParticles(beerFive->getWorldPosition(), glm::vec3(0.0f, 0.25f, 0.0f), ParticleType::BEER_SPARKLE, 3.0f, 10, false, beerFive);

	beerIdx.push_back(roomFive.size());
	roomFive.push_back(beerFive);
	roomFive.push_back(rightWallRoomFive);
	roomFive.push_back(leftWallRoomFive);
	roomFive.push_back(behindWallRightExitRoomFive);
	roomFive.push_back(behindWallLeftExitRoomFive);
	roomFive.push_back(frontWallRoomFive);
	roomFive.push_back(groundRoomFive);
	roomFive.push_back(roofRoomFive);

	// ----------------------------------------------------

	// corridor into first room
	Ground* groundCorridorFirst = new Ground{ &defaultShader, gPhysics };
	groundCorridorFirst->setLocalPosition(glm::vec3(0.0f, -0.5f, -17.5f));
	groundCorridorFirst->setScale(glm::vec3(2.0f, 1.0f, 7.5f), true);
	// right wall
	StaticCube* rightWallCorridorFirst = new StaticCube{ &defaultShader, gPhysics };
	rightWallCorridorFirst->setLocalPosition(groundCorridorFirst->getWorldPosition() + glm::vec3(-3.0f, 2.5f, 0.0f));
	rightWallCorridorFirst->setScale(glm::vec3(1.0f, 2.0f, 6.5f), true);
	// left wall
	StaticCube* leftWallCorridorFirst = new StaticCube{ &defaultShader, gPhysics };
	leftWallCorridorFirst->setLocalPosition(groundCorridorFirst->getWorldPosition() + glm::vec3(3.0f, 2.5f, 0.0f));
	leftWallCorridorFirst->setScale(glm::vec3(1.0f, 2.0f, 6.5f), true);
	// Behind wall
	StaticCube* behinWallCorridorFirst = new StaticCube{ &defaultShader, gPhysics };
	behinWallCorridorFirst->setLocalPosition(groundCorridorFirst->getWorldPosition() + glm::vec3(0.0f, 2.5f, -5.5f));
	behinWallCorridorFirst->setScale(glm::vec3(2.0f, 2.0f, 1.0f), true);

	corridors.push_back(rightWallCorridorFirst);
	corridors.push_back(leftWallCorridorFirst);
	corridors.push_back(behinWallCorridorFirst);
	corridors.push_back(groundCorridorFirst);

	// corridor first and second room
	// floor
	Ground* groundCorridorFirstSecond = new Ground{ &defaultShader, gPhysics };
	groundCorridorFirstSecond->setLocalPosition(glm::vec3(0.0f, -0.5f, 17.5f));
	groundCorridorFirstSecond->setScale(glm::vec3(2.0f, 1.0f, 7.5f), true);
	// right wall
	StaticCube* rightWallCorridorFirstSecond = new StaticCube{ &defaultShader, gPhysics };
	rightWallCorridorFirstSecond->setLocalPosition(groundCorridorFirstSecond->getWorldPosition() + glm::vec3(-3.0f, 2.5f, 0.0f));
	rightWallCorridorFirstSecond->setScale(glm::vec3(1.0f, 2.0f, 6.5f), true);
	// left wall
	StaticCube* leftWallCorridorFirstSecond = new StaticCube{ &defaultShader, gPhysics };
	leftWallCorridorFirstSecond->setLocalPosition(groundCorridorFirstSecond->getWorldPosition() + glm::vec3(3.0f, 2.5f, 0.0f));
	leftWallCorridorFirstSecond->setScale(glm::vec3(1.0f, 2.0f, 6.5f), true);

	corridors.push_back(rightWallCorridorFirstSecond);
	corridors.push_back(leftWallCorridorFirstSecond);
	corridors.push_back(groundCorridorFirstSecond);

	// corridor second and third room
	// floor
	Ground* groundCorridorSecondThird = new Ground{ &defaultShader, gPhysics };
	groundCorridorSecondThird->setLocalPosition(glm::vec3(17.5f, -0.5f, 35.0f));
	groundCorridorSecondThird->setScale(glm::vec3(7.5f, 1.0f, 2.0f), true);
	// right wall
	StaticCube* rightWallCorridorSecondThird = new StaticCube{ &defaultShader, gPhysics };
	rightWallCorridorSecondThird->setLocalPosition(groundCorridorSecondThird->getWorldPosition() + glm::vec3(0.0f, 2.5f, -3.0f));
	rightWallCorridorSecondThird->setScale(glm::vec3(6.5f, 2.0f, 1.0f), true);
	// left wall
	StaticCube* leftWallCorridorSecondThird = new StaticCube{ &defaultShader, gPhysics };
	leftWallCorridorSecondThird->setLocalPosition(groundCorridorSecondThird->getWorldPosition() + glm::vec3(0.0f, 2.5f, 3.0f));
	leftWallCorridorSecondThird->setScale(glm::vec3(6.5f, 2.0f, 1.0f), true);

	corridors.push_back(rightWallCorridorSecondThird);
	corridors.push_back(leftWallCorridorSecondThird);
	corridors.push_back(groundCorridorSecondThird);

	// corridor second and fourth room
	Ground* groundCorridorSecondFourth = new Ground{ &defaultShader, gPhysics };
	groundCorridorSecondFourth->setLocalPosition(glm::vec3(-17.5f, -0.5f, 35.0f));
	groundCorridorSecondFourth->setScale(glm::vec3(7.5f, 1.0f, 2.0f), true);
	// right wall
	StaticCube* rightWallCorridorSecondFourth = new StaticCube{ &defaultShader, gPhysics };
	rightWallCorridorSecondFourth->setLocalPosition(groundCorridorSecondFourth->getWorldPosition() + glm::vec3(0.0f, 2.5f, -3.0f));
	rightWallCorridorSecondFourth->setScale(glm::vec3(6.5f, 2.0f, 1.0f), true);
	// left wall
	StaticCube* leftWallCorridorSecondFourth = new StaticCube{ &defaultShader, gPhysics };
	leftWallCorridorSecondFourth->setLocalPosition(groundCorridorSecondFourth->getWorldPosition() + glm::vec3(0.0f, 2.5f, 3.0f));
	leftWallCorridorSecondFourth->setScale(glm::vec3(6.5f, 2.0f, 1.0f), true);

	corridors.push_back(rightWallCorridorSecondFourth);
	corridors.push_back(leftWallCorridorSecondFourth);
	corridors.push_back(groundCorridorSecondFourth);

	// corridor second and fifth room
	// floor
	Ground* groundCorridorSecondFifth = new Ground{ &defaultShader, gPhysics };
	groundCorridorSecondFifth->setLocalPosition(glm::vec3(0.0f, -0.5f, 52.5f));
	groundCorridorSecondFifth->setScale(glm::vec3(2.0f, 1.0f, 7.5f), true);
	// right wall
	StaticCube* rightWallCorridorSecondFifth = new StaticCube{ &defaultShader, gPhysics };
	rightWallCorridorSecondFifth->setLocalPosition(groundCorridorSecondFifth->getWorldPosition() + glm::vec3(-3.0f, 2.5f, 0.0f));
	rightWallCorridorSecondFifth->setScale(glm::vec3(1.0f, 2.0f, 6.5f), true);
	// left wall
	StaticCube* leftWallCorridorSecondFifth = new StaticCube{ &defaultShader, gPhysics };
	leftWallCorridorSecondFifth->setLocalPosition(groundCorridorSecondFifth->getWorldPosition() + glm::vec3(3.0f, 2.5f, 0.0f));
	leftWallCorridorSecondFifth->setScale(glm::vec3(1.0f, 2.0f, 6.5f), true);

	corridors.push_back(rightWallCorridorSecondFifth);
	corridors.push_back(leftWallCorridorSecondFifth);
	corridors.push_back(groundCorridorSecondFifth);
}

void static renderHUD(MyTextRenderer textRenderer, MyShader textShader) {
	if (isPaused == false) {
		// Scoreboard - replaced by GUI beers
		//textRenderer.renderText(textShader, "Score: " + std::to_string(score) + "/" + std::to_string(maxScore), 14.0f, 14.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
		// Health - replaced by GUI health
		//textRenderer.renderText(textShader, "Health: " + std::to_string(player->getHealth()) + "/" + std::to_string(player->maxHealth), 14.0f, 38.0f, 0.5f, glm::vec3(1, 1, 1), enableWireframe);
		// Ammo - replaced by GUI bullets
		//textRenderer.renderText(textShader, "Ammo: " + std::to_string(bullets) + "/" + std::to_string(maxBullets), 14.0f, 62.0f, 0.5f, glm::vec3(1, 1, 1), enableWireframe);

		if (player->getActiveCameraType() == PlayerCameraType::CAMERA_FIRST_PERSON) {
			// display crosshair
			textRenderer.renderText(textShader, "+", ((float)screenWidth / 2.0f) - (48.0f * 0.2f), ((float)screenHeight / 2.0f) - (48.0f * 0.2f), 0.8f, glm::vec3(1, 1, 1), enableWireframe);
		}
	} else {
		if (isPaused == true && isGameLost == false && isGameWon == false) {
			// Pause Menu
			textRenderer.renderText(textShader, "GAME PAUSED", 20.0f, (float)screenHeight - 62.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "PRESS [X] TO QUIT", 24.0f, (float)screenHeight - 90.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "CONTROLS", 24.0f, (float)screenHeight - 150.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "WASD", 24.0f, (float)screenHeight - 170.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Move", 180.0f, (float)screenHeight - 170.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "L SHIFT", 24.0f, (float)screenHeight - 190.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Sprint", 180.0f, (float)screenHeight - 190.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "Mouse", 24.0f, (float)screenHeight - 210.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Look Around", 180.0f, (float)screenHeight - 210.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "LMB", 24.0f, (float)screenHeight - 230.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Shoot", 180.0f, (float)screenHeight - 230.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "E", 24.0f, (float)screenHeight - 250.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Interact", 180.0f, (float)screenHeight - 250.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "Escape", 24.0f, (float)screenHeight - 270.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Pause / Unpause", 180.0f, (float)screenHeight - 270.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "+/- (Numpad)", 24.0f, (float)screenHeight - 290.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Increase/Decrease Exposure", 180.0f, (float)screenHeight - 290.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "Page Up/Down", 24.0f, (float)screenHeight - 310.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Increase/Decrease Gamma", 180.0f, (float)screenHeight - 310.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);

			textRenderer.renderText(textShader, "Arrow Up/Down", 24.0f, (float)screenHeight - 330.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Increase/Decrease Volume", 180.0f, (float)screenHeight - 330.0f, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
		}

		if (isGameLost == true) {
			textRenderer.renderText(textShader, "GAME OVER", (float)(screenWidth / 2.0f) * 0.8f, (float)(screenHeight / 2.0f) , 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Press [Enter] to restart", (float)(screenWidth / 2.0f) * 0.8375f, (float)(screenHeight / 2.0f) * 0.9f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
		}

		if (isGameWon == true) {
			textRenderer.renderText(textShader, "You Win!", (float)(screenWidth / 2.0f) * 0.875f, (float)(screenHeight / 2.0f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
			textRenderer.renderText(textShader, "Press [Enter] to restart", (float)(screenWidth / 2.0f) * 0.8375f, (float)(screenHeight / 2.0f) * 0.9f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), enableWireframe);
		}
	}

	if (enableDebugHUD) {
		static int delayFrameCounter = 0;
		static float previousFPS = framesPerSecond;
		if (delayFrameCounter-- == 0) {
			textRenderer.renderText(textShader,
				"FPS: " + std::to_string((double)framesPerSecond) + 
				std::string(", FOV: ") + std::to_string(player->getActiveCamera()->getFov()) + std::string(", Gamma:") + std::to_string(gamma) + std::string(", Exposure:") + std::to_string(exposure) + std::string(", Player on Ground: ") + std::string(player->isOnGround() ? "yes" : "no"),
				14.0f, (float)screenHeight - 20.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
			previousFPS = framesPerSecond;
			delayFrameCounter += 4; // draw only every few frames, because otherwise it's unreadable
		} else {
			textRenderer.renderText(textShader,
				"FPS: " + std::to_string((double)previousFPS) +
				std::string(", FOV: ") + std::to_string(player->getActiveCamera()->getFov()) + std::string(", Gamma:") + std::to_string(gamma) + std::string(", Exposure:") + std::to_string(exposure) + std::string(", Player on Ground: ") + std::string(player->isOnGround() ? "yes" : "no"),
				14.0f, (float)screenHeight - 20.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		}

		textRenderer.renderText(textShader, "[F1] Wireframe Mode: " + std::string(enableWireframe ? "ON" : "OFF"), 14.0f, (float)screenHeight - 34.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F2] Backface Culling: " + std::string(enableBackfaceCulling ? "ON" : "OFF"), 14.0f, (float)screenHeight - 48.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F3] Game-HUD: " + std::string(enableGUIHUD ? "ON" : "OFF"), 14.0f, (float)screenHeight - 62.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F4] Normal Mapping: " + std::string(enableNormalMapping ? "ON" : "OFF"), 14.0f, (float)screenHeight - 76.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F5] Display Normals: " + std::string(enableShowNormals ? "ON" : "OFF"), 14.0f, (float)screenHeight - 90.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F6] Bloom: " + std::string(enableBloom ? "ON" : "OFF"), 14.0f, (float)screenHeight - 104.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F8] View-Frustum Culling: OFF", 14.0f, (float)screenHeight - 118.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F9] Zombie Pathfinding: " + std::string(freezeZombies ? "DISABLED" : "ENABLED"), 14.0f, (float)screenHeight - 132.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		textRenderer.renderText(textShader, "[F10] Close Debug HUD", 14.0f, (float)screenHeight - 152.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
		//textRenderer.renderText(textShader, ", Player on Ground: " + std::string(player->isOnGround() ? "yes" : "no") + std::string(", FOV: ") + std::to_string(player->getActiveCamera()->getFov()) + std::string(", Gamma:") + std::to_string(gamma) + std::string(", Exposure:") + std::to_string(exposure), 14.0f, (float)screenHeight - 138.0f, 0.25f, glm::vec3(0.2f, 1.0f, 0.2f), enableWireframe);
	}
}

void renderBloomQuad() {
	if (bloomQuadVAO == 0) {
		float quadVertices[] = {
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &bloomQuadVAO);
		glGenBuffers(1, &bloomQuadVBO);
		glBindVertexArray(bloomQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, bloomQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(bloomQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);
	glBindVertexArray(0);
}

void setupBloomBuffers() {
	glGenFramebuffers(1, &hdrFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer);
	// 2 floating point color buffers, 1 for normal rendering, 2 for brightness thresholds
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to corresponding framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depthBuffer (renderBuffer)
	glGenRenderbuffers(1, &renderBufferDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferDepth);
	// tell openGL which color attachments to use for rendering
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(2, attachments);
	// check framebuffer for completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer (depth) not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong framebuffer for blurring (bloom effect around light)
	glGenFramebuffers(2, pingPongFrameBuffer);
	glGenTextures(2, pingPongColorBuffers);
	for (unsigned int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFrameBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongColorBuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer (blur) not complete!" << std::endl;
		}
	}
}

void adjustBloomBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer);
	// 2 floating point color buffers, 1 for normal rendering, 2 for brightness thresholds
	for (unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to corresponding framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depthBuffer (renderBuffer)
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferDepth);
	// tell openGL which color attachments to use for rendering
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(2, attachments);
	// check framebuffer for completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer (depth) not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong framebuffer for blurring (bloom effect around light)
	for (unsigned int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFrameBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongColorBuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer (blur) not complete!" << std::endl;
		}
	}
}

void createTextProjection() {
	textProjection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));
	textShader.use();
	textShader.setMat4("projection", textProjection);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// F1 - Wireframe
	// F2 - Culling
	// F3 - Toggle GUI HUD
	// F4 - Toggle normal mapping
	// F5 - Toggle displaying normals
	// F6 - Toggle debug text HUD
	// F8 - Toggle view-frustum culling
	// Esc - Exit
	// Enter - restart game if won/lost
	// E - Interact with Object
	// R - Reload
	// C - Switch First/Third Person Camera
	// F - Toggle flashlight
	// +/- - increase/decrease illumination

	if (action != GLFW_PRESS) {
		return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
		// Pause game
		if (isGameLost == false && isGameWon == false) {
			isPaused = !isPaused;
		}
		break;
	case GLFW_KEY_ENTER:
		// restart game if over
		if (isGameLost == true || isGameWon == true) {
			scene->reset();
			bullets = maxBullets;
			score = 0;
			isGameLost = false;
			isGameWon = false;
			isPaused = false;
		}
		break;
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
		// toggle GUI HUD
		enableGUIHUD = !enableGUIHUD;
		break;
	case GLFW_KEY_F4:
		// toggle normal mapping
		enableNormalMapping = !enableNormalMapping;
		break;
	case GLFW_KEY_F5:
		// toggle normal map value display
		enableShowNormals = !enableShowNormals;
		break;
	case GLFW_KEY_F6:
		// toggle bloom
		enableBloom = !enableBloom;
		break;
	case GLFW_KEY_F8:
		// toggle view-frustum culling
		std::cout << "Toggle view-frustum culling" << std::endl;
		break;
	case GLFW_KEY_F9:
		// freeze zombies
		freezeZombies = !freezeZombies;
		break;
	case GLFW_KEY_F10:
		// toggle debug text hud
		enableDebugHUD = !enableDebugHUD;
		break;
	case GLFW_KEY_UP:
		// increase volume
		audioVolume += 0.1f;
		if (audioVolume > 1.0f) {
			audioVolume = 1.0f;
		}
		MyAssetManager::irrKlangSoundEngine_->setSoundVolume(audioVolume);
		break;
	case GLFW_KEY_DOWN:
		// increase volume
		audioVolume -= 0.1f;
		if (audioVolume < 0.0f) {
			audioVolume = 0.0f;
		}
		MyAssetManager::irrKlangSoundEngine_->setSoundVolume(audioVolume);
		break;
	case GLFW_KEY_KP_ADD:
		// increase illumination
		exposure += 0.1f;
		break;
	case GLFW_KEY_KP_SUBTRACT:
		// decrease illumination
		exposure -= 0.1f;
		if (exposure < 0.1f) {
			exposure = 0.1f;
		}
		break;
	case GLFW_KEY_PAGE_UP:
		gamma += 0.1f;
		break;
	case GLFW_KEY_PAGE_DOWN: 
		gamma -= 0.1f;
		if (gamma < 0.1f) {
			gamma = 0.1f;
		}
		break;
	}

	if (!isPaused) {
		// these buttons only work if the game is unpaused
		switch (key) {
		case GLFW_KEY_E:
			scene->startPlayerInteraction();
			break;
		case GLFW_KEY_F:
			playerFlashLight->setEnabled(!playerFlashLight->isEnabled());
			break;
		case GLFW_KEY_C:
			// swap camera
			player->swapCamera();
			break;
		case GLFW_KEY_R:
			// reload
			bullets = maxBullets;
			MyAssetManager::playSound("reload");
			break;
		/*
		case GLFW_KEY_SPACE:
			player->jump();
			break;
		*/
		}
	} else {
		// these buttons only work if the game is paused
		switch (key) {
		case GLFW_KEY_X:
			glfwSetWindowShouldClose(window, true);
			break;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (!isPaused) {
		scene->handleMouseButtonInput(window, button, action, mods);
	}
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true;
	static float lastX = 0.0f;
	static float lastY = 0.0f;

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

	if (!isPaused) {
		scene->handleMouseInput(xOffset, yOffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//camera.handleMouseScrolling(yoffset);
	player->processMouseScrolling(yoffset);

	//std::cout << "Mouse scroll event" << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	//std::cout << "Size callback: width: " << width << " height: " << height << std::endl;

	glViewport(0, 0, width, height);

	screenWidth = width;
	screenHeight = height;

	adjustBloomBuffers();

	createTextProjection();

	//std::cout << "Viewport event" << std::endl;
}

void readINIFile() {
	INIReader iniReader = INIReader::INIReader("settings.ini");

	screenWidth = iniReader.GetInteger("window", "width", SCR_WIDTH_DEFAULT);
	screenHeight = iniReader.GetInteger("window", "height", SCR_HEIGHT_DEFAULT);
	refreshRate = iniReader.GetInteger("window", "refresh_rate", REFRESH_RATE_DEFAULT);
	windowTitle = iniReader.Get("window", "title", WINDOW_TITLE_DEFAULT);
	startFullscreen = iniReader.GetBoolean("window", "fullscreen", START_FULLSCREEN_DEFAULT);

	cameraFov = iniReader.GetReal("camera", "fov", CAMERA_FOV_DEFAULT);
	cameraNear = iniReader.GetReal("camera", "near", CAMERA_NEAR_DEFAULT);
	cameraFar = iniReader.GetReal("camera", "far", CAMERA_FAR_DEFAULT);

	audioVolume = iniReader.GetReal("audio", "volume", AUDIO_VOLUME_DEFAULT);
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

void static initIrrKlang() {
	MyAssetManager::irrKlangSoundEngine_->setSoundVolume(audioVolume);

	MyAssetManager::loadSoundSource("assets/sounds/bell.wav", "bell");
	MyAssetManager::loadSoundSource("assets/sounds/gunshot.mp3", "gunshot");
	MyAssetManager::loadSoundSource("assets/sounds/reload.mp3", "reload");
	MyAssetManager::loadSoundSource("assets/sounds/grunt_0.mp3", "grunt_0");
	MyAssetManager::loadSoundSource("assets/sounds/grunt_1.mp3", "grunt_1");
	MyAssetManager::loadSoundSource("assets/sounds/grunt_2.mp3", "grunt_2");
	MyAssetManager::loadSoundSource("assets/sounds/grunt_3.mp3", "grunt_3");
	MyAssetManager::loadSoundSource("assets/sounds/grunt_4.mp3", "grunt_4");
	MyAssetManager::loadSoundSource("assets/sounds/grunt_5.mp3", "grunt_5");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_0.mp3", "zombie_attack_0");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_1.mp3", "zombie_attack_1");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_2.mp3", "zombie_attack_2");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_3.mp3", "zombie_attack_3");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_4.mp3", "zombie_attack_4");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_5.mp3", "zombie_attack_5");
	MyAssetManager::loadSoundSource("assets/sounds/zombie_attack_6.mp3", "zombie_attack_6");
	MyAssetManager::loadSoundSource("assets/sounds/button_click.wav", "button_click");
	MyAssetManager::loadSoundSource("assets/sounds/wood_hit.mp3", "wood_hit");
	MyAssetManager::loadSoundSource("assets/sounds/wood_destroyed.mp3", "wood_destroyed");
	MyAssetManager::loadSoundSource("assets/sounds/footstep_0.mp3", "footstep_0");
}

void error_callback(int errorCode, const char* description) {
	std::cout << "[GLFW Error " << errorCode << "]: " << description << std::endl;
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
	case GL_DEBUG_SOURCE_API:
	{
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION:
	{
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
	{
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
	{
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY:
	{
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER:
	{
		sourceString = "Other";
		break;
	}
	default:
	{
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
	{
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	{
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
	{
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
	{
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE:
	{
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER:
	{
		typeString = "Other";
		break;
	}
	default:
	{
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
	{
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM:
	{
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW:
	{
		severityString = "Low";
		break;
	}
	default:
	{
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