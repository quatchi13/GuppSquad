#pragma once
#include <string>
#include "ECS.h"
#include "PhysicsPlayground.h"
#include "Scene.h"
class Bullet: public PhysicsPlayground
{
    friend class BulletPool;
	public:
        Bullet()
            : framesLeft_(0) 
        {}
  void init(double x, double y,
            double xVel, double yVel, int lifetime)
  {
    x_ = x; y_ = y;
    xVel_ = xVel; yVel_ = yVel;
    framesLeft_ = lifetime;
  }

  void animate()
  {
    if (!inUse()) return;

    framesLeft_--;
    x_ += xVel_;
    y_ += yVel_;
  }

  bool inUse() const { return framesLeft_ > 0; }

private:
  int framesLeft_;
  double x_, y_;
  double xVel_, yVel_;
};

