#pragma once
#include "Camera.h"
#include "Object.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

class Enemy :
	public Object
{
public:
	float health;
	int camp;
	float deadtime;
	bool live;
	bool isShoot;
	float ShootTime;
	int ID;

	void doSomething(glm::vec3 playerPositon);
	Enemy(glm::vec3 Pos, int ID);
	~Enemy();
};

