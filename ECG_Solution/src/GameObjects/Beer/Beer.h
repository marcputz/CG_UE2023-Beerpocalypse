#pragma once

#include "../GameObject.h"

class Beer : public GameObject {

private:
	bool animateRotation = true;

	float animationTime = 0.0f;
	float rotationAngle = 0.0f;
	const float totalAnimationTime = 4.0f;

private:
	void onBeforeUpdate() override;
	void onUpdate(float deltaTime) override;

public:
	Beer(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime) override;
	void processMouseInput(float offsetX, float offsetY) override;
	void onCollision(GameObject* otherObject) override;

	// Inherited via GameObject
	virtual void onHealthChange(int oldHealth, int newHealth) override;

	// Inherited via GameObject
	virtual void resetSpecifics() override;
};