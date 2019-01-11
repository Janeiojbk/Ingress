#include "Portal.h"



Portal::Portal(glm::vec3 Position)
	:Object("./model/portal/statue.obj", glm::vec3(0.6f, 0.6f, 0.6f), 0.0f)
{
	position = Position;
	color = glm::vec3(0.8f, 0.5f, 0.3f);
	lastHacked = 0.0f;
}

Portal::~Portal()
{
}
