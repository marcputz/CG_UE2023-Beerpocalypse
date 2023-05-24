#include "MyAnimator.h"

MyAnimator::MyAnimator(MyAnimation* animation) {
	currentTime_ = 0.0f;
	currentAnimation_ = animation;

	finalBoneMatrices_.reserve(100);

	for (int i = 0; i < 100; i++) {
		finalBoneMatrices_.push_back(glm::mat4(1.0f));
	}
}

void MyAnimator::updateAnimation(float deltaTime) {
	deltaTime_ = deltaTime;

	if (currentAnimation_) {
		currentTime_ += currentAnimation_->getTicksPerSecond() * deltaTime;
		currentTime_ = fmod(currentTime_, currentAnimation_->getDuration());
		calculateBoneTransform(&currentAnimation_->getRootNode(), glm::mat4(1.0f));
	}
}

void MyAnimator::playAnimation(MyAnimation* animationToPlay) {
	currentAnimation_ = animationToPlay;
	currentTime_ = 0.0f;
}

void MyAnimator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
	//std::string nodeName = node->name;
	const std::string& nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;
	
	MyBone* bone = currentAnimation_->findBone(nodeName);
	//std::shared_ptr<MyBone> bone = currentAnimation_->findBone(nodeName);

	if (bone) {
		bone->update(currentTime_);
		nodeTransform = bone->getLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	//std::map<std::string, BoneInfo> boneInfoMap = currentAnimation_->getBoneInfoMap();
	const std::map<std::string, BoneInfo>& boneInfoMap = currentAnimation_->getBoneInfoMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
		//int index = boneInfoMap[nodeName].ID;
		//glm::mat4 offset = boneInfoMap[nodeName].offset;
		//finalBoneMatrices_[index] = globalTransformation * offset;
		int index = boneInfoMap.at(nodeName).ID;
		finalBoneMatrices_[index] = globalTransformation * boneInfoMap.at(nodeName).offset;
	}

	for (int i = 0; i < node->childrenCount; i++) {
		calculateBoneTransform(&node->children[i], globalTransformation);
	}
}

std::vector<glm::mat4> MyAnimator::getFinalBoneMatrices() {
	return finalBoneMatrices_;
}
