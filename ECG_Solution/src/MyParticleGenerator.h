#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "MyShader.h"
#include "My2DTexture.h"
#include "GameObjects/Player/Player.h"
#include "GameObjects/NewGameObject.h"

struct MyParticle {
	glm::vec3 position;
	glm::vec3 velocity;
	int textureSelect;
	bool affectedByGravity;
	bool isImmortal;
	float size;
	float remainingLife;
	float maxLife;
	float cameraDistance;
	NewGameObject* boundTo;
	//float angle;
	//float weight;
	//glm::vec4 color;
	//char r, g, b, a;

	MyParticle() : position(0.0f), velocity(0.0f)/*, r(255), g(255), b(255), a(255)*/, size(1.0f)/*, angle(0.0f), weight(1.0f)*/, remainingLife(0.0f), maxLife(0.0f), boundTo(nullptr), isImmortal(false) {}
	bool operator<(const MyParticle& that) const {
		return this->cameraDistance > that.cameraDistance;
	}
};

enum ParticleType {
	ZOMBIE_BLOOD,
	BEER_SPARKLE
};

const int MaxParticles = 100;

class MyParticleGenerator {
public:
	MyParticleGenerator(MyShader& shader/*, My2DTexture& texture*/, Player* player, unsigned int amount);
	//void update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void update(float deltaTime);
	void draw();

	void createParticles(glm::vec3 position, glm::vec3 direction, ParticleType type, float avgLifetime, int amount, bool hasGravity, NewGameObject* objectToBindTo);

protected:

private:
	std::vector<MyParticle> particles_;
	unsigned int amount_;
	unsigned int amountToDraw_;
	MyShader* shader_;
	My2DTexture zombieBloodTexture_;
	My2DTexture beerParticleTexture_;
	Player* player_;
	unsigned int VAO_;
	unsigned int verticesVBO_, positionVBO_, textureVBO_, colorVBO_;
	unsigned int lastUsedParticle_ = 0;

	MyParticle particlesContainer_[MaxParticles];
	float* particlePositions_ = new float[MaxParticles * 4];
	int* particleTextures_ = new int[MaxParticles * 1];
	//char* particleColors_ = new char[MaxParticles * 4];

	void init();
	unsigned int findFirstUnusedParticle();
	//void respawnParticle(MyParticle& particle, GameObject& object, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void sortParticles();
};