#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "GameObjects/GameObject.h"

enum Camera_Movement_Direction {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;

class MyFPSCamera {
public:
	glm::vec3 position_;
	glm::vec3 front_;
	glm::vec3 up_;
	glm::vec3 right_;
	glm::vec3 worldUp_;

	float yaw_;
	float pitch_;

	float movementSpeed_;
	float lookSensitivity_;
	float fov_;

	MyFPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	MyFPSCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 getViewMatrix();
	void processKeyboardInput(Camera_Movement_Direction direction, float deltaTime);
	void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
	void processMouseScrolling(float yOffset);
	void attachToSubject(GameObject* subject);
	bool isAttached();
	void detach();
	void update();


private:
	GameObject* subject_ = nullptr;
	glm::vec3 relativePosition_ = glm::vec3(0.0f, 0.0f, 5.0f);

	void updateCamVectors();
};