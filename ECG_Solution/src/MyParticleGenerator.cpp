#include "MyParticleGenerator.h"

MyParticleGenerator::MyParticleGenerator(MyShader& shader/*, My2DTexture& texture*/, NewPlayer* player, unsigned int amount) : shader_(&shader)/*, zombieBloodTexture_(&texture)*/, player_(player), amount_(amount) {
	init();
}

//void MyParticleGenerator::update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec3 offset) {
void MyParticleGenerator::update(float deltaTime) {

	/*
	for (unsigned int i = 0; i < newParticles; i++) {
		int unusedParticleIdx = findFirstUnusedParticle();
		respawnParticle(particles_[unusedParticleIdx], object, offset);
	}

	for (unsigned int i = 0; i < amount_; i++) {
		MyParticle& p = particles_[i];
		p.life -= deltaTime;
		if (p.life > 0.0f) {
			p.position -= p.velocity * deltaTime;
			p.color.a -= deltaTime * 2.5f;
		}
	}
	*/

	/*
	int newP = (int)(deltaTime * 1000.0f);
	if (newP > (int)(0.016f * 1000.0f)) {
		newP = (int)(0.016f * 1000.0f);
	}

	for (int i = 0; i < newP; i++) {
		int particleIndex = findFirstUnusedParticle();
		particlesContainer_[particleIndex].life = 5.0f;
		particlesContainer_[particleIndex].position = glm::vec3(0.0f, 3.0f, 1.0f);
		particlesContainer_[particleIndex].textureSelect = rand() % (2 - 1 + 1) + 1; // %(max-min + 1) + min

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 randomDir = glm::vec3((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);

		particlesContainer_[particleIndex].velocity = maindir + randomDir * spread;

		//particlesContainer_[particleIndex].r = rand() % 256;
		//particlesContainer_[particleIndex].g = rand() % 256;
		//particlesContainer_[particleIndex].b = rand() % 256;
		particlesContainer_[particleIndex].a = (rand() % 256) / 3;

		particlesContainer_[particleIndex].size = (rand() % 1000) / 3000.0f + 0.1f;
	}
	*/

	amountToDraw_ = 0;
	for (int i = 0; i < MaxParticles; i++) {
		MyParticle& p = particlesContainer_[i];

		if (p.remainingLife > 0.0f) {
			p.remainingLife -= deltaTime;

			// reset lifetime of sparkles and reverse their movement
			if (p.remainingLife < 0.0f && p.textureSelect == 2) {
				p.remainingLife = p.maxLife;
				p.velocity = (p.velocity * -1.0f);
			}

			if (p.remainingLife > 0.0f) {
				if (p.affectedByGravity) {
					p.velocity += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime * 0.05f;
				}
				p.position += p.velocity * deltaTime;
				p.cameraDistance = glm::length2(p.position - player_->getActiveCamera()->getPosition());

				particlePositions_[4 * amountToDraw_ + 0] = p.position.x;
				particlePositions_[4 * amountToDraw_ + 1] = p.position.y;
				particlePositions_[4 * amountToDraw_ + 2] = p.position.z;
				particlePositions_[4 * amountToDraw_ + 3] = p.size;

				particleTextures_[amountToDraw_] = p.textureSelect;

				/*
				particleColors_[4 * amountToDraw_ + 0] = p.r;
				particleColors_[4 * amountToDraw_ + 1] = p.g;
				particleColors_[4 * amountToDraw_ + 2] = p.b;
				particleColors_[4 * amountToDraw_ + 3] = p.a;
				*/
			} else {
				p.cameraDistance = -1.0f;
			}

			amountToDraw_++;
		}
	}

	sortParticles();

	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
	glBufferData(GL_ARRAY_BUFFER, amountToDraw_ * 4 * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, amountToDraw_ * 4 * sizeof(float), particlePositions_);

	glBindBuffer(GL_ARRAY_BUFFER, textureVBO_);
	glBufferData(GL_ARRAY_BUFFER, amountToDraw_ * 1 * sizeof(int), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, amountToDraw_ * 1 * sizeof(int), particleTextures_);

	/*
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO_);
	glBufferData(GL_ARRAY_BUFFER, amountToDraw_ * 4 * sizeof(char), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, amountToDraw_ * 4 * sizeof(char), particleColors_);
	*/

	glBindVertexArray(0);
}

void MyParticleGenerator::draw() {
	/*
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	shader_->use();
	for (MyParticle particle : particles_) {
		if (particle.life > 0.0f) {
			shader_->setVec3("offset", particle.position);
			shader_->setVec4("color", particle.color);
			texture_->bind();
			glBindVertexArray(VAO_);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/

	shader_->use();

	glActiveTexture(GL_TEXTURE0);
	zombieBloodTexture_.bind();
	// TODO: create an actual blood texture and also a sparkle texture and bind it
	shader_->setInt("bloodTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	beerParticleTexture_.bind();
	shader_->setInt("sparkleTexture", 1);

	PlayerCamera* currCam = player_->getActiveCamera();
	currCam->updateRightAndUp();
	shader_->setVec3("cameraRight", currCam->getRight() * -1.0f);
	shader_->setVec3("cameraUp", currCam->getUp());

	glBindVertexArray(VAO_);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO_);
	glVertexAttribIPointer(2, 1, GL_INT, 0, (void*)0);

	/*
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO_);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
	*/

	glVertexAttribDivisor(0, 0); // vertices
	glVertexAttribDivisor(1, 1); // positions
	glVertexAttribDivisor(2, 1); // textureSelect
	//glVertexAttribDivisor(3, 1);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amountToDraw_);

	glBindVertexArray(0);
}

void MyParticleGenerator::createParticles(glm::vec3 position, glm::vec3 direction, ParticleType type, float avgLifetime, int amount, bool hasGravity) {
	int texSelect;

	switch (type) {
		case ZOMBIE_BLOOD:
			// all particles we create are zombie blood
			texSelect = 1;

			for (int i = 0; i < amount; i++) {
				int particleIndex = findFirstUnusedParticle();
				particlesContainer_[particleIndex].maxLife = avgLifetime;
				particlesContainer_[particleIndex].remainingLife = avgLifetime;
				// all blood particles begin a the same spot
				particlesContainer_[particleIndex].position = position;
				particlesContainer_[particleIndex].textureSelect = texSelect;
				// but "spurt out" in different angles
				float spread = 1.5f;
				glm::vec3 randomDir = glm::vec3(
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f);

				particlesContainer_[particleIndex].velocity = direction + randomDir * spread;
				particlesContainer_[particleIndex].size = (rand() % 1000) / 5000.0f + 0.05f;
				particlesContainer_[particleIndex].affectedByGravity = hasGravity;
			}

			break;
		case BEER_SPARKLE:
			// all particles we create are beer sparkles
			texSelect = 2;

			for (int i = 0; i < amount; i++) {
				int particleIndex = findFirstUnusedParticle();
				// add some lifetime variance
				float randomTimeOffset = (rand() % 2000 - 1000.0f) / 2000.0f;
				particlesContainer_[particleIndex].maxLife = avgLifetime + randomTimeOffset;
				particlesContainer_[particleIndex].remainingLife = particlesContainer_[particleIndex].maxLife;
				// particles should be randomly around the beer
				// add randomness to position
				glm::vec3 randomXYZPos = glm::vec3(
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 2000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f);
				particlesContainer_[particleIndex].position = position + randomXYZPos;
				
				particlesContainer_[particleIndex].textureSelect = texSelect;

				//float spread = 1.5f;
				//glm::vec3 randomDir = glm::vec3((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);

				particlesContainer_[particleIndex].velocity = direction; // + randomDir * spread;
				particlesContainer_[particleIndex].size = (rand() % 1000) / 5000.0f + 0.05f;
				particlesContainer_[particleIndex].affectedByGravity = hasGravity;
			}

			break;
		default:
			// no default particles
			return;
	}
}

void MyParticleGenerator::init() {
	/*
	unsigned int VBO;

	float particleQuad[] = {
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);

	for (unsigned int i = 0; i < amount_; i++) {
		particles_.push_back(MyParticle());
	}
	*/

	for (unsigned int i = 0; i < MaxParticles; i++) {
		particlesContainer_[i].remainingLife = -1.0f;
		particlesContainer_[i].cameraDistance = -1.0f;
	}

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f
	};

	this->zombieBloodTexture_ = MyAssetManager::loadTexture("assets/textures/zombieBloodParticle.png", DIFFUSE, true, "zombieBloodParticle");
	this->beerParticleTexture_ = MyAssetManager::loadTexture("assets/textures/beerSparkle.png", DIFFUSE, true, "beerSparkleParticle");

	glGenVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);

	glGenBuffers(1, &verticesVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &positionVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 4 * sizeof(float), NULL, GL_STREAM_DRAW); // data is NULL for now

	glGenBuffers(1, &textureVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 1 * sizeof(int), NULL, GL_STREAM_DRAW); // using an int to determine which texture to use

	/*
	glGenBuffers(1, &colorVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 4 * sizeof(char), NULL, GL_STREAM_DRAW); // char is basically GLubyte
	*/

	glBindVertexArray(0);
}

unsigned int MyParticleGenerator::findFirstUnusedParticle() {
	// search starting from the last used particle
	/*
	for (unsigned int i = lastUsedParticle_; i < amount_; i++) {
		if (particles_[i].life <= 0.0f) {
			lastUsedParticle_ = i;
			return i;
		}
	}
	// if search from last used particle till the end did not return search from 0 till last used particle
	for (unsigned int i = 0; i < lastUsedParticle_; i++) {
		if (particles_[i].life <= 0.0f) {
			lastUsedParticle_ = i;
			return i;
		}
	}

	lastUsedParticle_ = 0;
	return 0;
	*/

	for (unsigned int i = 0; i < MaxParticles; i++) {
		if (particlesContainer_[i].remainingLife <= 0.0f) {
			lastUsedParticle_ = i;
			return i;
		}
	}

	for (unsigned int i = 0; i < lastUsedParticle_; i++) {
		if (particlesContainer_[i].remainingLife <= 0.0f) {
			lastUsedParticle_ = i;
			return i;
		}
	}

	return 0;
}

/*
void MyParticleGenerator::respawnParticle(MyParticle& particle, GameObject& object, glm::vec3 offset) {
	float random = ((rand() % 100) - 50) / 10.0f;
	float randomColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.position = object.getGlmPosition() + random + offset;
	particle.color = glm::vec4(randomColor, randomColor, randomColor, 1.0f);
	particle.life = 1.0f;
	particle.velocity = glm::vec3(0.5f);
}
*/

void MyParticleGenerator::sortParticles() {
	std::sort(&particlesContainer_[0], &particlesContainer_[MaxParticles]);
}
