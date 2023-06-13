#include "Scene.h"
#include "GameObjects/Zombie/Zombie.h"
#include "GameObjects/Cube/DynamicCube.h"
#include "GameObjects/Button/Button.h"

extern unsigned int screenWidth;
extern unsigned int screenHeight;
extern float cameraNear;
extern float cameraFar;
extern int bullets;

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize) {
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eNOTIFY_CONTACT_POINTS | PxPairFlag::eDETECT_DISCRETE_CONTACT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	} else {
		pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
	}

	return physx::PxFilterFlag::eDEFAULT;
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

void Scene::addObject(GameObject* gameObject, bool addToPhysics) {
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

void Scene::setParticleGenerator(MyParticleGenerator* newParticleGenerator) {
	this->particleGenerator = newParticleGenerator;
}

void Scene::startPlayerInteraction() {
	// Let player interact with objects
	static const PxReal maxInteractDistance = PxReal(5.0f);

	// Find player
	GameObject* playerGo = nullptr;
	for (GameObject* go : objects) {
		if (go->name_ == "Player") {
			playerGo = go;
			break;
		}
	}

	if (playerGo != nullptr) {
		Player* player = static_cast<Player*>(playerGo);

		// Define Ray
		PxVec3 rayOrigin = asPxVec3(player->getWorldPosition());
		PxVec3 rayDirection;
		const PxU32 hitBufferSize = 32;
		PxRaycastHit hitBuffer[hitBufferSize];
		PxRaycastBuffer buf(hitBuffer, hitBufferSize);

		// If first person camera, use camera direction, if third person, use player forward vector
		if (player->getActiveCameraType() == PlayerCameraType::CAMERA_FIRST_PERSON) {
			rayDirection = asPxVec3(player->getActiveCamera()->getDirection()).getNormalized();
		}
		else {
			rayDirection = asPxVec3(player->getForwardVector()).getNormalized();
		}

		// Make Raycast for player interaction
		bool raycastStatus = physicsScene->raycast(rayOrigin, rayDirection, maxInteractDistance, buf);
		if (raycastStatus) {
			// raycast has hit something
			for (PxU32 i = 0; i < buf.nbTouches; i++) {
				PxRaycastHit currentHit = buf.touches[i];
				GameObject* object = static_cast<GameObject*>(currentHit.actor->userData);
				if (object != nullptr) {
					// Raycast hit game object
					// Skip player as it is always hit
					Player* player = dynamic_cast<Player*>(object);
					if (player == nullptr) {
						// Skip invisble objects
						if (object->isVisible()) {
							//std::cout << "Interacting with '" << object->name_ << "'" << std::endl;

							Button* button = dynamic_cast<Button*>(object);
							if (button != nullptr) {
								// Interacted with button
								button->interact();
							}

							return; // End function, do not deal with other raycast objects
						}
					}
				}
			}
		}
	}
}

void Scene::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
	for (GameObject* go : objects) {
		go->processWindowInput(window, deltaTime);
	}
}

void Scene::handleMouseInput(float xOffset, float yOffset) {
	for (GameObject* go : objects) {
		go->processMouseInput(xOffset, yOffset);
	}
}

void Scene::handleMouseButtonInput(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// player wants to shoot

		if (bullets-- <= 0) {
			bullets = 0;
			return;
		}

		MyAssetManager::playSound("gunshot");

		// make a raycast and see what you've hit
		static const PxReal maxShootDistance = 20.0f;

		// Find player
		GameObject* playerGo = nullptr;
		for (GameObject* go : objects) {
			if (go->name_ == "Player") {
				playerGo = go;
				break;
			}
		}

		if (playerGo != nullptr) {
			Player* player = static_cast<Player*>(playerGo);

			// Define Ray
			PxVec3 rayOrigin = asPxVec3(player->getWorldPosition());
			PxVec3 rayDirection;
			const PxU32 hitBufferSize = 32;
			PxRaycastHit hitBuffer[hitBufferSize];
			PxRaycastBuffer buf(hitBuffer, hitBufferSize);

			// If first person camera, use camera direction, if third person, use player forward vector
			if (player->getActiveCameraType() == PlayerCameraType::CAMERA_FIRST_PERSON) {
				rayDirection = asPxVec3(player->getActiveCamera()->getDirection()).getNormalized();
			} else {
				rayDirection = asPxVec3(player->getForwardVector()).getNormalized();
			}

			// Make Raycast for player gunshot
			bool raycastStatus = physicsScene->raycast(rayOrigin, rayDirection, maxShootDistance, buf);
			if (raycastStatus) {
				// Raycast has hit something
				for (PxU32 i = 0; i < buf.nbTouches; i++) {
					PxRaycastHit currentHit = buf.touches[i];
					GameObject* object = static_cast<GameObject*>(currentHit.actor->userData);
					if (object != nullptr) {
						// Raycast hit game object
						// Skip player as it is always hit
						Player* player = dynamic_cast<Player*>(object);
						if (player == nullptr) {
							//std::cout << "Hit '" << object->name_ << "'" << std::endl;

							// If object is zombie, deal damage
							Zombie* zombie = dynamic_cast<Zombie*>(object);
							if (zombie != nullptr) {
								if (zombie->isVisible()) {
									zombie->setHealth(zombie->getHealth() - 40);

									if (particleGenerator != nullptr) {
										particleGenerator->createParticles(
											asGlmVec3(currentHit.position),
											(asGlmVec3(rayDirection) * -1.0f), ParticleType::ZOMBIE_BLOOD, 1.0f, 5, true, nullptr);
									}

									return; // skip the rest of this function, as only the first hit should be processed.
								} else {
									// zombie is invisible, so skip it (most likely it was defeated before)
								}
							} else {
								// if object is a dynamic cube, set some force to push the cube
								DynamicCube* cube = dynamic_cast<DynamicCube*>(object);
								if (cube != nullptr) {
									PxRigidDynamic* dyn = static_cast<PxRigidDynamic*>(cube->getRigidActor());
									dyn->addForce(rayDirection * 3.0, PxForceMode::eIMPULSE);
									cube->setHealth(cube->getHealth() - 30);
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

void Scene::step(float deltaTime) {
	// Make pre-update calls
	for (GameObject* go : objects) {
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
	for (GameObject* go : objects) {
		go->update(deltaTime);

		// player ground check via raycast, also creates frustum
		{
			Player* p = dynamic_cast<Player*>(go);
			if (p != nullptr) {
				//frustum = Frustum::createFrustumFromCamera(*p->getActiveCamera(), (float)screenWidth/(float)screenHeight, glm::radians(p->getActiveCamera()->getFov()), cameraNear, cameraFar);

				PxVec3 rayOrigin = asPxVec3(p->getWorldPosition());
				PxVec3 rayDirection = asPxVec3(glm::vec3(0.0f, -1.0f, 0.0f));
				const PxU32 hitBufferSize = 32;
				PxRaycastHit hitBuffer[hitBufferSize];
				PxRaycastBuffer buf(hitBuffer, hitBufferSize);
				bool raycastStatus = physicsScene->raycast(rayOrigin, rayDirection, 1.01f, buf);
				if (raycastStatus) {
					// Raycast has hit something
					for (PxU32 i = 0; i < buf.nbTouches; i++) {
						PxRaycastHit currentHit = buf.touches[i];
						GameObject* object = static_cast<GameObject*>(currentHit.actor->userData);
						if (object != nullptr) {
							// Raycast hit game object
							// Skip player as it is always hit
							Player* player = dynamic_cast<Player*>(object);
							Zombie* zombie = dynamic_cast<Zombie*>(object);
							Beer* beer = dynamic_cast<Beer*>(object);
							if (player == nullptr && zombie == nullptr && beer == nullptr) {
								//std::cout << "Hit '" << object->name_ << "'" << std::endl;

								p->setIsOnGround(true);
							}
						}
					}
				}
			}
		}
	}

	if (particleGenerator != nullptr) {
		particleGenerator->update(deltaTime);
	}
}

void Scene::draw() {
	for (GameObject* go : objects) {
		go->draw();
		/*
		if (go->isInsideFrustum(frustum)) {
			go->draw();
		}
		*/
	}

	if (particleGenerator != nullptr) {
		particleGenerator->draw();
	}
}


void Scene::reset() {
	for (GameObject* go : objects) {
		go->reset();
	}
}

/* --- SIMULATION CALLBACKS --- */

void Scene::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {

}

void Scene::onWake(PxActor** actors, PxU32 count) {

}

void Scene::onSleep(PxActor** actors, PxU32 count) {

}

PxScene* Scene::getPhysicsScene() {
	return physicsScene;
}

void Scene::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	//std::printf("onContact\n");

	for (physx::PxU32 i = 0; i < nbPairs; i++) {
		const physx::PxContactPair& cp = pairs[i];
		PxShape* shapeOne = cp.shapes[0];
		PxShape* shapeTwo = cp.shapes[1];
		GameObject* objectOne = static_cast<GameObject*>(shapeOne->userData);
		GameObject* objectTwo = static_cast<GameObject*>(shapeTwo->userData);

		if (objectOne != objectTwo) {
			//std::cout << "Collision: '" << objectOne->name_ << "' collided with '" << objectTwo->name_ << "'" << std::endl;

			objectOne->onCollision(objectTwo);
			objectTwo->onCollision(objectOne);
		}
	}

	return;
}

void Scene::onTrigger(PxTriggerPair* pairs, PxU32 count) {

}

void Scene::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {

}

physx::PxVec3 Scene::asPxVec3(glm::vec3 vec) {
	return PxVec3(vec.x, vec.y, vec.z);
}

glm::vec3 Scene::asGlmVec3(physx::PxVec3 vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}