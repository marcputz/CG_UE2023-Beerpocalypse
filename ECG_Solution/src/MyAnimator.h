#pragma once

#include "MyAnimation.h"

enum Animation_Enum {
	IDLE,
	WALKING,
	ATTACKING,
	NONE
};

class MyAnimator {
public:
	MyAnimator();
	MyAnimator(MyAnimation* animation);
	void updateAnimation(float deltaTime);
	void playAnimation(MyAnimation* animationToPlay);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4>& getFinalBoneMatrices();
	void setAnimationSpeedMultiplier(float newAnimationSpeedMultiplier);
	float& getAnimationSpeedMultiplier();
	void addAnimation(Animation_Enum enumID, MyAnimation* animation);
	void changeAnimation(Animation_Enum enumID, float newAnimationSpeedMultiplier = 1.0f, bool forceAnimationCompletion = false);

protected:

private:
	std::vector<glm::mat4> finalBoneMatrices_;
	MyAnimation* currentAnimation_;
	Animation_Enum currentAnimEnum_;
	bool forceAnimationCompletion_;
	bool animationComplete_;
	std::map<Animation_Enum, MyAnimation*> animations_;
	float currentTime_;
	float deltaTime_;
	float animationSpeedMultiplier_;

};