#pragma once
#include "Bullet.h"
class BulletPool
{
public:
    void create(double x, double y,
        double xVel, double yVel, int lifetime);

    void animate()
    {
        for (int i = 0; i < POOL_SIZE; i++)
        {
            bullets_[i].animate();
        }
    }

private:
    static const int POOL_SIZE = 10;
    Bullet bullets_[POOL_SIZE];
};

