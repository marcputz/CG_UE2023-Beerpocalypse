#include "Utils.h"
#include <sstream>
#include <stdio.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "GameObjects/GameObject.h"



/* ------------------------- */
/*        PROTOTYPES         */
/* ------------------------- */

void static initOpenGL();
static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);

void static initBullet();
void static destroyBullet();

/* ------------------------- */
/*     GLOBAL VARIABLES      */
/* ------------------------- */

GLFWwindow* window;
string windowTitle = "Beerpocalypse (CG SS2023)";
bool enableWireframe = false;
bool enableBackfaceCulling = false;

// Bullet Physics Engine
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* collisionDispatcher;
btBroadphaseInterface* overlappingPairCache;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;


/* ------------------------- */
/*           MAIN            */
/* ------------------------- */

int main(int argc, char** argv) {

	// Initialize OpenGL
	initOpenGL();
	std::cout << "OpenGL initialized" << std::endl;

	// Initialize Bullet Physics Engine
	initBullet();
	std::cout << "Bullet initialized" << std::endl;


	/*---- RENDER LOOP -----*/
	float time = float(glfwGetTime());
	float deltaTime = 0.0f;
	float timeSum = 0.0f;
	std::cout << "Render Loop START" << std::endl;
	while (!glfwWindowShouldClose(window)) {
		// Clear backbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Poll events
		glfwPollEvents();

		drawTeapot();


		// Compute frame time
		deltaTime = time;
		time = float(glfwGetTime());
		deltaTime = time - deltaTime;
		timeSum += deltaTime;

		// Swap buffers
		glfwSwapBuffers(window);
	}
	std::cout << "Render Loop STOP" << std::endl;



	// Breakdown Bullet Physics Engine
	//destroyBullet();

	// Breakdown OpenGL
	destroyFramework();
	glfwTerminate();

	// Close Application with no error
	return 0;
}

/* ------------------------- */
/*        FUNCTIONS          */
/* ------------------------- */

void static initBullet() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void static destroyBullet() {
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete collisionDispatcher;
	delete collisionConfiguration;
}

void static initOpenGL() {
	
	/* --------------------------------------------- */
	// Init GLFW
	/* --------------------------------------------- */

	glfwSetErrorCallback([](int error, const char* description) { std::cout << "[GLFW Error " << error << "]: " << description << std::endl; });

	if (!glfwInit()) {
		glfwTerminate();
		std::cout << "[GLFW Error]: Failed to init GLFW" << std::endl;
		exit(101);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, 60); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	GLFWmonitor* monitor = nullptr;

	if (false)
		monitor = glfwGetPrimaryMonitor();

	window = glfwCreateWindow(800, 800, windowTitle.c_str(), monitor, nullptr);

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

	// Set standard values for OpenGL
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
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