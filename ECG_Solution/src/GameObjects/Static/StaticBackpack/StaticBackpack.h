#include "../StaticGameObject.h"
#include <PxPhysicsAPI.h>

using namespace std;
using namespace physx;

class StaticBackpack : public StaticGameObject {
private:

protected:

public:
	void update(float deltaTime);

	// Constructors
	StaticBackpack(MyShader& shader, PxPhysics* physics) : StaticGameObject("backpack/backpack.obj", shader, physics) {}
};