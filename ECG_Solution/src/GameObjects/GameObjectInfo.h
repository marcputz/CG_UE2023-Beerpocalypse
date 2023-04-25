#pragma once

#include <string>
#include <PxPhysicsAPI.h>

using namespace std;
using namespace physx;

enum GameObjectActorType {
	TYPE_STATIC,
	TYPE_DYNAMIC
};

struct GameObjectInfo {
	string modelPath;
	GameObjectActorType actorType;
	PxVec3 location;
	float staticFriction = 0;
	float dynamicFriction = 0;
	float restitution = 0;
};