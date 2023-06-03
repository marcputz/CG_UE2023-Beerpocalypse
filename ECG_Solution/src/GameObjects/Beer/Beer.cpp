#include "Beer.h"

Beer::Beer(MyShader* shader, PxPhysics* physics) 
	: NewGameObject("Beer", shader, physics, "beer/14043_16_oz._Beer_Bottle_v1_l3.obj", glm::vec3(0.5, 0.5, 0.1), false, true)
{
	setScale(glm::vec3(0.07f, 0.07f, 0.07f));

	physicsActor_->detachShape(*physicsShape_);
	physicsShape_ = physics_->createShape(PxBoxGeometry(3.0f * transform_->getScale().x, 3.0f * transform_->getScale().y, 30.0f * transform_->getScale().z), *physicsMaterial_, true);
	physicsShape_->userData = this;
	PxFilterData collissionFilterData;
	collissionFilterData.word0 = (1 << 2); // Own ID
	collissionFilterData.word1 = (1 << 1); // ID's to collide with
	physicsShape_->setSimulationFilterData(collissionFilterData);
	physicsActor_->attachShape(*physicsShape_);

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
		static int animationFrame = 0;
		static float rotationAngle = 0;
		static const int totalAnimationFrames = 120;

		// first correct the rotation of the model to make the bottle stand up
		glm::quat modelCorrection = glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

		// turn the bottle a little to the side
		glm::quat sidewaysTurn = glm::quat(glm::vec3(glm::radians(15.0f), 0.0f, 0.0f));

		// spin the bottle around the Y axis
		rotationAngle = ((float) animationFrame / (float) totalAnimationFrames) * 360.0f;
		glm::quat rotation = asGlmQuat(PxQuat(glm::radians(rotationAngle), PxVec3(0, 1, 0)));

		// prepare next rotation
		animationFrame++;
		if (animationFrame >= totalAnimationFrames) {
			animationFrame = 0;
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

