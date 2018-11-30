#include "Enemy.h"



Enemy::Enemy():Player(), Object("./wheelByYang/enemy.obj", glm::vec3(0.1f, 0.1f, 0.1f))
{
	camp = 0;
	position = glm::vec3(0.0f, 0.0f, -5.0f);
}


Enemy::~Enemy()
{
}
