#include "Portal.h"



Portal::Portal(glm::vec3 Position)
	:Object(glm::vec3(0.6f, 0.4f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f))
{
	position = Position;
	color = glm::vec3(0.8f, 0.5f, 0.3f);
	lastHacked = 0.0f;
}

Portal::~Portal()
{
}
