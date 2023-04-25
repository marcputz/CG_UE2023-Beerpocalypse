#include "MyFPSCamera.h"

MyFPSCamera::MyFPSCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: front_(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed_(SPEED), lookSensitivity_(SENSITIVITY), fov_(FOV) {
	position_ = position;
	worldUp_ = up;
	yaw_ = yaw;
	pitch_ = pitch;

	updateCamVectors();
}

MyFPSCamera::MyFPSCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: front_(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed_(SPEED), lookSensitivity_(SENSITIVITY), fov_(FOV) {
	position_ = glm::vec3(posX, posY, posZ);
	worldUp_ = glm::vec3(upX, upY, upZ);
	yaw_ = yaw;
	pitch_ = pitch;

	updateCamVectors();
}

glm::mat4 MyFPSCamera::getViewMatrix() {
	return glm::lookAt(position_, position_ + front_, up_);
}

void MyFPSCamera::processKeyboardInput(Camera_Movement_Direction direction, float deltaTime) {
	float velocity = movementSpeed_ * deltaTime;
	if (direction == FORWARD) {
		position_ += front_ * velocity;
	}
	if (direction == BACKWARD) {
		position_ -= front_ * velocity;
	}
	if (direction == LEFT) {
		position_ -= right_ * velocity;
	}
	if (direction == RIGHT) {
		position_ += right_ * velocity;
	}
	// force camera to stay "at ground level"
	//position_.y = 0.0f;
}

void MyFPSCamera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
	xOffset *= lookSensitivity_;
	yOffset *= lookSensitivity_;

	yaw_ += xOffset;
	pitch_ += yOffset;

	// so we can't flip the world by looking up/down
	if (constrainPitch) {
		if (pitch_ > 89.0f) {
			pitch_ = 89.0f;
		}
		if (pitch_ < -89.0f) {
			pitch_ = -89.0f;
		}
	}

	updateCamVectors();
}

void MyFPSCamera::processMouseScrolling(float yOffset) {
	fov_ -= (float)yOffset;
	if (fov_ < 20.0f) {
		fov_ = 20.0f;
	}
	if (fov_ > 90.0f) {
		fov_ = 90.0f;
	}
}

void MyFPSCamera::attachToSubject(GameObject* subject) {
	subject_ = subject;
}

bool MyFPSCamera::isAttached() {
	return subject_ != nullptr;
}

void MyFPSCamera::detach() {
	subject_ = nullptr;
}

void MyFPSCamera::update() {
	if (subject_) {
		glm::vec3 subjectPos(subject_->getActor()->getGlobalPose().p.x, subject_->getActor()->getGlobalPose().p.y, subject_->getActor()->getGlobalPose().p.z);
		glm::vec3 combinedPos = subjectPos + relativePosition_;
		position_ = combinedPos;
	}
}

// recalculate front, right and up vectors
void MyFPSCamera::updateCamVectors() {
	/*
	if (subject_) {
		glm::vec3 front;
		PxQuat q = subject_->getActor()->getGlobalPose().q;
		
		float pitch = asin(2.0 * (q.y * q.w - q.z + q.x));
		float yaw = atan2(2.0 * (q.z * q.w + q.x * q.y), -1.0 + 2.0 * (q.w * q.w + q.x * q.x));
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front_ = glm::normalize(front);

		right_ = glm::normalize(glm::cross(front_, worldUp_));
		up_ = glm::normalize(glm::cross(right_, front_));
	} else {
		glm::vec3 front;
		front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		front.y = sin(glm::radians(pitch_));
		front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		front_ = glm::normalize(front);

		right_ = glm::normalize(glm::cross(front_, worldUp_));
		up_ = glm::normalize(glm::cross(right_, front_));
	}
	*/
	glm::vec3 front;
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_ = glm::normalize(front);

	right_ = glm::normalize(glm::cross(front_, worldUp_));
	up_ = glm::normalize(glm::cross(right_, front_));
}
