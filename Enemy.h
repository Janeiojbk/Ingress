#pragma once
#include "Player.h"
#include "Object.h"

class Enemy :
	public Player,
	public Object
{
public:
	friend Player;
	Enemy();
	~Enemy();

	void AIplay();
};

