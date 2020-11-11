#include "PhysicsPlayground.h"
#include "Utilities.h"
#include <random>


PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{	
	//No gravity this is a top down scene
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

		//Attaches the camera to vert and horiz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//Link entity
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
		std::string fileName = "LinkStandby.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 40, 30);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(30.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY)/2.f), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		//std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(0, tempSpr.GetHeight() / 2.f) };
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.5);
		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(1.f);
	}

	makeImage("HelloWorld.png", 100, 80, 0.5, 0, 0, 0);

	//Setup static Top Platform
	makeStaticObject("boxSprite.jpg", 150, 10, 30, -20, 2, 30, -10, 0, 0, GROUND, 0, 1, 0, 0.3, 0);
	makeStaticObject("boxSprite.jpg", 80, 10, 30, -20, 2, 137.1, -29.3, 0, 0, GROUND, 0, 1, 0, 0.3, -30);
	makeStaticObject("boxSprite.jpg", 150, 10, 30, -20, 2, 240, -50, 0, 0, GROUND, 0, 1, 0, 0.3, 0);
	makeStaticObject("boxSprite.jpg", 50, 15, 30, -20, 2, 325, -30, 0, 0, ENVIRONMENT, 0, 1, 0, 0.3, 90);
	




	//Ball
	{
		auto entity = ECS::CreateEntity();
		ball = entity;
		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up the components
		std::string fileName = "BeachBall.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -8.f, 3.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(45.f), float32(-8.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.3f);

		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
	}

	//Setup trigger
	makeDestroyTrigger(40, 40, 30, -20, 80, 7, 0, 0, 300, -30, TRIGGER, PLAYER, 1, 0, 0, 0.3);

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));

	
}

void PhysicsPlayground::Update()
{
	
}


void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	float speed = 1.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);

	if (Input::GetKey(Key::Shift))
	{
		speed *= 5.f;
	}

	if (Input::GetKey(Key::A))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(-400000.f * speed, 0.f), true);
	}
	if (Input::GetKey(Key::D))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(400000.f * speed, 0.f), true);
	}

	//Change physics body size for circle
	if (Input::GetKey(Key::O))
	{
		player.ScaleBody(1.3 * Timer::deltaTime, 0);
	}
	else if (Input::GetKey(Key::I))
	{
		player.ScaleBody(-1.3 * Timer::deltaTime, 0);
	}
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
	if (canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, 160000.f), true);
			canJump.m_canJump = false;
		}
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


void PhysicsPlayground::makeDestroyTrigger(int length, int width, int x, int y, int z, int target, float shrinkX, float shrinkY, int physX, int physY, EntityCategories type, EntityCategories canActivate, float r, float g, float b, float opacity)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	//space
	std::string fileName = "nothingness.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, length, width);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, z));
	ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(target);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);


	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(physX), float32(physY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, type, canActivate);
	tempPhsBody.SetColor(vec4(r, g, b, opacity));
}


