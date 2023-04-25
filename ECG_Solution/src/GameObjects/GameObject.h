#pragma once

#include <stdio.h>
#include <sstream>
#include "Utils.h"
#include "Shader.h"
#include "Geometry.h"
#include "Texture.h"
#include "MyModel.h"
#include "MyTransform.h"
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
	
	// Shader
	MyShader* shader_;

	// Physics
	PxRigidActor* actor_ = nullptr;

public:

//---------------------------------------------------------------
/* FUNCTIONS */
protected:
	// Constructor
	explicit GameObject(const std::string& path, MyShader& shader);

	// Game-logic and OpenGL
	void updateChildren(float deltaTime);

public:
	// Parent-Child management functions
	void addChild(GameObject* child);

	// Physics
	PxRigidActor* getActor();

	// Game-logic and OpenGL
	virtual void update(float deltaTime) = 0;
	void draw();
	void setPosition(glm::vec3 pos);
	void setPosition(physx::PxVec3 pos);
	virtual void setPosition(float x, float y, float z) = 0;
};

