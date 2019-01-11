#include "Resonator.h"


Resonator::Resonator() : Object(){
	health = 0;
	index = -1;
	//Particle = ParticleGenerator(position, glm::vec3(0.0f, 0.0f, 0.0f), 100);
}
Resonator::Resonator(glm::vec3 PortalPosition, int index, float Distance): 
	Object("./model/resonator/resonator.obj", glm::vec3(0.6f, 0.6f, 0.6f), 0.0f)
{
	distance = Distance;
	health = 100;
	Resonator::index = index;

	float x = PortalPosition.x + distance * glm::cos(glm::radians(45.0 * index));
	float z = PortalPosition.z + distance * glm::sin(glm::radians(45.0 * index));
	position = glm::vec3(x, 0.0f, z);

	// Particle = ParticleGenerator(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(15.0f, 15.0f, 15.0f), 100);
	Particle = ParticleGenerator(position, PortalPosition, 10);

	putTime = glfwGetTime();
}


Resonator::~Resonator()
{
}

