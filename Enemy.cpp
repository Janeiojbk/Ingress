#include "Enemy.h"



Enemy::Enemy(glm::vec3 Pos, int ID) : Object("./model/Enemy/enemy.obj", glm::vec3(0.05f, 0.05f, 0.05f), 0.0f)
{
	camp = 0;
	position = Pos;
	health = 100.0f;
	deadtime = 0.0f;
	live = true;
	isShoot = false;
	Enemy::ID = ID;
}

void Enemy::doSomething(glm::vec3 playerPosition)
{
	glm::vec3 obj = playerPosition - position;
	if (glm::sqrt(glm::dot(obj, obj)) < 50.0f) {
		theta = acos(glm::dot(-glm::normalize(obj), glm::vec3(0.0f, 0.0f, 1.0f))) + 3.1415926f;
		if (glm::sqrt(glm::dot(obj, obj)) < 5.0f) {
			if (!isShoot) {
				isShoot = true;
				ShootTime = glfwGetTime();
			}
		}
		else if(glfwGetTime() > 2.0f){
			srand(glfwGetTime()*10 + ID);
			position = position + glm::normalize(obj) / 5.0f + glm::vec3(rand() % 100 / 5000.0, 0.0f, rand() % 100 / 5000.0);
		}
	}
	else
	{
		;
	}
}

Enemy::~Enemy()
{
}
