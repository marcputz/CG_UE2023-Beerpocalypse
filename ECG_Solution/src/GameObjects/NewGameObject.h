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

enum CollisionLayer : PxU32 {
	LAYER_DEFAULT = (1 << 0),
	LAYER_PLAYER = (1 << 1),
	LAYER_COLLECTABLES = (1 << 2),
	LAYER_ENEMIES = (1 << 3)
};

class NewGameObject {
/* VARIABLES */
private:
	bool isStatic_ = false;
	bool isVisible_ = true;

	int health = 100;

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

	const int maxHealth = 100;

/* FUNCTIONS */
private:

	/**
	* This function takes the physics transform and copies it into the game object's transform
	*/
	void synchronizeTransforms();

protected:
	// Constructors
	explicit NewGameObject(string name, MyShader* shader, PxPhysics* physics, string modelPath, bool isStatic = false);
	NewGameObject(NewGameObject&&) = delete; // move constructor
	NewGameObject(const NewGameObject&) = delete; // copy constructor

	// operators
	NewGameObject& NewGameObject::operator=(const NewGameObject&) = delete;
	bool operator==(const NewGameObject&) const = delete;

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
	void setWorldPosition(glm::vec3 newPosition);
	void setScale(glm::vec3 newScale);
	glm::vec3 getLocalPosition();
	glm::quat getLocalRotation();
	glm::vec3 getWorldPosition();
	glm::quat getWorldRotation();
	glm::vec3 getScale();
	glm::vec3 getForwardVector();

	MyModel* getModel();
	void setAnimator(MyAnimator& newAnimator);

	void setCollider(PxShape* colliderShape);
	PxShape* getCollider() { return physicsShape_; }
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

	void setHealth(int newHealth);
	int getHealth();
	virtual void onHealthChange(int oldHealth, int newHealth) = 0;

	/**
	* draws object on screen
	*/
	void draw();
};
