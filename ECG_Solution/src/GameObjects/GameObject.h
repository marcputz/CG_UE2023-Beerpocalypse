#pragma once

#include <stdio.h>
#include <sstream>
#include "Utils.h"
#include "Shader.h"
#include "Geometry.h"
#include "Texture.h"
#include "MyModel.h"
#include "MyTransform.h"

class GameObject {

public:
	std::list<std::shared_ptr<GameObject>> children_;
	GameObject* parent_ = nullptr;

	MyTransform transform_;

	GameObject(const std::string& path, MyShader& shader);
	~GameObject();

	void update(float deltaTime);
	void draw();

	void addChild(const std::string& path, MyShader& shader);
	void addChild(std::shared_ptr<GameObject> child);

protected:
	MyModel* model_;
	MyShader* shader_;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Material> material;

private:
	//void updateSelfAndChild();
	//void forceUpdateSelfAndChild();

};

