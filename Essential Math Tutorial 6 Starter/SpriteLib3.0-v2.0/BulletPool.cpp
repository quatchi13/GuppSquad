#include "BulletPool.h"

void BulletPool::create(double x, double y, double xVel, double yVel, int lifetime)
{
    // Find an available particle.
    for (int i = 0; i < POOL_SIZE; i++)
    {
        if (!bullets_[i].inUse())
        {
            bullets_[i].init(x, y, xVel, yVel, lifetime);
            std::cout << "Bullet created \n";
            return;
        }
    }
}
