#pragma once

#include <stdio.h>
#include <sstream>
#include "../MyModel.h"
#include <PxPhysicsAPI.h>
#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include "../MyAnimator.h"
#include "../MyFrustum.h"

using namespace physx;
using namespace std;

enum CollisionLayer : PxU32 {
	LAYER_DEFAULT = (1 << 0),
	LAYER_PLAYER = (1 << 1),
	LAYER_COLLECTABLES = (1 << 2),
	LAYER_ENEMIES = (1 << 3)
};

class GameObject {
/* VARIABLES */
private:
	bool isStatic_ = false;
	bool isVisible_ = true;

	int health = 100;

protected:

	// Transform (Position, Rotation, etc.)
	Transform* transform_;
	glm::vec3 originalPosition;
	bool hasOriginalPosition = false;

	// Model data
	MyModel* model_;

	// Animator of the gameObject
	MyAnimator* animator_;

	// Shader
	MyShader* shader_;

	// Physics
	PxPhysics* physics_;
	PxRigidActor* physicsActor_;
	PxShape* physicsShape_;
	PxMaterial* physicsMaterial_;

	bool highlighted_ = false;
	glm::vec3 highlightColor_ = glm::vec3(1.0f, 1.0f, 1.0f);
	float highlightDuration_ = 0.0f;
	float remainingHighlightDuration_ = 0.0f;

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
	explicit GameObject(string name, MyShader* shader, PxPhysics* physics, string modelPath, bool isStatic = false);
	GameObject(GameObject&&) = delete; // move constructor
	GameObject(const GameObject&) = delete; // copy constructor

	// operators
	GameObject& GameObject::operator=(const GameObject&) = delete;
	bool operator==(const GameObject&) const = delete;

	/**
	* These functions can be used by this class' derivates to define their functionality
	*/
	virtual void onBeforeUpdate() = 0;
	virtual void onUpdate(float deltaTime) = 0;
	virtual void resetSpecifics() = 0;

	// Helper Functions
	glm::vec3 asGlmVec3(physx::PxVec3 vec);
	physx::PxVec3 asPxVec3(glm::vec3 vec);
	glm::quat asGlmQuat(physx::PxQuat quat);
	physx::PxQuat asPxQuat(glm::quat quat);

public:
	void setParent(GameObject* newParent);

	bool isStatic() { return isStatic_; }

	bool isVisible() { return isVisible_; }
	void setVisible(bool isVisible) { isVisible_ = isVisible; }

	void enableCollider(bool enabled);

	void setLocalPosition(glm::vec3 newPosition);
	void setLocalRotation(glm::quat newRotation);
	void setWorldPosition(glm::vec3 newPosition);
	void setScale(glm::vec3 newScale, bool applyTilingScale = false);
	glm::vec3 getLocalPosition();
	glm::quat getLocalRotation();
	glm::vec3 getWorldPosition();
	glm::quat getWorldRotation();
	glm::vec3 getScale();
	glm::vec3 getForwardVector();

	MyModel* getModel();
	void setAnimator(MyAnimator& newAnimator);

	void setCollider(PxGeometry* geometry, PxMaterial* material);
	PxRigidActor* getRigidActor() { return this->physicsActor_; }

	void reset();

	bool isInsideFrustum(const Frustum& frustum);

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

	virtual void onCollision(GameObject* otherObject) = 0;

	void setHealth(int newHealth);
	int getHealth();
	virtual void onHealthChange(int oldHealth, int newHealth) = 0;

	/**
	* draws object on screen
	*/
	void draw();
};
