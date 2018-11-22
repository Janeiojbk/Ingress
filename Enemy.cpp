#include "Enemy.h"



Enemy::Enemy():Player(), Object(glm::vec3(0.2f, 0.5f, 0.8f), glm::vec3(0.8f, 3.0f, 0.8f))
{
	camp = 0;
	position = glm::vec3(5.0f, 0.0f, -5.0f);
}


Enemy::~Enemy()
{
}
