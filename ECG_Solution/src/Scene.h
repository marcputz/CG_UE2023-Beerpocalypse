#pragma once

#include "GameObjects/NewGameObject.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "Lights/PointLight/MyPointLight.h"
#include "Lights/SpotLight/MySpotLight.h"
#include "Lights/DirectionalLight/MyDirectionalLight.h"
#include "MyParticleGenerator.h"

class Scene : public PxSimulationEventCallback {
private:
	vector<NewGameObject*> objects = {};

	MyParticleGenerator* particleGenerator;
	MyDirectionalLight* directionLight;
	vector<MyPointLight*> pointLights = {};
	vector<MySpotLight*> spotLights = {};

	PxPhysics* physics;
	PxScene* physicsScene;

private:
	// helper functions
	physx::PxVec3 asPxVec3(glm::vec3 vec);
	glm::vec3 asGlmVec3(physx::PxVec3 vec);

public:
	Scene(PxPhysics* physics);

	PxScene* getPhysicsScene();

	void addObject(NewGameObject* gameObject, bool addToPhysics = true);
	void addLight(MyDirectionalLight* dirLight);
	void addLight(MyPointLight* pointLight);
	void addLight(MySpotLight* spotLight);
	void setParticleGenerator(MyParticleGenerator* newParticleGenerator);

	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);
	void handleMouseButtonInput(GLFWwindow* window, int button, int action, int mods);

	void step(float deltaTime);
	void draw();
	void reset();

	// Geerbt über PxSimulationEventCallback
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	virtual void onWake(PxActor** actors, PxU32 count) override;
	virtual void onSleep(PxActor** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;
};