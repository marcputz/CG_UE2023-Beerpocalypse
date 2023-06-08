#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "GameObjects/GameObject.h"
#include "MyShader.h"
#include "My2DTexture.h"
#include "GameObjects/Player/NewPlayer.h"

struct MyParticle {
	glm::vec3 position;
	glm::vec3 velocity;
	//glm::vec4 color;
	char r, g, b, a;
	float size, angle, weight;
	float life, cameraDistance;

	MyParticle() : position(0.0f), velocity(0.0f), r(255), g(255), b(255), a(255), size(1.0f), angle(0.0f), weight(1.0f), life(0.0f) {}
	bool operator<(const MyParticle& that) const {
		return this->cameraDistance > that.cameraDistance;
	}
};

const int MaxParticles = 50;

class MyParticleGenerator {
public:
	MyParticleGenerator(MyShader& shader, My2DTexture& texture, NewPlayer* player, unsigned int amount);
	//void update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void update(float deltaTime);
	void draw();

protected:

private:
	std::vector<MyParticle> particles_;
	unsigned int amount_;
	MyShader* shader_;
	My2DTexture* texture_;
	NewPlayer* player_;
	unsigned int VAO_;
	unsigned int verticesVBO_, positionVBO_, colorVBO_;
	unsigned int lastUsedParticle_ = 0;

	MyParticle particlesContainer_[MaxParticles];
	float* particlePositions_ = new float[MaxParticles * 4];
	char* particleColors_ = new char[MaxParticles * 4];

	void init();
	unsigned int findFirstUnusedParticle();
	//void respawnParticle(MyParticle& particle, GameObject& object, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void sortParticles();
};