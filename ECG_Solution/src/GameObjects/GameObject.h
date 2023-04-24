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
	std::list<std::unique_ptr<GameObject>> children_;
	GameObject* parent_ = nullptr;

	MyTransform transform_;

	GameObject(const std::string& path);
	~GameObject();

	void update(float deltaTime);
	void draw(MyShader& shader);

	void addChild(const std::string& path);
	void addChild(std::unique_ptr<GameObject> child);
	void updateSelfAndChild();
	void forceUpdateSelfAndChild();

protected:
	MyModel* model_;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Material> material;

private:

};

