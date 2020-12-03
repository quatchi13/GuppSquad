#pragma once
#include "Vector.h"
#include "Scene.h"
#include "PhysicsPlaygroundListener.h"

class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;
	
	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;
	
	void makeImage(std::string filename, int width, int height, float opacity, float x, float y, float z);

	void makeStaticObject(std::string filename, int width, int height, int x, int y, int z, int physx, int physy, float shrinkX, float shrinkY, EntityCategories type, float r, float g, float b, float opacity, int rotate);

	void makeShotTrigger(int index, int x, int y, int z, int physX, int physY);

	void makeEnemy(int index, int x, int y, float physx, float physy, float shrinkx, float shrinky, int r, int g, int b, float opacity);

	void makeBullet(int index);

	void fireBullet();

protected:
	PhysicsPlaygroundListener listener;

	int spawnCount = 0;
	int tempBullet;
	
	std::vector <int> bullet{ 0 };
	std::vector <int> enemies{ 0 };
	std::vector <int> shotTriggers{ 0 };
	std::vector <int> hostileBullets{ 0 };
	
	int deletedBullets = 0;
	int deletedEnemies = 0;
	int dummy = 0;

	int score = 0;
};
