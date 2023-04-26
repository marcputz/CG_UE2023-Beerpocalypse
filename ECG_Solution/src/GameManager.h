#pragma once

#include <vector>
#include <memory>
#include <sstream>
#include <glm/glm.hpp>

#include "MyFPSCamera.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Player/Player.h"

class GameManager {
public:

	GameManager(PxPhysics* physics, MyFPSCamera &camera);
	virtual ~GameManager();

	void setPlayer(Player* player);
	void draw();
	void stepUpdate(float deltaTime);
	void handleInput(GLFWwindow* window, float deltaTime);
	void addObject(GameObject* gameObject);

protected:
	PxPhysics* physics_;
	MyFPSCamera* camera_;
	Player* player_;

	std::list<GameObject*> gameObjects_;
	PxScene* physicsScene_;

private:
	void stepPhysics(float deltaTime);
};
