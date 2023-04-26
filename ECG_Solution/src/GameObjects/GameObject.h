#pragma once

#include <stdio.h>
#include <sstream>
#include "../Utils.h"
#include "../Shader.h"
#include "../Geometry.h"
#include "../Texture.h"
#include "../MyModel.h"
#include "../MyTransform.h"
#include "GameObjectInfo.h"
#include <PxPhysicsAPI.h>

using namespace physx;
using namespace std;

class GameObject {
/* VARIABLES */
protected:

	// Parent-Child variables
	std::list<GameObject*> children_;
	GameObject* parent_ = nullptr;

	// Vertex-geometry
	MyModel* model_;
	PxVec3 scale_ = PxVec3(1, 1, 1);
	
	// Shader
	MyShader* shader_;

	// Physics
	PxRigidActor* actor_;
	GameObjectActorType actorType_;

public:

//---------------------------------------------------------------
/* FUNCTIONS */
protected:
	// Constructor
	explicit GameObject(MyShader& shader, PxPhysics* physics, GameObjectInfo& goInfo);

	// Game-logic and OpenGL
	void updateChildren(float deltaTime);

public:
	// Parent-Child management functions
	void addChild(GameObject* child);

	// Physics
	PxRigidActor* getActor();
	void setPosition(glm::vec3 pos);
	void setPosition(physx::PxVec3 pos);
	void setPosition(float x, float y, float z);
	PxVec3 getPosition();
	void move(physx::PxVec3 moveVector);

	// Game-logic and OpenGL
	virtual void update(float deltaTime) = 0;
	virtual void handleInput(GLFWwindow* window, float deltaTime) = 0;
	void draw();
};

