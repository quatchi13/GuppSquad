#pragma once
#include "Vector.h"
#include "Scene.h"
#include "PhysicsPlaygroundListener.h"

class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground();

	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	void Aiming(vec2 mousePosition);

	void MouseMotion(SDL_MouseMotionEvent evnt);

	void MouseDown(SDL_MouseButtonEvent evnt);

	void MouseClick(SDL_MouseButtonEvent evnt);

	//void MouseEvent(SDL_MouseButtonEvent evnt);

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

	void makeImage(std::string filename, int width, int height, float opacity, float x, float y, float z);

	void makeStaticObject(int index);

	void makeShotTrigger(int index);

	void makeEnemy(int index);

	void makeBullet(int index);

	void makeMovingBarrier(int physx, int physy);

	void makeMovingBackBarrier(int physx, int physy);

	void makeStaticBarrier(int physx, int physy);

	void makeHostileBullet(int index);

	b2Vec2 mouseLocation;

	void fireBullet();

	std::vector <int> bullet{ 0 };
protected:
	PhysicsPlaygroundListener listener;
	int endCard;

	bool hasStarted = false;
	bool hasEnded = false;

	int spawnCount = 5;
	int totalEnt = 117;
	int tempBullet;
	int vectorPush;

	std::vector <int> enemies{ 0 };
	std::vector <int> activeBullets{ 0 };
	std::vector <int> bulletHold{ 0 };
	std::vector <int> shotTriggers{ 0 };
	std::vector <int> hostileBullets{ 0 };
	std::vector <int> platforms{ 0 };

	int deletedBullets = 0;
	int deletedEnemies = 0;
	int backBarrier;
	int frontBarrier;

	int score = 0;

	int positionArray[117][2]{
		{440, 40},
		{860, 20},
		{1280, -30},
		{1690, -30},
		{2120, 70},
		{2140, -30},
		{2170, -30},
		{2190, 70},
		{2540, 20},
		{2560, 50},
		{2590, 56},
		{2610, 30},
		{2850, 70},
		{2950, 60},
		{3000, -30},
		{3060, 30},
		{3150, 70},
		{3180, 50},
		{3210, 30},
		{3260, 70},
		{3290, -30},
		{3360, 30},
		{3400, 70},
		{3460, 40},
		{3490, -30},
		{3580, 30},
		{3610, 50},
		{3730, 70},
		{3780, 30},
		{3850, 50},
		{3880, -30},
		{3950, -30},
		{3990, 50},
		{4400, 50},
		{4440, 20},
		{4480, 40},
		{4510, 70},
		{4540, -30},
		{4590, 20},
		{4640, 40},
		{4680, 70},
		{4750, -30},
		{4800, 70},
		{4850, 20},
		{4900, 70},
		{4980, 70},
		{5020, 20},
		{5060, 70},
		{5100, 50},
		{5180, 70},
		{5230, 20},
		{5290, -30},
		{5340, 50},
		{5380, -30},
		{5420, 70},
		{5470, -30},
		{5520, 50},
		{5560, 20},
		{5710, -30},
		{5750, 70},
		{5800, 50},
		{5850, 40},
		{5900, 20},
		{6000, 70},
		{ 6100, 70},
		{ 6150, -30},
		{ 6200, 40},
		{ 6250, 20},
		{ 6550, 50},
		{ 6600, 40},
		{ 6650, 70},
		{ 6700, -30},
		{ 6750, 50},
		{ 6800, 40},
		{6850, 20},
		{7270, 40},
		{7700, -30},
		{7900, -30},
		{8100, 70},
		{8200, 30},
		{8300, 30},
		{8400, 30},
		{8500, 30},
		{9650, 30},
		{9700, 50},
		{9750, 70},
		{10000, 70},
		{10050, 50},
		{10100, 30},
		{10300, 70},
		{10350, -30},
		{10400, 30},
		{10550, 38},
		{10650, 70},
		{10750, 30},
		{10850, 40},
		{10950, -30},
		{11150, 38},
		{11250, 40},
		{11270, 50},
		{11350, -30},
		{11370, 30},
		{11670, 50},
		{11770, 30},
		{12120, 70},
		{12195, 50},
		{12220, 30},
		{12470, 70},
		{12495, 30},
		{12520, -10},
		{12570, -30},
		{12870, 50},
		{12980, 30},
		{13070, 40},
		{13170, -30},
		{13795, 50},
		{13870, 30}
	};

	
};
