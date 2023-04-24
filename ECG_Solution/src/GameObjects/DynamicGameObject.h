#pragma once

#include "GameObject.h"

class DynamicGameObject : public GameObject {

public:
	DynamicGameObject(std::string const& path);
	virtual ~DynamicGameObject();
	void update(float deltaT);
};

