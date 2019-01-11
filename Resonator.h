#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Resonator.h"
#include "Object.h"
#include "shader_m.h"
#include "particle.h"


class Resonator:public Object
{
public:
	Resonator();
	Resonator(glm::vec3 PortalPosition, int index, float Distance);
	~Resonator();

	float distance;
	long health;
	int index;

	float putTime;

	ParticleGenerator Particle;
};

