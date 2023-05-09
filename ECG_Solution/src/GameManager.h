#pragma once

#include <vector>
#include <memory>
#include <sstream>
#include <glm/glm.hpp>

#include "MyFPSCamera.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Player/Player.h"
#include "Lights/DirectionalLight/MyDirectionalLight.h"
#include "Lights/PointLight/MyPointLight.h"
#include "Lights/SpotLight/MySpotLight.h"

class GameManager {
public:

	GameManager(PxPhysics* physics, MyFPSCamera &camera);
	virtual ~GameManager();

	void setPlayer(Player* player);
	void draw();
	void stepUpdate(float deltaTime);
	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);
	void toggleFlashlight();
	void addObject(GameObject* gameObject);
	void addLight(MyDirectionalLight* dirLight);
	void addLight(MyPointLight* pointLight);
	void addLight(MySpotLight* spotLight);
	void setPlayerFlashLight(MySpotLight* spotLight);

protected:
	PxPhysics* physics_;
	MyFPSCamera* camera_;
	Player* player_;

	std::list<GameObject*> gameObjects_;
	PxScene* physicsScene_;

	MyDirectionalLight* dirLight_;
	std::list<MyPointLight*> pointLights_;
	std::list<MySpotLight*> spotLights_;
	MySpotLight* playerFlashLight_;

private:
	void stepPhysics(float deltaTime);
};
