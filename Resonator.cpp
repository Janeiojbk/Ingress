#include "Resonator.h"


Resonator::Resonator() : Object(){
	health = 0;
	index = -1;
}
Resonator::Resonator(glm::vec3 PortalPosition, int index, float Distance): 
	Object("./wheelByGuo/resonator/resonator.obj", glm::vec3(0.6f, 0.6f, 0.6f), 0.0f)
{
	distance = Distance;
	health = 100;
	Resonator::index = index;

	float x = PortalPosition.x + distance * glm::cos(glm::radians(45.0 * index));
	float z = PortalPosition.z + distance * glm::sin(glm::radians(45.0 * index));
	position = glm::vec3(x, 0.0f, z);
}


Resonator::~Resonator()
{
}

