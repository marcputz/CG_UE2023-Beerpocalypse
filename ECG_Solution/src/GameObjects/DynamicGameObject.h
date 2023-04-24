#pragma once

#include "GameObject.h"

class DynamicGameObject : public GameObject {

public:
	DynamicGameObject(std::string const& path, MyShader& shader);
	virtual ~DynamicGameObject();
	void update(float deltaT);
};

