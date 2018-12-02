#include "Enemy.h"



Enemy::Enemy():Object("./wheelByYang/enemy.obj", glm::vec3(0.05f, 0.05f, 0.05f), 0.0f)
{
	camp = 0;
	position = glm::vec3(0.0f, 0.3f, -5.0f);
	health = 100.0f;
	deadtime = 0.0f;
	live = true;
}


Enemy::~Enemy()
{
}
