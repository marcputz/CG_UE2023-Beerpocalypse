#pragma once

#include "MyAssetManager.h"
#include <glm/glm.hpp>
#include <vector>
#include "Utils.h"
#include "MyShader.h"

class MyGUIRenderer {
public:
	MyGUIRenderer();
	void renderGUI(MyShader& shader, int currentBeerCount, int maxBeerCount);

protected:

private:
	My2DTexture beerTexture_;
	My2DTexture beerSilhouetteTexture_;
	unsigned int VAO_, VBO_;

	void initTextures();

	float quadVertices_[20] = {
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f
	};

};