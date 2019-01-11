#include "Player.h"


Player::Player()
{
	xm = 2000;
	camp = 1;
	// 道具数量
	resonatorNum = 8;
	xmpBursterNum = 2;
	powerCubeNum = 2;
	getR = false;
	getX = false;
	getC = false;
	isWin = false;
	isUsingXmp = false;
	isShoot = false;
	HackTime = 0;
}


Player::~Player()
{
}

void Player::hack(std::vector<Portal> &portalVec)
{
	for (Portal &portal : portalVec){
		glm::vec3 ObjVec = portal.position - Position;
		float theta = glm::dot(glm::normalize(ObjVec), glm::normalize(Front));
		float timeValue = glfwGetTime();
		std::cout << timeValue << std::endl;
		float distance = sqrt(glm::dot(ObjVec, ObjVec));
		if (theta >= distance / sqrt(distance*distance + PSIZE*PSIZE) && distance <= RADIUS 
			&&(timeValue - portal.lastHacked >= HACKTIME || portal.lastHacked < 0.01f)) {
			float greenValue = sin(timeValue) / 2.0f + 0.5f;
			portal.lastHacked = timeValue;
			portal.color.y = greenValue;

			// getItem
			srand(unsigned(timeValue*100+1100));
			int num = rand();
			if (num % 3 == 0) {
				resonatorNum++;
				getR = true;
			}
			if (num % 3 == 1) {
				xmpBursterNum++;
				getX = true;
			}
			if (num % 3 == 2) {
				powerCubeNum++;
				getC = true;
			}
			HackPosition = portal.position;
			HackTime = timeValue;
		}
	}
}

void Player::UseResonator(std::vector<Portal> &portalVec)
{
	if (resonatorNum > 0) {
		for (Portal &portal : portalVec) {
			glm::vec3 ObjVec = portal.position - Position;
			float distance = sqrt(glm::dot(ObjVec, ObjVec));
			if (distance <= RADIUS && resonatorNum > 0) {
				for (int i = 0; i != 8; i++) {
					if (portal.resonator[i].index == -1) {
						portal.resonator[i] = Resonator(portal.position, i, distance);
						break;
					}
				}
			}
		}
		--resonatorNum;
	}
}
void Player::UsePowerCube()
{
	if (powerCubeNum > 0) {
		powerCubeNum--;
		if (xm <= 2000)
			xm += 8000;
		else
			xm = 10000;
	}
}
void Player::UseXmpBurster(std::vector<Portal> &portalVec)
{
	if (xmpBursterNum > 0) {
		for (Portal &portal : portalVec) {
			glm::vec3 ObjVec = portal.position - Position;
			float timeValue = glfwGetTime();
			float distance = sqrt(glm::dot(ObjVec, ObjVec));
			if (distance <= RADIUS) {
				float greenValue = sin(timeValue) / 2.0f + 0.5f;
				std::vector<int> index;
				for (Resonator &item : portal.resonator) {
					glm::vec3 playerToRes = item.position - Position;
					float distanceRes = sqrt(glm::dot(playerToRes, playerToRes));
					if (distanceRes <= XMPRADIUS) {
						item.health -= HURT;
						item.index = -1;
					}
				}
			}
		}
		isUsingXmp = true;
		XmpTime = glfwGetTime();
		xmpBursterNum--;
	}
}

void Player::shoot(vector<Enemy> &enemyVec)
{
	for (Enemy &enemy : enemyVec) {
		glm::vec3 ObjVec = enemy.position - Position;
		float theta = glm::dot(glm::normalize(ObjVec), glm::normalize(Front));
		float distance = sqrt(glm::dot(ObjVec, ObjVec));
		if (theta >= distance / sqrt(distance*distance + 5.0f * 5.0f) && distance <= 50.0f && enemy.live)
		{
			cout << "get it!";
			enemy.health -= 50.1f;
			if (enemy.health <= 0) {
				enemy.deadtime = glfwGetTime();
				enemy.live = false;
			}
			break;
		}
	}
	isShoot = true;
	ShootTime = glfwGetTime();
}

void Player::chat(glm::vec3 b2position)
{
	
}