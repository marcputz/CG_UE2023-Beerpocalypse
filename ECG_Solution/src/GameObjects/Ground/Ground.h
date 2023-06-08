#pragma once

#include "../NewGameObject.h"

class Ground : public NewGameObject {
private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(NewGameObject* otherObject) {}

public:
	Ground(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime) {};
	void processMouseInput(float offsetX, float offsetY) {};

	// Inherited via NewGameObject
	virtual void onHealthChange(int oldHealth, int newHealth) override;
};