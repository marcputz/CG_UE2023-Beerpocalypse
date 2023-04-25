#include "../DynamicGameObject.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <PxRigidActor.h>

class Backpack : public DynamicGameObject {
private:

protected:

public:
	void update(float deltaTime);

	// Constructors
	Backpack(const std::string& path, MyShader& shader, PxPhysics* physics) : DynamicGameObject(path, shader, physics) {}
};