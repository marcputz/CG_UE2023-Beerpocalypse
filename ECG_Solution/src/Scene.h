#pragma once

#include "GameObjects/NewGameObject.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "Lights/PointLight/MyPointLight.h"
#include "Lights/SpotLight/MySpotLight.h"
#include "Lights/DirectionalLight/MyDirectionalLight.h"

class Scene : public PxSimulationEventCallback {
private:
	vector<NewGameObject*> objects = {};

	MyDirectionalLight* directionLight;
	vector<MyPointLight*> pointLights = {};
	vector<MySpotLight*> spotLights = {};

	PxPhysics* physics;
	PxScene* physicsScene;

public:
	Scene(PxPhysics* physics);


	void addObject(NewGameObject* gameObject, bool addToPhysics = true);
	void addLight(MyDirectionalLight* dirLight);
	void addLight(MyPointLight* pointLight);
	void addLight(MySpotLight* spotLight);

	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);

	void step(float deltaTime);
	void draw();

	// Geerbt über PxSimulationEventCallback
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	virtual void onWake(PxActor** actors, PxU32 count) override;
	virtual void onSleep(PxActor** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;
};