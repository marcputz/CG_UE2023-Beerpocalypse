#pragma once

#include "MyAnimation.h"

class MyAnimator {
public:
	MyAnimator(MyAnimation* animation);
	void updateAnimation(float deltaTime);
	void playAnimation(MyAnimation* animationToPlay);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> getFinalBoneMatrices();

protected:

private:
	std::vector<glm::mat4> finalBoneMatrices_;
	MyAnimation* currentAnimation_;
	float currentTime_;
	float deltaTime_;

};