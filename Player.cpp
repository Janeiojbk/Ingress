#include "Player.h"


Player::Player()
{
	xm = 10000;
	camp = 1;
	// 道具数量
	resonatorNum = 99;
	xmpBursterNum = 99;
	powerCubeNum = 2;
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
			if (num / 2 == 0)
				resonatorNum++;
			srand(unsigned(timeValue*timeValue*10 + 9954));
			num = rand();
			if (num / 2 == 0)
				xmpBursterNum++;
			srand(unsigned(sqrt(timeValue*10 + 3384)));
			num = rand();
			if (num / 2 == 0)
				powerCubeNum++;
		}
	}
}

void Player::UseResonator(std::vector<Portal> &portalVec)
{
	for (Portal &portal : portalVec) {
		glm::vec3 ObjVec = portal.position - Position;
		float distance = sqrt(glm::dot(ObjVec, ObjVec));
		if (distance <= RADIUS && resonatorNum > 0) {
			--resonatorNum;
			for (int i = 0; i != 8; i++) {
				if (portal.resonator[i].index == -1) {
					portal.resonator[i] = Resonator(portal.position, i, distance);
					break;
				}
			}
		}
	}
}
void Player::UsePowerCube()
{
	if (xmpBursterNum > 0) {
		xmpBursterNum--;
		if (xm <= 2000)
			xm += 8000;
		else
			xm = 10000;
	}
}
void Player::UseXmpBurster(std::vector<Portal> &portalVec)
{
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
}