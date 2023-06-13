#pragma once

#include "../GameObject.h"

class Zombie : public GameObject {
private:
	const float movementSpeed = 2.0f;
	GameObject* followObject = nullptr;

public:
	bool canSeePlayer = false;

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(GameObject* otherObject) {}

public:
	Zombie(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);

	virtual void onHealthChange(int oldHealth, int newHealth) override;

	bool isFollowing();
	GameObject* getFollowing();
	void follow(GameObject* objectToFollow);

	// Inherited via GameObject
	virtual void resetSpecifics() override;
};