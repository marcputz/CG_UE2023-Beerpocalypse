#pragma once
#include "../GameObject.h"
#include <MyFPSCamera.h>

class Player : public GameObject {
protected:
	MyFPSCamera* camera_;

public:
	void update(float deltaTime);
	void handleInput(GLFWwindow* window, float deltaTime);

	// Constructors
	Player(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo, MyFPSCamera& camera) : GameObject(shader, physics, goInfo) {
		camera_ = &camera;

		PxMaterial* material = physics->createMaterial(goInfo.staticFriction, goInfo.dynamicFriction, goInfo.restitution);
		PxShape* shape = physics->createShape(PxBoxGeometry(1, 2, 1), *material);
		actor_ = physics->createRigidDynamic(PxTransform(goInfo.location));
		actor_->attachShape(*shape);
		actorType_ = TYPE_DYNAMIC;
	}
};

