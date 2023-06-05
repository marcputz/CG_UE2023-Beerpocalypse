#pragma once

#include "MyAssetManager.h"
#include <glm/glm.hpp>
#include <vector>
#include "Utils.h"
#include "MyShader.h"

class MyGUIRenderer {
public:
	MyGUIRenderer();
	void renderGUI(MyShader& shader, int currentBeerCount, int maxBeerCount, int currentBulletCount, int maxBulletCount, int currentHealth, int maxHealth);

protected:

private:
	My2DTexture beerTexture_;
	My2DTexture beerSilhouetteTexture_;
	My2DTexture bulletTexture_;
	My2DTexture bulletSilhouetteTexture_;
	My2DTexture healthTexture_;
	unsigned int VAO_, VBO_;

	float quadVertices_[20] = {
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f
	};

	void renderBeers(MyShader& shader, int currentBeerCount, int maxBeerCount);
	void renderBullets(MyShader& shader, int currentBulletCount, int maxBulletCount);
	void renderHealth(MyShader& shader, int currentHealth, int maxHealth);
	void initTextures();
};