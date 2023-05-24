#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <string>

struct KeyPosition {
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation {
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale {
	glm::vec3 scale;
	float timeStamp;
};

class MyBone {
public:
	MyBone(const std::string& name, int ID, const aiNodeAnim* channel);
	void update(float animationTime);
	glm::mat4 getLocalTransform();
	std::string getBoneName() const;
	int getBoneID();
	int getPositionIndex(float animationTime);
	int getRotationIndex(float animationTime);
	int getScaleIndex(float animationTime);


protected:

private:
	std::vector<KeyPosition> keyPositions_;
	std::vector<KeyRotation> keyRotations_;
	std::vector<KeyScale> keyScales_;

	int numPositions_;
	int numRotations_;
	int numScales_;

	glm::mat4 localTransform_;
	std::string name_;
	int ID_;

	float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 interpolatePosition(float animationTime);
	glm::mat4 interpolateRotation(float animationTime);
	glm::mat4 interpolateScaling(float animationTime);
};