#pragma once

#include <stdio.h>
#include <sstream>
#include "Utils.h"
#include "Shader.h"
#include "Geometry.h"
#include "Texture.h"

class GameObject
{

public:
	void draw();

private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Material> material;
};

