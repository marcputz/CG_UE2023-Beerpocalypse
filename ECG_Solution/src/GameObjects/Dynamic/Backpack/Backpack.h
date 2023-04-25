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
	Backpack(MyShader& shader, PxPhysics* physics) : DynamicGameObject("backpack/backpack.obj", shader, physics) {}
};