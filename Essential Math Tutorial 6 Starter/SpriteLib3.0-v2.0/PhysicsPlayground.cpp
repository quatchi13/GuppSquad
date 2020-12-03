#include "PhysicsPlayground.h"
#include "PhysicsPlaygroundListener.h"
#include "Utilities.h"
#include "Vector.h"
#include <vector>
#include <random>


PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{	
	m_gravity = b2Vec2(0.f, -98.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	PlaySound(TEXT("music.wav"), NULL, SND_ASYNC);



	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup MainCamera Entity
	{
		/*Scene::CreateCamera(m_sceneReg, vec4(-75.f, 75.f, -75.f, 75.f), -100.f, 100.f, windowWidth, windowHeight, true, true);*/

		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		ECS::GetComponent<Camera>(entity).SetPosition( 50, 40, 50);

		//Attaches the camera to vert and horiz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//player entity
	{
		/*Scene::CreatePhysicsSprite(m_sceneReg, "LinkStandby", 80, 60, 1.f, vec3(0.f, 30.f, 2.f), b2_dynamicBody, 0.f, 0.f, true, true)*/

		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<CanJump>(entity);

		//Sets up the components
		std::string fileName = "Concept_Design_PLAYER.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 25);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(15.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(-2.f, 0.f), false, PLAYER, GROUND | TRIGGER, 0.f, 3.f);
		//tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY)/2.f), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		//std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(0, tempSpr.GetHeight() / 2.f) };
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.5);
		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(10.f);
	}

	{
		for (int i = 0; i < 2; i++) {
			std::string fileName = "LinkStandby.png";
			int x, y;

			switch (i) {
			case 0:
				x = 390;
				y = 55;
				hostileBullets.erase(hostileBullets.begin());
				break;
			default:
				x = 900;
				y = 35;
			}

			auto entity = ECS::CreateEntity();
			hostileBullets.push_back(entity);

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 5, 5);
			ECS::GetComponent<Transform>(entity).SetPosition(100, 50, 3);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);

			tempSpr.SetTransparency(0.f);

			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(x, y);

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth()) / 2.f), vec2(0.f, 0.f), false, BULLET, PLAYER);
			tempPhsBody.SetColor(vec4(1, 0, 0, 0.3));
			tempPhsBody.SetRotationAngleDeg(0);
			tempPhsBody.SetGravityScale(0.f);

		}
	}


	makeImage("Background.png", 20000, 1000, 1, 8800, 10, -4);

	//Setup static Top Platform
	makeStaticObject("White_Platform.png", 15000000, 15, 30, -30, 2, 0, 0, 0, 0, GROUND, 0, 1, 0, 0.3, 0);
	//first segment
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 900, 20, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 400, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 1300, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 1500, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 1700, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	//second segment
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2000, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2100, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2150, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2200, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2300, 20, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2500, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 2700, 56, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 3100, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 3200, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 3250, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 3300, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	// on the ground (1)
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 3900, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 3950, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4000, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4150, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4250, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4300, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4650, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4700, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4750, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 4850, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	// doubles
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5000, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5100, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5250, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5350, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5500, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5600, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5700, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5800, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5900, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 5950, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 6000, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 6100, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 6200, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 6300, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	//repeat first seg
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 7000, 20, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 7500, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 7900, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 8100, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 8300, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	 // after repeat
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 9550, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 9600, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 9650, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 9800, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 9850, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10000, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10400, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10450, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10500, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10650, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	//2nd doubles
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10750, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10850, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 10950, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11050, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11150, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11250, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11350, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11450, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11550, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 11650, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	//robo down
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12150, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12200, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12250, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	//third double
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12350, 70, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12450, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12550, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12650, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12750, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12850, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 12950, 40, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 13050, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 13150, 50, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);
	makeStaticObject("Plaform.png ", 30, 8, 30, -30, 2, 13250, 30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 0);

	
	//first segment
	makeEnemy(0, 18, 25, 400, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(1, 18, 25, 900, 30, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(2, 18, 25, 1300, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(3, 18, 25, 1500, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(4, 18, 25, 1700, 72, 0, 0, 1, 0, 0, 0.3);
	//second segment
	makeEnemy(5, 18, 25, 2000, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(6, 18, 25, 2100, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(7, 18, 25, 2150, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(8, 18, 25, 2200, 72, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(9, 18, 25, 2300, 22, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(10, 18, 25, 2500, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(11, 18, 25, 2700, 58, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(12, 18, 25, 3100, 32, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(13, 18, 25, 3200, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(14, 18, 25, 3250, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(15, 18, 25, 3300, 32, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(16, 18, 25, 3400, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(17, 18, 25, 3500, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(18, 18, 25, 3600, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(19, 18, 25, 3700, 32, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(20, 18, 25, 3900, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(21, 18, 25, 3950, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(22, 18, 25, 4000, 72, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(23, 18, 25, 4150, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(24, 18, 25, 4250, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(25, 18, 25, 4300, 32, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(26, 18, 25, 4650, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(27, 18, 25, 4700, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(28, 18, 25, 4750, 38, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(29, 18, 25, 4850, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(30, 18, 25, 5000, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(31, 18, 25, 5100, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(32, 18, 25, 5250, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(33, 18, 25, 5350, -28, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(34, 18, 25, 5500, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(35, 18, 25, 5600, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(36, 18, 25, 5700, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(37, 18, 25, 5800, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(38, 18, 25, 5900, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(39, 18, 25, 5950, 72, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(40, 18, 25, 6000, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(41, 18, 25, 6100, 72, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(42, 18, 25, 6200, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(43, 18, 25, 6300, -28, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(44, 18, 25, 7000, 30, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(45, 18, 25, 7500, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(46, 18, 25, 7900, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(47, 18, 25, 8100, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(48, 18, 25, 8300, 72, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(49, 18, 25, 8400, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(50, 18, 25, 8500, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(51, 18, 25, 8600, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(52, 18, 25, 8700, 32, 0, 0, 1, 0, 0, 0.3);
	//after repeat
	makeEnemy(53, 18, 25, 9550, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(54, 18, 25, 9600, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(55, 18, 25, 9650, 72, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(56, 18, 25, 9800, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(57, 18, 25, 9850, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(58, 18, 25, 10000, 32, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(59, 18, 25, 10400, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(60, 18, 25, 10450, -28, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(61, 18, 25, 10500, 38, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(62, 18, 25, 10650, 38, 0, 0, 1, 0, 0, 0.3);
	//2nd double
	makeEnemy(63, 18, 25, 10750, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(64, 18, 25, 10850, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(65, 18, 25, 10950, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(66, 18, 25, 11050, -28, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(67, 18, 25, 11150, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(68, 18, 25, 11250, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(69, 18, 25, 11350, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(70, 18, 25, 11450, -28, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(71, 18, 25, 11550, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(72, 18, 25, 11650, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(73, 18, 25, 11800, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(74, 18, 25, 11900, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(75, 18, 25, 12000, 32, 0, 0, 1, 0, 0, 0.3);
	//robo down
	makeEnemy(76, 18, 25, 12150, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(77, 18, 25, 12200, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(78, 18, 25, 12250, 32, 0, 0, 1, 0, 0, 0.3);
	//third double
	makeEnemy(79, 18, 25, 12350, 72, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(80, 18, 25, 12450, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(81, 18, 25, 12550, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(82, 18, 25, 12650, -28, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(83, 18, 25, 12750, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(84, 18, 25, 12850, 38, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(85, 18, 25, 12950, 42, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(86, 18, 25, 13050, -28, 0, 0, 1, 0, 0, 0.3);

	makeEnemy(87, 18, 25, 13150, 52, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(88, 18, 25, 13250, 38, 0, 0, 1, 0, 0, 0.3);
	//outro
	makeEnemy(89, 18, 25, 13400, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(90, 18, 25, 13500, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(91, 18, 25, 13600, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(92, 18, 25, 13700, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(93, 18, 25, 13800, 32, 0, 0, 1, 0, 0, 0.3);
	makeEnemy(94, 18, 25, 13900, 32, 0, 0, 1, 0, 0, 0.3);

	for (int i = 0; i < 60; i++) {
		auto entity = ECS::CreateEntity();
		bullet.push_back(entity);
		

		if (i == 0) {
			bullet.erase(bullet.begin());
		}
	}
	for (int i = 0; i < 60; i++) {
		makeBullet(i);
	}
	
	//Setup trigger
	makeShotTrigger(0, 30, -20, 80, 280, -30);
	makeShotTrigger(1, 30, -20, 80, 700, -30);
	//makeTriggerEnemy(0, 400, 22, shotTriggers[0]);

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));

	
}

void PhysicsPlayground::Update()
{
	bulletStuff:
	int active = spawnCount - deletedBullets;
	bool hasDespawned;


	if (active > 0) {
		for (int i = deletedBullets; i < spawnCount; i++) {
			
			hasDespawned = false;
			auto& bPos = ECS::GetComponent<PhysicsBody>(bullet[i]);
			
			for (int j = 0; j < enemies.size(); j++) {
				if (!hasDespawned) {

					auto& ePos = ECS::GetComponent<PhysicsBody>(enemies[j]);
					int maxX = ePos.GetBody()->GetPosition().x + 13;
					int minX = ePos.GetBody()->GetPosition().x - 13;
					int maxY = ePos.GetBody()->GetPosition().y + 9;
					int minY = ePos.GetBody()->GetPosition().y - 9;


					if (bPos.GetBody()->GetPosition().x < maxX && bPos.GetBody()->GetPosition().x > minX
						&& bPos.GetBody()->GetPosition().y < maxY && bPos.GetBody()->GetPosition().y > minY) {
						
						/*if (ECS::GetComponent<PhysicsBody>(hostileBullets[j]).GetVelocity().x == 0) {
							ECS::DestroyEntity(hostileBullets[j]);
							hostileBullets.erase(hostileBullets.begin() + j);
						}*/
						
						//ECS::GetComponent<PhysicsBody>(shotTriggers[j]).SetPosition(b2Vec2(-5.f, 0.f));
						//shotTriggers.erase(shotTriggers.begin() + j);
						ECS::DestroyEntity(enemies[j]);
						enemies.erase(enemies.begin() + j);
						ECS::DestroyEntity(bullet[i]);
						bullet.erase(bullet.begin() + i);

						hasDespawned = true;
						deletedBullets++;

						score += 500;
						std::cout << '\n' << score << '\n';
					}
				}
			}

			if (!hasDespawned) {
					auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
					if (bPos.GetBody()->GetPosition().x < (player.GetBody()->GetPosition().x - 60) 
						|| bPos.GetBody()->GetPosition().x > (player.GetBody()->GetPosition().x + 200)
						|| bPos.GetBody()->GetPosition().y < (player.GetBody()->GetPosition().y - 60) 
						|| bPos.GetBody()->GetPosition().y > (player.GetBody()->GetPosition().y + 200)) {
						
						ECS::DestroyEntity(bullet[i]);
						bullet.erase(bullet.begin() + i);
						hasDespawned = true;
						deletedBullets++;
					}

				}

			if (hasDespawned) {
				std::cout << "\nzap!\n";
				goto bulletStuff;
			}
		}

		//std::cout << '\n' << bullet[0] << " " << enemies[0] << " " << shotTriggers[0];

	}


}
PhysicsPlaygroundListener timer;

void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());
	
	float speed = 0.6f;
	int vel = 0;
	vec3 velocity(90, 0, 0);

	if (Input::GetKey(Key::A))
	{
		//player.GetBody()->ApplyForceToCenter(b2Vec2(-500000.f * speed, 0.f), true);
		player.SetVelocity(vec3(-90, 0, 0));

		//std::cout << bullet[0] << " " << enemies[0] << " " << shotTriggers[0] << '\n';
		//std::cout << bullet[1] << " " << enemies[1] << " " << shotTriggers[1] << '\n';
	}
	if (player.GetPosition().y <= 20.015) {
		canJump.m_canJump = true;
	}
	if (canJump.m_canJump)
	{
		if (player.GetPosition().y <= 20.015) {
			timer.SetTimer(0);
		}
		if (Input::GetKey(Key::Space))
		{
			if (timer.GetTimer() < 0.4) {
				 timer.AddTime(Timer::deltaTime);
				vel += 70;
			}
			else
			{
				canJump.m_canJump = false;
			}
			
		}
		else
		{
			canJump.m_canJump = false;
		}
	}
	if (Input::GetKey(Key::U))
	{
		std::cout << player.GetPosition().y;
	}
	player.SetVelocity(vec3(90, vel, 0));
}

void PhysicsPlayground::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());

	if (Input::GetKeyDown(Key::Q))
	{
		
	}

	if (Input::GetKeyDown(Key::T))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}
	

	if (Input::GetKeyDown(Key::S)) {
		fireBullet();
	}
}

void PhysicsPlayground::KeyboardUp()
{
	

}


void PhysicsPlayground::makeImage(std::string filename, int width, int height, float opacity, float x, float y, float z)
{
	/*Scene::CreateSprite(m_sceneReg, "HelloWorld.png", 100, 60, 0.5f, vec3(0.f, 0.f, 0.f));*/

		//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);

	//Set up the components
	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);
	ECS::GetComponent<Sprite>(entity).SetTransparency(opacity);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, z));
}

void PhysicsPlayground::makeStaticObject(std::string filename, int width, int height, int x, int y, int z, 
												int physx, int physy, float shrinkX, float shrinkY, 
													EntityCategories type, float r, float g, float b, float opacity, int rotate)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, z));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(physx), float32(physy));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
		float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, type, PLAYER | ENEMY);
	tempPhsBody.SetColor(vec4(r, g, b, opacity));
	tempPhsBody.SetRotationAngleDeg(rotate);
}


void PhysicsPlayground::makeShotTrigger(int index, int x, int y, int z, int physX, int physY)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	shotTriggers.push_back(entity);

	if (index == 0) {
		shotTriggers.erase(shotTriggers.begin());
	}


	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	//space
	std::string fileName = "nothingness.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 1, 200);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, z));
	ECS::GetComponent<Trigger*>(entity) = new EnemyShotTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(MainEntities::MainPlayer());
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(hostileBullets[index]);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);


	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(physX), float32(physY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()), float(tempSpr.GetHeight()), vec2(0.f, 0.f), true, TRIGGER, PLAYER);
	tempPhsBody.SetColor(vec4(1, 0, 0, 0.3));
}

void PhysicsPlayground::makeEnemy(int index, int x, int y, float physx, float physy, float shrinkx, float shrinky, int r, int g, int b, float opacity)
{
	

	auto entity = ECS::CreateEntity();
	enemies.push_back(entity);
	if (index == 0) {
		enemies.erase(enemies.begin());
	}

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<CanJump>(entity);

	//Sets up the components
	std::string fileName = "Concept_Design_ENEMY.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 18, 25);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 3.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(physx), float32(physy));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkx), float(tempSpr.GetHeight() - shrinky), vec2(-2.f, 0.f), false, ENEMY, ENVIRONMENT | GROUND, 0.5f, 3.f);
	tempPhsBody.SetRotationAngleDeg(0.f);
	tempPhsBody.SetFixedRotation(true);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
	tempPhsBody.SetGravityScale(2.f);
}


void PhysicsPlayground::makeBullet(int index) {

	std::string fileName = "Bullet.png";
	vec3 source(-100, 10, 3);
	auto entity = bullet[index];

	tempBullet = entity;

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 5, 5);
	ECS::GetComponent<Transform>(entity).SetPosition(source);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(source.x, source.y);

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight()) / 2.f), vec2(0.f, 0.f), false, OBJECTS, TRIGGER);
	tempPhsBody.SetColor(vec4(1, 0, 0, 0.3));
	tempPhsBody.SetRotationAngleDeg(0);
	tempPhsBody.SetGravityScale(0.f);

}

void PhysicsPlayground::fireBullet() {
	b2Vec2 source((ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetBody()->GetPosition().x + 10),
		(ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetBody()->GetPosition().y));

	auto& pBullet = ECS::GetComponent<PhysicsBody>(bullet[spawnCount]);
	pBullet.SetPosition(b2Vec2(source));
	
	spawnCount++;

	vec2 targetCoord(pBullet.GetBody()->GetPosition().x + 100, pBullet.GetBody()->GetPosition().y);
	vec2 startCoord(pBullet.GetBody()->GetPosition().x, pBullet.GetBody()->GetPosition().y);

	b2Vec2 shotVect(targetCoord.x - startCoord.x, targetCoord.y - startCoord.y);
	b2Vec2 unitVect = shotVect;
	double divisor = sqrt(pow(shotVect.x, 2) + pow(shotVect.y, 2));
	unitVect.x /= divisor;
	unitVect.y /= divisor;

	vec3 velocity(unitVect.x * 500000000000, unitVect.y * 500000000000, 0);

	pBullet.SetVelocity(velocity);

	std::cout << "pew!";
}


//artificial ameteurs arent at all amazing analytically i assault animate things broken barriers bounded by the bomb beat buildings are broken basically im bombarding 
//casually compete catastrophes casualties canceling cats got their canopies collapsing detonate a dime a dank daily doing dough demonstrations don dada on the down low
