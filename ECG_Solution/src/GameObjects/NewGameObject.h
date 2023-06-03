#pragma once

#include <stdio.h>
#include <sstream>
#include "../MyModel.h"
#include "GameObjectInfo.h"
#include <PxPhysicsAPI.h>
#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include "../MyAnimator.h"


class NewGameObject {
/* VARIABLES */
private:
	bool isStatic_ = false;
	bool isVisible_ = true;

protected:

	// Transform (Position, Rotation, etc.)
	Transform* transform_;

	// Model data
	MyModel* model_;

	// Animator of the gameObject
	MyAnimator* animator_;

	// Shader
	MyShader* shader_;

	// Physics
	PxPhysics* physics_;
	PxRigidActor* physicsActor_;
	PxMaterial* physicsMaterial_;
	PxShape* physicsShape_;

public:
	string name_;

/* FUNCTIONS */
private:

	/**
	* This function takes the physics transform and copies it into the game object's transform
	*/
	void synchronizeTransforms();

protected:
	// Constructor
	explicit NewGameObject(string name, MyShader* shader, PxPhysics* physics, string modelPath, glm::vec3 materialAttributes, bool isStatic, bool useAdvancedCollissionDetection = false);

	/**
	* These functions can be used by this class' derivates to define their functionality
	*/
	virtual void onBeforeUpdate() = 0;
	virtual void onUpdate(float deltaTime) = 0;

	// Helper Functions
	glm::vec3 asGlmVec3(physx::PxVec3 vec);
	physx::PxVec3 asPxVec3(glm::vec3 vec);
	glm::quat asGlmQuat(physx::PxQuat quat);
	physx::PxQuat asPxQuat(glm::quat quat);

public:
	void setParent(NewGameObject* newParent);

	bool isStatic() { return isStatic_; }

	bool isVisible() { return isVisible_; }
	void setVisible(bool isVisible) { isVisible_ = isVisible; }

	void setLocalPosition(glm::vec3 newPosition);
	void setLocalRotation(glm::quat newRotation);
	void setScale(glm::vec3 newScale);
	glm::vec3 getLocalPosition();
	glm::quat getLocalRotation();
	glm::vec3 getWorldPosition();
	glm::quat getWorldRotation();
	glm::vec3 getScale();
	glm::vec3 getForwardVector();

	MyModel* getModel();
	void setAnimator(MyAnimator& newAnimator);

	PxRigidActor* getRigidActor() { return this->physicsActor_; }

	/**
	* This update function must be called BEFORE the physics update
	*/
	void beforeUpdate();

	/**
	* This update function must be called AFTER the physics update
	*/
	void update(float deltaTime);

	/**
	* This function can be called from outside to handle keyboard (and window) inputs
	**/
	virtual void processWindowInput(GLFWwindow* window, float deltaTime) = 0;
	virtual void processMouseInput(float offsetX, float offsetY) = 0;


	virtual void onCollision(NewGameObject* otherObject) = 0;

	//virtual void handleKeyboardInput(GLFWwindow* window, float deltaTime) = 0;
	//virtual void handleMouseInput(float xOffset, float yOffset) = 0;

	/**
	* draws object on screen
	*/
	void draw();
};
