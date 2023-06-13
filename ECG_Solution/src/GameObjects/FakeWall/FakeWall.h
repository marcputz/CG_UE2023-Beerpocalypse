#pragma once

#include "../NewGameObject.h"
#include "../Button/Button.h"

class FakeWall : public NewGameObject, public Activatable {
private:

public:
	FakeWall(MyShader* shader, PxPhysics* physics);

	// Geerbt über NewGameObject
	virtual void onBeforeUpdate() override;

	virtual void onUpdate(float deltaTime) override;

	virtual void resetSpecifics() override;

	virtual void processWindowInput(GLFWwindow* window, float deltaTime) override;

	virtual void processMouseInput(float offsetX, float offsetY) override;

	virtual void onCollision(NewGameObject* otherObject) override;

	virtual void onHealthChange(int oldHealth, int newHealth) override;


	// Geerbt über Activatable
	virtual void onActivate() override;

	virtual void onDeactivate() override;

};