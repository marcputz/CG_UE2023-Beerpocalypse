#pragma once

#include "../NewGameObject.h"

class Activatable {
private:
	Activatable(Activatable&&) = delete;
	Activatable(const Activatable&) = delete;
public:
	Activatable() {}
	~Activatable() {}

	virtual void onActivate() = 0;
	virtual void onDeactivate() = 0;
};

class Button : public NewGameObject {
private:
	MyModel* greenButtonModel;
	MyModel* redButtonModel;

	bool isActivated = false;

	Activatable* linkedObject_ = nullptr;

public:
	Button(MyShader* shader, PxPhysics* physics, Activatable* objectToActivate);
	~Button();

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