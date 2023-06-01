#include "Scene.h"

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_CONTACT_POINTS | PxPairFlag::eDETECT_DISCRETE_CONTACT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND; // enable this if you want to persitantly call the callback while the touch is persisting: | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

	return physx::PxFilterFlag::eDEFAULT;

	//  // all initial and persisting reports for everything, with per-point data
	//  pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
	//        |	physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
	//        | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
	//        | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	//  return physx::PxFilterFlag::eNOTIFY; //:eCALLBACK; //physx::PxFilterFlag::eDEFAULT;
}

Scene::Scene(PxPhysics* physics) {

	// Setup Physics Scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxDefaultCpuDispatcher* gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
	sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.filterShader = contactReportFilterShader;

	this->physicsScene = physics->createScene(sceneDesc);
	this->physics = physics;
}

void Scene::addObject(NewGameObject* gameObject, bool addToPhysics) {
	objects.push_back(gameObject);

	if (addToPhysics) {
		physicsScene->addActor(*(gameObject->getRigidActor()));
	}
}

void Scene::addLight(MyDirectionalLight* dirLight) {
	directionLight = dirLight;
}

void Scene::addLight(MyPointLight* pointLight) {
	pointLights.push_back(pointLight);
}

void Scene::addLight(MySpotLight* spotLight) {
	spotLights.push_back(spotLight);
}

void Scene::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
	for (NewGameObject* go : objects) {
		go->processWindowInput(window, deltaTime);
	}
}

void Scene::handleMouseInput(float xOffset, float yOffset) {
	for (NewGameObject* go : objects) {
		go->processMouseInput(xOffset, yOffset);
	}
}

void Scene::step(float deltaTime) {
	// Make pre-update calls
	for (NewGameObject* go : objects) {
		go->beforeUpdate();
	}

	// Make Physics update
	this->physicsScene->simulate(deltaTime);
	this->physicsScene->fetchResults(true);

	// Update Lights
	directionLight->setUniforms(0);

	int pointLightIdx = 0;
	for (MyPointLight* pointLight : pointLights) {
		pointLight->setUniforms(pointLightIdx);
		pointLightIdx++;
	}

	int spotLightIdx = 0;
	for (MySpotLight* spotLight : spotLights) {
		spotLight->setUniforms(spotLightIdx);
		spotLightIdx++;
	}

	// Call update methods
	for (NewGameObject* go : objects) {
		go->update(deltaTime);
	}
}

void Scene::draw() {
	for (NewGameObject* go : objects) {
		go->draw();
	}
}

/* --- SIMULATION CALLBACKS --- */

void Scene::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {

}

void Scene::onWake(PxActor** actors, PxU32 count) {

}

void Scene::onSleep(PxActor** actors, PxU32 count) {

}

void Scene::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	std::printf("Scene::onContact\n");
}

void Scene::onTrigger(PxTriggerPair* pairs, PxU32 count) {

}

void Scene::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {
	
}