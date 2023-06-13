#pragma once

#include "../NewGameObject.h"

class Button : public NewGameObject {

public:
	Button(MyShader* shader, PxPhysics* physics);

	void interact();


	// Geerbt über NewGameObject
	virtual void onBeforeUpdate() override;

	virtual void onUpdate(float deltaTime) override;

	virtual void resetSpecifics() override;

	virtual void processWindowInput(GLFWwindow* window, float deltaTime) override;

	virtual void processMouseInput(float offsetX, float offsetY) override;

	virtual void onCollision(NewGameObject* otherObject) override;

	virtual void onHealthChange(int oldHealth, int newHealth) override;

};