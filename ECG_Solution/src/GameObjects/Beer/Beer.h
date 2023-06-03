#pragma once

#include "../NewGameObject.h"

class Beer : public NewGameObject {

private:
	bool animateRotation = true;

private:
	void onBeforeUpdate() override;
	void onUpdate(float deltaTime) override;

public:
	Beer(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime) override;
	void processMouseInput(float offsetX, float offsetY) override;
	void onCollision(NewGameObject* otherObject) override;
};