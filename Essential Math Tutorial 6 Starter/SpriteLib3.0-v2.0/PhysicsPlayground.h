#pragma once

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

	void makeDestroyTrigger(int length, int width, int x, int y, int z, int target, float shrinkX, float shrinkY, int physX, int physY, EntityCategories type, EntityCategories canActivate, float r, float g, float b, float opacity);

	void makeEnemy(int x, int y, float physx, float physy, float shrinkx, float shrinky, int r, int g, int b, float opacity);
protected:
	PhysicsPlaygroundListener listener;

	int ball = 0;
};
