#include "MyParticleGenerator.h"

MyParticleGenerator::MyParticleGenerator(MyShader& shader, My2DTexture& texture, MyFPSCamera& camera, unsigned int amount) : shader_(&shader), texture_(&texture), camera_(&camera), amount_(amount) {
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

	int newP = (int)(deltaTime * 1000.0f);
	if (newP > (int)(0.016f * 1000.0f)) {
		newP = (int)(0.016f * 1000.0f);
	}

	for (int i = 0; i < newP; i++) {
		int particleIndex = findFirstUnusedParticle();
		particlesContainer_[particleIndex].life = 5.0f;
		particlesContainer_[particleIndex].position = glm::vec3(0.0f, 0.0f, 1.0f);

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 randomDir = glm::vec3((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);

		particlesContainer_[particleIndex].velocity = maindir + randomDir * spread;

		particlesContainer_[particleIndex].r = rand() % 256;
		particlesContainer_[particleIndex].g = rand() % 256;
		particlesContainer_[particleIndex].b = rand() % 256;
		particlesContainer_[particleIndex].a = (rand() % 256) / 3;
		particlesContainer_[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
	}

	int particlesCount = 0;
	for (int i = 0; i < MaxParticles; i++) {
		MyParticle& p = particlesContainer_[i];

		if (p.life > 0.0f) {
			p.life -= deltaTime;
			
			if (p.life > 0.0f) {
				p.velocity += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime * 0.05f;
				p.position += p.velocity * deltaTime;
				p.cameraDistance = glm::length2(p.position - camera_->position_);

				particlePositions_[4 * particlesCount + 0] = p.position.x;
				particlePositions_[4 * particlesCount + 1] = p.position.y;
				particlePositions_[4 * particlesCount + 2] = p.position.z;
				particlePositions_[4 * particlesCount + 3] = p.size;

				particleColors_[4 * particlesCount + 0] = p.r;
				particleColors_[4 * particlesCount + 1] = p.g;
				particleColors_[4 * particlesCount + 2] = p.b;
				particleColors_[4 * particlesCount + 3] = p.a;
			} else {
				p.cameraDistance = -1.0f;
			}
			particlesCount++;
		}
	}

	sortParticles();

	glBindVertexArray(VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 4 * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, amount_ * sizeof(float) * 4, particlePositions_);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 4 * sizeof(char), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, amount_ * sizeof(char) * 4, particleColors_);

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
	texture_->bind();
	shader_->setInt("myTextureSampler", texture_->ID_);
	shader_->setVec3("cameraRight", camera_->right_);
	shader_->setVec3("cameraUp", camera_->up_);

	glBindVertexArray(VAO_);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO_);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount_);

	glBindVertexArray(0);
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
		particlesContainer_[i].life = -1.0f;
		particlesContainer_[i].cameraDistance = -1.0f;
	}

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f
	};

	glGenVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);

	glGenBuffers(1, &verticesVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &positionVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 4 * sizeof(float), NULL, GL_STREAM_DRAW); // data is NULL for now

	glGenBuffers(1, &colorVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO_);
	glBufferData(GL_ARRAY_BUFFER, amount_ * 4 * sizeof(char), NULL, GL_STREAM_DRAW); // char is basically GLubyte

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
		if (particlesContainer_[i].life <= 0.0f) {
			lastUsedParticle_ = i;
			return i;
		}
	}

	for (unsigned int i = 0; i < lastUsedParticle_; i++) {
		if (particlesContainer_[i].life <= 0.0f) {
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
