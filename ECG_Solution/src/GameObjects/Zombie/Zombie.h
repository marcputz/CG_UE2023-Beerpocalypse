#pragma once

#include "../NewGameObject.h"

class Zombie : public NewGameObject {

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(NewGameObject* otherObject) {}

public:
	Zombie(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);

	virtual void onHealthChange(int oldHealth, int newHealth) override;
};