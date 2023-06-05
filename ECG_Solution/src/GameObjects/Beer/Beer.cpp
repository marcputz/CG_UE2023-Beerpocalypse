#include "Beer.h"

Beer::Beer(MyShader* shader, PxPhysics* physics) 
	: NewGameObject("Beer", shader, physics, "beer/14043_16_oz._Beer_Bottle_v1_l3.obj", false)
{
	setScale(glm::vec3(0.07f, 0.07f, 0.07f));

	PxMaterial* material = physics->createMaterial(1, 1, 0.01f);
	PxBoxGeometry geometry = PxBoxGeometry(3, 3, 30);
	PxShape* collider = physics->createShape(geometry, *material);
	PxFilterData collissionFilterData;
	collissionFilterData.word0 = CollisionLayer::LAYER_COLLECTABLES; // Own ID
	collissionFilterData.word1 = CollisionLayer::LAYER_PLAYER; // IDs to do collision callback with
	collider->setSimulationFilterData(collissionFilterData);

	setCollider(collider);

	PxRigidBody* body = static_cast<PxRigidBody*>(physicsActor_);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	glm::quat rotation = glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
	setLocalRotation(rotation);
}

void Beer::onBeforeUpdate()
{
}

void Beer::onUpdate(float deltaTime)
{
	if (animateRotation) {

		// first correct the rotation of the model to make the bottle stand up
		glm::quat modelCorrection = glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

		// turn the bottle a little to the side
		glm::quat sidewaysTurn = glm::quat(glm::vec3(glm::radians(15.0f), 0.0f, 0.0f));

		// spin the bottle around the Y axis
		rotationAngle = (animationTime / totalAnimationTime) * 360.0f;
		glm::quat rotation = asGlmQuat(PxQuat(glm::radians(rotationAngle), PxVec3(0, 1, 0)));

		// prepare next rotation
		animationTime += deltaTime;
		if (animationTime >= totalAnimationTime) {
			animationTime = 0.0f;
		}

		glm::quat newRotation = rotation * (modelCorrection * sidewaysTurn);
		setLocalRotation(newRotation);
	}
}

void Beer::processWindowInput(GLFWwindow* window, float deltaTime)
{
}

void Beer::processMouseInput(float offsetX, float offsetY)
{
}

void Beer::onCollision(NewGameObject* otherObject)
{
}

void Beer::onHealthChange(int oldHealth, int newHealth)
{
}

