#pragma once

#include "camera.h"
#include "Portal.h"
#include "Resonator.h"
#include "shader_m.h"
#include "Enemy.h"

#include <vector>
#include <math.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <cstdlib>

// hack�뾶
#define RADIUS (15.0f)
// Portal Size
#define PSIZE (0.8f)
// hackTime interval
#define HACKTIME (5.0f)
// ը�������÷�Χ
#define XMPRADIUS (8.0f)
// �˺�
#define HURT (100)

class Player : public Camera
{
public:
	Player();
	~Player();

	void hack(std::vector<Portal> &portalVec);
	// ʹ�õ���
	void UseResonator(std::vector<Portal> &portalVec);
	void UsePowerCube();
	void UseXmpBurster(std::vector<Portal> &portalVec);
	// ��������
	int resonatorNum;
	int xmpBursterNum;
	int powerCubeNum;

	void shoot(vector<Enemy> &enemyVec);
	void chat(glm::vec3 b2position);

	int xm;
	bool camp;

	bool getR;
	bool getX;
	bool getC;
	bool isWin;
	bool isShoot;
	float ShootTime;
	float HackTime;

	bool isUsingXmp;
	float XmpTime;
	glm::vec3 HackPosition;
};

