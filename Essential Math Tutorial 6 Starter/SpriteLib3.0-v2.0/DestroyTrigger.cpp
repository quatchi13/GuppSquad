#include "DestroyTrigger.h"
#include "ECS.h"

void DestroyTrigger::OnTrigger()
{
	Trigger::OnTrigger();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			PhysicsBody::m_bodiesToDelete.push_back(m_targetEntities[i]);
		}

		triggered = true;
	}
}

//void DestroyTrigger::OnEnter()
//{
//	Trigger::OnEnter();
//
//	if (!triggered)
//	{
//		for (int i = 0; i < m_targetEntities.size(); i++)
//		{
//			PhysicsBody::m_bodiesToDelete.push_back(m_targetEntities[i]);
//		}
//
//		triggered = true;
//	}
//}

void DestroyTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			//make bullet visible
			auto& ebSpr = ECS::GetComponent<Sprite>(m_targetEntities[i]);

			ebSpr.SetTransparency(1.f);

			auto& enBul = ECS::GetComponent<PhysicsBody>(m_targetEntities[i]);


			enBul.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
			//shoot bullet

			auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

			vec3 source((enBul.GetBody()->GetPosition().x), (enBul.GetBody()->GetPosition().y), 3);
			vec3 target((player.GetBody()->GetPosition().x), (player.GetBody()->GetPosition().y), 3);

			b2Vec2 shotVec(target.x - source.x, target.y - source.y);
			b2Vec2 unitVec = shotVec;
			double divisor = sqrt(pow(shotVec.x, 2) + pow(shotVec.y, 2));
			unitVec.x /= divisor;
			unitVec.y /= divisor;

			enBul.GetBody()->ApplyForceToCenter(b2Vec2((unitVec.x * 25000000), 0.f), true);
			enBul.GetBody()->ApplyForceToCenter(b2Vec2(0.f, (unitVec.y * 25000000)), true);

			std::cout << "boom!";

		}

		triggered = true;
	}
}

void DestroyTrigger::OnExit()
{
	Trigger::OnExit();
}
