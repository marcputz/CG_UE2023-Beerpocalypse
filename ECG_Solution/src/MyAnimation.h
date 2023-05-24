#pragma once

#include "MyBone.h"
#include "MyModel.h"

struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class MyAnimation {
public:
	MyAnimation() = default;
	MyAnimation(const std::string& animationPath, MyModel* model);
	~MyAnimation();
	
	MyBone* findBone(const std::string& name);
	//std::shared_ptr<MyBone> findBone(const std::string& name);
	float getTicksPerSecond();
	float getDuration();
	const AssimpNodeData& getRootNode();
	const std::map<std::string, BoneInfo>& getBoneInfoMap();

protected:

private:
	float duration_;
	int ticksPerSecond_;
	std::vector<MyBone> bones_;
	AssimpNodeData rootNode_;
	std::map<std::string, BoneInfo> boneInfoMap_;

	void readMissingBones(const aiAnimation* animation, MyModel& model);
	void readHierarchyData(AssimpNodeData& destination, const aiNode* source); 

};