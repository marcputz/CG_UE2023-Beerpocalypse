#pragma once

#include "../NewGameObject.h"

class Zombie : public NewGameObject {
private:
	const float movementSpeed = 2.0f;
	NewGameObject* followObject = nullptr;

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(NewGameObject* otherObject) {}

public:
	Zombie(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);

	virtual void onHealthChange(int oldHealth, int newHealth) override;

	bool isFollowing();
	NewGameObject* getFollowing();
	void follow(NewGameObject* objectToFollow);
};