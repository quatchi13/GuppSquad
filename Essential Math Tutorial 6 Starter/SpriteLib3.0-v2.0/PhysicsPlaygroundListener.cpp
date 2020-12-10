#include "PhysicsPlaygroundListener.h"
#include "ECS.h"

PhysicsPlaygroundListener::PhysicsPlaygroundListener()
	: b2ContactListener()
{

}

void PhysicsPlaygroundListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	
	//if neither or both are sensors, will be false
	if ((sensorA ^ sensorB))
	{
		if (sensorA)
		{
			TriggerEnter(fixtureA);
		}
		else if (sensorB)
		{
			TriggerEnter(fixtureB);
		}
	}

	b2Filter filterA = fixtureA->GetFilterData();
	b2Filter filterB = fixtureB->GetFilterData();

	if ((filterA.categoryBits == PLAYER && filterB.categoryBits == GROUND) || (filterB.categoryBits == PLAYER && filterA.categoryBits == GROUND))
	{
		if (filterA.categoryBits == PLAYER)
		{
			ECS::GetComponent<CanJump>((int)fixtureA->GetBody()->GetUserData()).m_canJump = true;
			
			
		}
		else if (filterB.categoryBits == PLAYER)
		{
			ECS::GetComponent<CanJump>((int)fixtureB->GetBody()->GetUserData()).m_canJump = true;
			
		}
	}
	if ((filterA.categoryBits == ENEMY && filterB.categoryBits == BULLET) || (filterB.categoryBits == ENEMY && filterA.categoryBits == BULLET))
	{
		if (filterA.categoryBits == ENEMY)
		{
			//ECS::DestroyEntity((int)fixtureA->GetBody()->GetUserData());
			ECS::GetComponent<IsInactive>((int)fixtureA->GetBody()->GetUserData()).m_ready = true;
			ECS::GetComponent<IsInactive>((int)fixtureB->GetBody()->GetUserData()).m_notInUse = true;
			score += 700;
			std::cout << "Score: " << score << '\n';
		}
		else if (filterB.categoryBits == ENEMY)
		{
			ECS::GetComponent<IsInactive>((int)fixtureB->GetBody()->GetUserData()).m_ready = true;
			ECS::GetComponent<IsInactive>((int)fixtureA->GetBody()->GetUserData()).m_notInUse = true;
			score += 700;
			std::cout << "Score: " << score << '\n';
		}
	}

	if ((filterA.categoryBits == BULLET && filterB.categoryBits == BOUNDARY) || (filterB.categoryBits == BULLET && filterA.categoryBits == BOUNDARY))
	{
		if (filterA.categoryBits == BULLET)
		{
			ECS::GetComponent<IsInactive>((int)fixtureA->GetBody()->GetUserData()).m_notInUse = true;
			
		}
		else if (filterB.categoryBits == BULLET)
		{
			ECS::GetComponent<IsInactive>((int)fixtureB->GetBody()->GetUserData()).m_notInUse = true;
		}
	}

	if ((filterA.categoryBits == EBULLET && filterB.categoryBits == BOUNDARY) || (filterB.categoryBits == EBULLET && filterA.categoryBits == BOUNDARY))
	{
		if (filterA.categoryBits == EBULLET)
		{
			ECS::GetComponent<IsInactive>((int)fixtureA->GetBody()->GetUserData()).m_despawned = true;
			
		}
		else if (filterB.categoryBits == EBULLET)
		{
			ECS::GetComponent<IsInactive>((int)fixtureB->GetBody()->GetUserData()).m_despawned = true;
			
		}
	}

	if ((filterA.categoryBits == EBULLET && filterB.categoryBits == PLAYER) || (filterB.categoryBits == EBULLET && filterA.categoryBits == PLAYER))
	{
		if (filterA.categoryBits == EBULLET)
		{
			ECS::GetComponent<IsInactive>((int)fixtureA->GetBody()->GetUserData()).m_despawned = true;
			score -= 300;
			std::cout << "Score: " << score << '\n';
		}
		else if (filterB.categoryBits == EBULLET)
		{
			ECS::GetComponent<IsInactive>((int)fixtureB->GetBody()->GetUserData()).m_despawned = true;
			score -= 300;
			std::cout << "Score: " << score << '\n';
		}
	}

}

void PhysicsPlaygroundListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	//if neither or both are sensors, will be false
	if ((sensorA ^ sensorB))
	{
		if (sensorA)
		{
			TriggerExit(fixtureA);
		}
		else if (sensorB)
		{
			TriggerExit(fixtureB);
		}
	}
}

void PhysicsPlaygroundListener::SetTimer(int start)
{
	timer = start;
}

void PhysicsPlaygroundListener::AddTime(float add)
{
	timer += add;
}

float PhysicsPlaygroundListener::GetTimer()
{
	return timer;
}

void PhysicsPlaygroundListener::TriggerEnter(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnEnter();
}

void PhysicsPlaygroundListener::TriggerExit(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnExit();
}


