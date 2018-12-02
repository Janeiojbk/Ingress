#pragma once
#include "Camera.h"
#include "Object.h"

class Enemy :
	public Object
{
public:
	float health;
	int camp;
	float deadtime;
	bool live;
	Enemy();
	~Enemy();

};

