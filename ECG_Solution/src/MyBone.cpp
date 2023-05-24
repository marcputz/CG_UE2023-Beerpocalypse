#include "MyBone.h"

MyBone::MyBone(const std::string& name, int ID, const aiNodeAnim* channel) : name_(name), ID_(ID), localTransform_(1.0f) {
	numPositions_ = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < numPositions_; positionIndex++) {
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
		data.timeStamp = timeStamp;
		keyPositions_.push_back(data);
	}

	numRotations_ = channel->mNumRotationKeys;
	for (int rotatationIndex = 0; rotatationIndex < numRotations_; rotatationIndex++) {
		aiQuaternion aiOrientation = channel->mRotationKeys[rotatationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotatationIndex].mTime;
		KeyRotation data;
		data.orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
		data.timeStamp = timeStamp;
		keyRotations_.push_back(data);
	}

	numScales_ = channel->mNumScalingKeys;
	for (int scaleIndex = 0; scaleIndex < numScales_; scaleIndex++) {
		aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
		float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
		KeyScale data;
		data.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
		data.timeStamp = timeStamp;
		keyScales_.push_back(data);
	}
}

void MyBone::update(float deltaTime) {
	glm::mat4 translation = interpolatePosition(deltaTime);
	glm::mat4 rotation = interpolateRotation(deltaTime);
	glm::mat4 scale = interpolateScaling(deltaTime);
	localTransform_ = translation * rotation * scale;
}

glm::mat4 MyBone::getLocalTransform() {
	return localTransform_;
}

std::string MyBone::getBoneName() const {
	return name_;
}

int MyBone::getBoneID() {
	return ID_;
}

int MyBone::getPositionIndex(float animationTime) {
	for (int i = 0; i < numPositions_ - 1; i++) {
		if (animationTime < keyPositions_[i + 1].timeStamp) {
			return i;
		}
	}
	assert(0);
}

int MyBone::getRotationIndex(float animationTime) {
	for (int i = 0; i < numRotations_ - 1; i++) {
		if (animationTime < keyRotations_[i + 1].timeStamp) {
			return i;
		}
	}
	assert(0);
}

int MyBone::getScaleIndex(float animationTime) {
	for (int i = 0; i < numScales_ - 1; i++) {
		if (animationTime < keyScales_[i + 1].timeStamp) {
			return i;
		}
	}
	assert(0);
}

float MyBone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
	float scaleFactor = 0.0f;
	float midwayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midwayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 MyBone::interpolatePosition(float animationTime) {
	if (numPositions_ == 1) {
		return glm::translate(glm::mat4(1.0f), keyPositions_[0].position);
	}
	int p0Index = getPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(keyPositions_[p0Index].timeStamp, keyPositions_[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(keyPositions_[p0Index].position, keyPositions_[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 MyBone::interpolateRotation(float animationTime) {
	if (numRotations_ == 1) {
		auto rotation = glm::normalize(keyRotations_[0].orientation);
		return glm::toMat4(rotation);
	}
	int r0Index = getRotationIndex(animationTime);
	int r1Index = r0Index + 1;
	float scaleFactor = getScaleFactor(keyRotations_[r0Index].timeStamp, keyRotations_[r1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(keyRotations_[r0Index].orientation, keyRotations_[r1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

glm::mat4 MyBone::interpolateScaling(float animationTime) {
	if (numScales_ == 1) {
		return glm::scale(glm::mat4(1.0f), keyScales_[0].scale);
	}

	int s0Index = getScaleIndex(animationTime);
	int s1Index = s0Index + 1;
	float scaleFactor = getScaleFactor(keyScales_[s0Index].timeStamp, keyScales_[s1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(keyScales_[s0Index].scale, keyScales_[s1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}
