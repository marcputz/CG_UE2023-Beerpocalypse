#include "NewGameObject.h"

NewGameObject::NewGameObject(string name, MyShader* shader, PxPhysics* physics, string modelPath, glm::vec3 materialAttributes, bool isStatic, bool useAdvancedCollissionDetection)
	: shader_{ shader },
	name_{ name },
	physics_{ physics },
	transform_{ new Transform(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1)) }
{
	model_ = new MyModel(modelPath);

	if (isStatic) {
		physicsActor_ = physics_->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
	}
	else {
		physicsActor_ = physics_->createRigidDynamic(PxTransform(PxVec3(0, 0, 0)));
	}
	this->isStatic_ = isStatic;

	physicsMaterial_ = physics->createMaterial(materialAttributes.x, materialAttributes.y, materialAttributes.z);
	//physicsShape_ = physics->createShape(PxBoxGeometry(1, 1, 1), *physicsMaterial_);
	float boundingBoxX = model_->boundingBox_.absDiff.x / 2.0f;
	float boundingBoxY = model_->boundingBox_.absDiff.y / 2.0f;
	float boundingBoxZ = model_->boundingBox_.absDiff.z / 2.0f;
	//std::cout << boundingBoxX << "/" << boundingBoxY << "/" << boundingBoxZ << std::endl;
	physicsShape_ = physics_->createShape(PxBoxGeometry(boundingBoxX * transform_->getScale().x, boundingBoxY * transform_->getScale().y, boundingBoxZ * transform_->getScale().z), *physicsMaterial_, true);

	PxFilterData collissionFilterData;
	if (useAdvancedCollissionDetection) {
		collissionFilterData.word0 = (1 << 2); // Own ID
		collissionFilterData.word1 = (1 << 1); // ID's to collide with
	}
	else {
		collissionFilterData.word0 = (1 << 1); // Own ID
		collissionFilterData.word1 = (1 << 2); // ID's to collide with
	}
	physicsShape_->setSimulationFilterData(collissionFilterData);

	physicsActor_->attachShape(*physicsShape_);

	physicsMaterial_->userData = this;
	physicsShape_->userData = this;
	physicsActor_->userData = this;
}

void NewGameObject::synchronizeTransforms() {
	// Get the transform coordinates of the physics object and put them into the object's transform
	PxTransform physicsTransform = physicsActor_->getGlobalPose();
	transform_->setWorldPosition(asGlmVec3(physicsTransform.p));
	transform_->setWorldRotation(asGlmQuat(physicsTransform.q));
}

void NewGameObject::setParent(NewGameObject* newParent) {
	if (newParent != nullptr) {
		this->transform_->setParent(newParent->transform_);
	}
	else {
		this->transform_->setParent(nullptr);
	}
}

void NewGameObject::setLocalPosition(glm::vec3 newPosition) {
	this->transform_->setLocalPosition(newPosition);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.p = asPxVec3(this->transform_->getWorldPosition());
	physicsActor_->setGlobalPose(transf);
}

void NewGameObject::setLocalRotation(glm::quat newRotation) {
	this->transform_->setLocalRotation(newRotation);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.q = asPxQuat(this->transform_->getLocalRotation());
	physicsActor_->setGlobalPose(transf);
}

void NewGameObject::setScale(glm::vec3 newScale) {
	this->transform_->setScale(newScale);

	// Update Bounding boxes to use new scale
	physicsActor_->detachShape(*physicsShape_);
	physicsShape_->release();
	float boundingBoxX = model_->boundingBox_.absDiff.x / 2.0f;
	float boundingBoxY = model_->boundingBox_.absDiff.y / 2.0f;
	float boundingBoxZ = model_->boundingBox_.absDiff.z / 2.0f;
	physicsShape_ = physics_->createShape(PxBoxGeometry(boundingBoxX * transform_->getScale().x, boundingBoxY * transform_->getScale().y, boundingBoxZ * transform_->getScale().z), *physicsMaterial_, true);
	//physicsShape_ = physics_->createShape(PxBoxGeometry(1, 1, 1), *physicsMaterial_);
	physicsShape_->userData = this;
	physicsActor_->attachShape(*physicsShape_);
}

glm::vec3 NewGameObject::getLocalPosition() {
	return this->transform_->getLocalPosition();
}

glm::quat NewGameObject::getLocalRotation() {
	return this->transform_->getLocalRotation();
}

glm::vec3 NewGameObject::getWorldPosition() {
	return this->transform_->getWorldPosition();
}

glm::quat NewGameObject::getWorldRotation() {
	return this->transform_->getWorldRotation();
}

glm::vec3 NewGameObject::getScale() {
	return this->transform_->getScale();
}

glm::vec3 NewGameObject::getForwardVector() {
	return this->transform_->getForwardVector();
}

MyModel* NewGameObject::getModel() {
	return this->model_;
}

/**
* This update function must be called BEFORE the physics update
*/
void NewGameObject::beforeUpdate() {
	// Call custom code
	onBeforeUpdate();
}

/**
* This update function must be called AFTER the physics update
*/
void NewGameObject::update(float deltaTime) {
	// synchronize physcis transform into object's transform
	synchronizeTransforms();

	// Call custom code
	onUpdate(deltaTime);
}

//virtual void handleKeyboardInput(GLFWwindow* window, float deltaTime) = 0;
//virtual void handleMouseInput(float xOffset, float yOffset) = 0;

void NewGameObject::draw() {
	if (!isVisible_) {
		return;
	}

	glm::vec3 position = transform_->getWorldPosition();
	// TODO: Replace with world rotation and check if it's working
	glm::quat rotation = transform_->getLocalRotation();
	glm::vec3 scale = transform_->getScale();

	glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = transMatrix * rotMatrix * scaleMatrix;

	shader_->use();
	shader_->setMat4("model", modelMatrix);
	model_->draw(*shader_);
}

glm::vec3 NewGameObject::asGlmVec3(physx::PxVec3 vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

physx::PxVec3 NewGameObject::asPxVec3(glm::vec3 vec) {
	return physx::PxVec3(vec.x, vec.y, vec.z);
}

glm::quat NewGameObject::asGlmQuat(physx::PxQuat quat) {
	return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

physx::PxQuat NewGameObject::asPxQuat(glm::quat quat) {
	return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}