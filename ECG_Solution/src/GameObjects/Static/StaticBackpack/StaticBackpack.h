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
	StaticBackpack(const std::string& path, MyShader& shader, PxPhysics* physics) : StaticGameObject(path, shader, physics) {}
};