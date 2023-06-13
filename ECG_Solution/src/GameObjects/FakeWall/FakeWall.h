#pragma once

#include "../GameObject.h"
#include "../Button/Button.h"

class FakeWall : public GameObject, public Activatable {
private:

public:
	FakeWall(MyShader* shader, PxPhysics* physics);

	// Geerbt über GameObject
	virtual void onBeforeUpdate() override;

	virtual void onUpdate(float deltaTime) override;

	virtual void resetSpecifics() override;

	virtual void processWindowInput(GLFWwindow* window, float deltaTime) override;

	virtual void processMouseInput(float offsetX, float offsetY) override;

	virtual void onCollision(GameObject* otherObject) override;

	virtual void onHealthChange(int oldHealth, int newHealth) override;


	// Geerbt über Activatable
	virtual void onActivate() override;

	virtual void onDeactivate() override;

};