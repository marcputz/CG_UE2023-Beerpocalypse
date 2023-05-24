#include "MyAnimation.h"

MyAnimation::MyAnimation(const std::string& animationPath, MyModel* model) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	duration_ = animation->mDuration;
	ticksPerSecond_ = animation->mTicksPerSecond;
	readHierarchyData(rootNode_, scene->mRootNode);
	readMissingBones(animation, *model);
}

MyAnimation::~MyAnimation() {
}


MyBone* MyAnimation::findBone(const std::string& name) {
	auto iter = std::find_if(bones_.begin(), bones_.end(),
		// lambda
		[&](const MyBone& bone) {
			//return bone.getBoneName() == name;
			return bone.name_ == name;
		});
	if (iter == bones_.end()) {
		return nullptr;
	} else {
		return &(*iter);
	}
}

/*
std::shared_ptr<MyBone> MyAnimation::findBone(const std::string& name) {
	auto iter = std::find_if(bones_.begin(), bones_.end(),
		// lambda
		[&](const MyBone& bone) {
			return bone.getBoneName() == name;
		});
	if (iter == bones_.end()) {
		return nullptr;
	} else {
		return std::make_shared<MyBone>(iter._Ptr);
	}
}
*/

float MyAnimation::getTicksPerSecond() {
	return ticksPerSecond_;
}

float MyAnimation::getDuration() {
	return duration_;
}

const AssimpNodeData& MyAnimation::getRootNode() {
	return rootNode_;
}

const std::map<std::string, BoneInfo>& MyAnimation::getBoneInfoMap() {
	return boneInfoMap_;
}

void MyAnimation::readMissingBones(const aiAnimation* animation, MyModel& model) {
	int size = animation->mNumChannels;

	std::map<std::string, BoneInfo>& boneInfoMap = model.getBoneInfoMap();
	int& boneCount = model.getBoneCount();

	for (int i = 0; i < size; i++) {
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			boneInfoMap[boneName].ID = boneCount;
			boneCount++;
		}
		bones_.push_back(MyBone(boneName, boneInfoMap[channel->mNodeName.data].ID, channel));
	}
	boneInfoMap_ = boneInfoMap;
}

void MyAnimation::readHierarchyData(AssimpNodeData& destination, const aiNode* source) {
	assert(source);

	destination.name = source->mName.data;
	destination.transformation = glm::mat4(
		source->mTransformation.a1, source->mTransformation.b1, source->mTransformation.c1, source->mTransformation.d1,
		source->mTransformation.a2, source->mTransformation.b2, source->mTransformation.c2, source->mTransformation.d2,
		source->mTransformation.a3, source->mTransformation.b3, source->mTransformation.c3, source->mTransformation.d3,
		source->mTransformation.a4, source->mTransformation.b4, source->mTransformation.c4, source->mTransformation.d4
	);
	destination.childrenCount = source->mNumChildren;

	for (int i = 0; i < source->mNumChildren; i++) {
		AssimpNodeData newData;
		readHierarchyData(newData, source->mChildren[i]);
		destination.children.push_back(newData);
	}
}
