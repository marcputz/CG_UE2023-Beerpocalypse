#include "MyFrustum.h"

void Frustum::print() {
	nearFace.print("Near");
	farFace.print("Far");
	rightFace.print("Right");
	leftFace.print("Left");
	topFace.print("Top");
	bottomFace.print("Bottom");
}

Frustum Frustum::createFrustumFromCamera(PlayerCamera& camera, float aspect, float fovY, float zNear, float zFar) {
	Frustum frustum;
	const float halfVSide = zFar * tanf(fovY * 0.5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 cameraDir = camera.getDirection();
	const glm::vec3 cameraPos = camera.getPosition();
	const glm::vec3 cameraUp = camera.getUp();
	const glm::vec3 cameraRight = camera.getRight();
	const glm::vec3 dirMultFar = cameraDir * zFar;

	frustum.nearFace = { cameraPos + zNear * cameraDir, cameraDir };
	frustum.farFace = { cameraPos + dirMultFar, -cameraDir };
	frustum.rightFace = { cameraPos, glm::cross(dirMultFar - cameraRight * halfHSide, cameraUp)};
	frustum.leftFace = { cameraPos, glm::cross(cameraUp, dirMultFar + cameraRight * halfHSide)};
	frustum.topFace = { cameraPos, glm::cross(cameraRight, dirMultFar - cameraUp * halfVSide)};
	frustum.bottomFace = { cameraPos, glm::cross(dirMultFar + cameraUp * halfVSide, cameraRight)};
	
	return frustum;
}
