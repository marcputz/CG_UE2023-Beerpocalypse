#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "GameObjects/Player/Camera/PlayerCamera.h"

struct Plane {
	// unit vector
	glm::vec3 normal = { 0.0f, 1.0f, 0.0f };
	//distance from origin to nearest point in the plane
	float distance = 0.0f;

	Plane() = default;
	Plane(const glm::vec3& pos, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, pos)) {}

	float getDistanceToPlane(const glm::vec3& point) const {
		return glm::dot(normal, point) - distance;
	}

	void print(const std::string& name) {
		std::cout << name.c_str() << " Normal: " << normal.x << "/" << normal.y << "/" << normal.z << ", Distance: " << distance << std::endl;
	}
};

struct Frustum {
	Plane topFace;
	Plane bottomFace;
	Plane rightFace;
	Plane leftFace;
	Plane farFace;
	Plane nearFace;

	void print();

	static Frustum createFrustumFromCamera(PlayerCamera& camera, float aspect, float fovY, float zNear, float zFar);
};