#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Resonator.h"
#include "shader_m.h"

class Portal: public Object
{
public:
	Portal(glm::vec3 Position);
	~Portal();
	float lastHacked;
	float radius;
	Resonator resonator[8];
};

