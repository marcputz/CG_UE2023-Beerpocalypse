#pragma once

#include <vector>
#include <memory>
#include <sstream>
#include <glm/glm.hpp>

#include "MyFPSCamera.h"
#include "GameObjects/DynamicGameObject.h"

class MyScene {
public:

	MyScene(MyFPSCamera &camera);
	virtual ~MyScene();

	void draw();
	void update(float deltaTime);

	void addGameObject(GameObject& gameObject);

protected:

	MyFPSCamera* camera_;
	std::vector<GameObject*> gameObjects_;

	GameObject* gameObject_;

	//MyDirectionalLight dirLight_;
	//std::vector<MyPointLight> pointLights_;
	//std::vector<MySpotLight> spotLights_;

	//std::vector<std::shared_ptr<DynamicGameObject>> dynamicGameObjects_;
};
