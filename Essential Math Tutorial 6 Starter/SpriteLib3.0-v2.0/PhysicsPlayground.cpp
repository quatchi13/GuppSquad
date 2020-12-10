#include "PhysicsPlayground.h"
#include "PhysicsPlaygroundListener.h"
#include "Utilities.h"
#include "Vector.h"
#include <vector>
#include <random>


PhysicsPlayground::PhysicsPlayground()
{
}

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{	
	m_gravity = b2Vec2(0.f, -98.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
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
		ECS::AttachComponent<Player>(entity);
        ECS::AttachComponent<AnimationController>(entity);

        //Sets up the components
        std::string fileName = "spritesheets/Playerpain.png";
        std::string animations = "Playerpain.json";
        ECS::GetComponent<Player>(entity).InitPlayer(fileName, animations, 25, 25, &ECS::GetComponent<Sprite>(entity),
            &ECS::GetComponent<AnimationController>(entity), &ECS::GetComponent<Transform>(entity));
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 25);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 8.f;
		float shrinkY = 8.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(15.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(-2.f, 0.f), false, PLAYER, GROUND | TRIGGER | EBULLET, 0.f, 3.f);
		//tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY)/2.f), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		//std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(0, tempSpr.GetHeight() / 2.f) };
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.5);
		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(40.f);
	}

	makeStaticBarrier(0, 115);
	makeStaticBarrier(0, -35);
	makeMovingBarrier(235, 0);
	makeMovingBackBarrier(-50, 0);
	
	makeImage("Background.png", 25000, 150, 1, 12000, 40, -4);
	makeImage("controls.png", 100, 100, 1, 90, 60, -2);
	//makeImage("endcard.png", 150, 150, 1, 41375, 60, -1);
	makeImage("End.png", 200, 150, 0, 14380, 75, 2);

	{
		std::string filename = "White_Platform.png";
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 15000000, 15);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(30, -30, 2));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()),
			float(tempSpr.GetHeight()), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY);
		tempPhsBody.SetColor(vec4(0, 1, 0, 0.3));
	}

	for (int i = 0; i < 5; i++) {
		makeStaticObject(i);
		makeEnemy(i);
		makeHostileBullet(i);
		makeShotTrigger(i);
	}

	for (int i = 0; i < 5; i++)
	{
		auto entity = ECS::CreateEntity();
		bulletHold.push_back(entity);

		makeBullet(i + 1);
	}
	

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{
	//animations
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& pain = ECS::GetComponent<Player>(MainEntities::MainPlayer());
	pain.Update();


	//cursed juggling
	for (int i = 0; i < enemies.size(); i++) {
		bool defeated = (ECS::GetComponent<IsInactive>(enemies[i]).m_ready);

		if (defeated) {
			ECS::GetComponent<IsInactive>(enemies[i]).m_ready = false;
			int newPos = ECS::GetComponent<PhysicsBody>(enemies[i]).GetPosition().x;
			ECS::GetComponent<PhysicsBody>(enemies[i]).SetPosition(b2Vec2(newPos, 200));
			
			ECS::GetComponent<PhysicsBody>(shotTriggers[i]).SetPosition(b2Vec2(newPos, 200));

			if (ECS::GetComponent<PhysicsBody>(hostileBullets[i]).GetVelocity().x == 0) {
				ECS::GetComponent<PhysicsBody>(hostileBullets[i]).SetPosition(b2Vec2(newPos, 200));
			}	
		}
	}

	for (int i = 0; i < hostileBullets.size(); i++) {
		if(ECS::GetComponent<IsInactive>(hostileBullets[i]).m_despawned == true){
			ECS::GetComponent<Sprite>(hostileBullets[i]).SetTransparency(0.f);
			ECS::GetComponent<IsInactive>(hostileBullets[i]).m_despawned = false;
		}
	}

	for (int i = 0; i < platforms.size(); i++) {
		if ((ECS::GetComponent<PhysicsBody>(platforms[i]).GetBody()->GetPosition().x + 15) < 
			(ECS::GetComponent<PhysicsBody>(backBarrier).GetBody()->GetPosition().x)) {
			
			if (spawnCount < totalEnt) {
				b2Vec2 positions(positionArray[spawnCount][0], positionArray[spawnCount][1]);
				ECS::GetComponent<PhysicsBody>(platforms[i]).SetPosition(positions);
				platforms.push_back(platforms[i]);
				ECS::GetComponent<PhysicsBody>(enemies[i]).SetPosition(b2Vec2(positions.x, positions.y + 13));
				enemies.push_back(enemies[i]);
				ECS::GetComponent<Sprite>(hostileBullets[i]).SetTransparency(0.f);
				ECS::GetComponent<PhysicsBody>(hostileBullets[i]).SetVelocity(vec3(0.f, 0.f, 0.f));
				ECS::GetComponent<PhysicsBody>(hostileBullets[i]).SetPosition(b2Vec2(positions.x - 10, positions.y + 20));
				hostileBullets.push_back(hostileBullets[i]);
				ECS::GetComponent<PhysicsBody>(shotTriggers[i]).SetPosition(b2Vec2(positions.x - 120, -30));
				shotTriggers.push_back(shotTriggers[i]);
				spawnCount++;
			}
			else {
				ECS::GetComponent<PhysicsBody>(platforms[i]).DeleteBody();
				ECS::DestroyEntity(platforms[i]);
				ECS::GetComponent<PhysicsBody>(enemies[i]).DeleteBody();
				ECS::DestroyEntity(enemies[i]);
				ECS::GetComponent<PhysicsBody>(hostileBullets[i]).DeleteBody();
				ECS::DestroyEntity(hostileBullets[i]);
				ECS::GetComponent<PhysicsBody>(shotTriggers[i]).DeleteBody();
				ECS::DestroyEntity(shotTriggers[i]);
			}
			platforms.erase(platforms.begin() + i);
			enemies.erase(enemies.begin() + i);
			hostileBullets.erase(hostileBullets.begin() + i);
			shotTriggers.erase(shotTriggers.begin() + i);
		}
	}

	if (activeBullets.size() > 1) {
		for (int i = 1; i < activeBullets.size(); i++) {
			bool inactive = ECS::GetComponent<IsInactive>(activeBullets[i]).m_notInUse;

			if (inactive) {
				ECS::GetComponent<PhysicsBody>(activeBullets[i]).SetVelocity(vec3(0.f, 0.f, 0.f));
				ECS::GetComponent<PhysicsBody>(activeBullets[i]).SetPosition(b2Vec2(100, 300));
				bulletHold.push_back(activeBullets[i]);
				activeBullets.erase(activeBullets.begin() + i);
			}
		}
	}

	if (player.GetPosition().x > 14250) {
		hasEnded = true;
		ECS::GetComponent<Sprite>(endCard).SetTransparency(1.f);
	}

}

void PhysicsPlayground::Aiming(vec2 mousePosition)
{
	vec3 playerLoc = ECS::GetComponent<Transform>(MainEntities::MainPlayer()).GetPosition();

	vec2 convertedMouse = mousePosition;

	float dx = playerLoc.x - convertedMouse.x;
	float dy = playerLoc.y - convertedMouse.y;
	//std::cout << convertedMouse.x << " | " << convertedMouse.y << "\n";
	mouseLocation = b2Vec2(convertedMouse.x, convertedMouse.y);
	float playerDir = (atan2(dy, dx));
}

void PhysicsPlayground::MouseMotion(SDL_MouseMotionEvent evnt)
{
	
	Aiming(Util::ConvertToGL(m_sceneReg, vec2(float(evnt.x), float(evnt.y))));
}

void PhysicsPlayground::MouseClick(SDL_MouseButtonEvent evnt)
{
	
	if (evnt.state == SDL_PRESSED)
	{
		Aiming(Util::ConvertToGL(m_sceneReg, vec2(float(evnt.x), float(evnt.y))));
		fireBullet();

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
				vel += 140;
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
	if (hasStarted == true && hasEnded == false) {
		player.SetVelocity(vec3(70, vel, 0));
	}
	else {
		player.SetVelocity(vec3(0, vel, 0));
		ECS::GetComponent<PhysicsBody>(backBarrier).SetVelocity(vec3(0, 0, 0));
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

	if (Input::GetKeyDown(Key::Enter)) {
		if (!hasStarted) {
			player.SetVelocity(vec3(70, 0, 0));
			ECS::GetComponent<PhysicsBody>(frontBarrier).SetVelocity(vec3(70, 0, 0));
			ECS::GetComponent<PhysicsBody>(backBarrier).SetVelocity(vec3(70, 0, 0));
			hasStarted = true;
			PlaySound(TEXT("music.wav"), NULL, SND_ASYNC);
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
	if (filename == "End.png") {
		endCard = entity;
	}
	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);

	//Set up the components
	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);
	ECS::GetComponent<Sprite>(entity).SetTransparency(opacity);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, z));
}

void PhysicsPlayground::makeStaticObject(int index)
{
	std::string filename = "Plaform.png";
	//Creates entity
	auto entity = ECS::CreateEntity();
	platforms.push_back(entity);

	if (index == 0) {
		platforms.erase(platforms.begin());
	}

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 30, 8);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(30, -30, 2));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(positionArray[index][0]), float32(positionArray[index][1]));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()),
		float(tempSpr.GetHeight()), vec2(0.f, 0.f), false, ENVIRONMENT, PLAYER | ENEMY);
	tempPhsBody.SetColor(vec4(0, 1, 0, 0.3));
}

void PhysicsPlayground::makeMovingBarrier(int physx, int physy)
{
	std::string fileName = "nothingness.png";

	//Creates entity
	auto entity = ECS::CreateEntity();
	frontBarrier = entity;

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 300);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(30, -20, 2));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_kinematicBody;
	tempDef.position.Set(float32(physx), float32(physy));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()),
		float(tempSpr.GetHeight()), vec2(0.f, 0.f), false, BOUNDARY, BULLET);
	tempPhsBody.SetColor(vec4(2, 1, 0, 0.3));
	tempPhsBody.SetGravityScale(0.f);
	tempPhsBody.SetFixedRotation(true);

}

void PhysicsPlayground::makeMovingBackBarrier(int physx, int physy)
{
	std::string fileName = "nothingness.png";

	//Creates entity
	auto entity = ECS::CreateEntity();
	backBarrier = entity;

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 300);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(30, -20, 2));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_kinematicBody;
	tempDef.position.Set(float32(physx), float32(physy));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()),
		float(tempSpr.GetHeight()), vec2(0.f, 0.f), false, BOUNDARY, EBULLET | BULLET, 1.f, 1.f);
	tempPhsBody.SetColor(vec4(2, 1, 0, 0.3));
	tempPhsBody.SetGravityScale(0.f);
	tempPhsBody.SetFixedRotation(true);

}

void PhysicsPlayground::makeStaticBarrier(int physx, int physy)
{
	std::string fileName = "nothingness.png";

	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15000000, 10);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(30, -20, 2));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(physx), float32(physy));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()),
		float(tempSpr.GetHeight()), vec2(0.f, 0.f), false, BOUNDARY, BULLET | EBULLET);
	tempPhsBody.SetColor(vec4(2, 1, 0, 0.3));
	tempPhsBody.SetGravityScale(0.f);
	tempPhsBody.SetFixedRotation(true);

}

void PhysicsPlayground::makeShotTrigger(int index)
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
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(30,-20, 80));
	ECS::GetComponent<Trigger*>(entity) = new EnemyShotTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(MainEntities::MainPlayer());
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(hostileBullets[index]);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);


	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(positionArray[index][0] -120), float32(-30));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()), float(tempSpr.GetHeight()), vec2(0.f, 0.f), true, TRIGGER, PLAYER);
	tempPhsBody.SetColor(vec4(1, 0, 0, 0.3));
}

void PhysicsPlayground::makeEnemy(int index)
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
	ECS::AttachComponent<IsInactive>(entity);


	//Sets up the components
	std::string fileName = "Concept_Design_ENEMY.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 18, 25);
	ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 3.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_kinematicBody;
	tempDef.position.Set(float32(positionArray[index][0]), float32(positionArray[index][1] + 13));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth()), float(tempSpr.GetHeight()), vec2(-2.f, 0.f), false, ENEMY, ENVIRONMENT | GROUND | BULLET, 0.5f, 3.f);
	tempPhsBody.SetRotationAngleDeg(0.f);
	tempPhsBody.SetFixedRotation(true);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	tempPhsBody.SetGravityScale(0.f);
}


void PhysicsPlayground::makeBullet(int index) {

	std::string fileName = "Bullet.png";
	vec3 source(100, 300, 3);
	auto entity = bulletHold[index];

	tempBullet = entity;

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<IsInactive>(entity);

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 5, 5);
	ECS::GetComponent<Transform>(entity).SetPosition(source);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(source.x, source.y);

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight()) / 2.f), vec2(0.f, 0.f), false, BULLET, ENEMY | BOUNDARY);
	tempPhsBody.SetColor(vec4(1, 0, 0, 0.3));
	tempPhsBody.SetRotationAngleDeg(0);
	tempPhsBody.SetFixedRotation(true);
	tempPhsBody.SetGravityScale(0.f);

}

void PhysicsPlayground::makeHostileBullet(int index) {
	std::string fileName = "EBullet.png";

	auto entity = ECS::CreateEntity();
	hostileBullets.push_back(entity);
	if (index == 0) {
		hostileBullets.erase(hostileBullets.begin());
	}


	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<IsInactive>(entity);

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 5, 5);
	ECS::GetComponent<Transform>(entity).SetPosition(100, 50, 3);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);

	tempSpr.SetTransparency(0.f);

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(positionArray[index][0] - 10, positionArray[index][1] + 20);

	tempBody = m_physicsWorld->CreateBody(&tempDef);
	tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth()) / 2.f), vec2(0.f, 0.f), false, EBULLET, PLAYER);
	tempPhsBody.SetColor(vec4(1, 0, 0, 0.3));
	tempPhsBody.SetRotationAngleDeg(0);
	tempPhsBody.SetGravityScale(0.f);

}

void PhysicsPlayground::fireBullet() {
	b2Vec2 source((ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetBody()->GetPosition().x + 10),
		(ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetBody()->GetPosition().y));
	int bulletToShoot;
	if (bulletHold.size() > 1) 
	{
		activeBullets.push_back(bulletHold[1]);
		bulletHold.erase(bulletHold.begin() + 1);
	}
	else
	{
		bulletToShoot = activeBullets[1];
		activeBullets.push_back(bulletToShoot);
		activeBullets.erase(activeBullets.begin() + 1);
	}
	bulletToShoot = activeBullets.size() - 1;



	ECS::GetComponent<IsInactive>(activeBullets[bulletToShoot]).m_notInUse = false;
	auto& pBullet = ECS::GetComponent<PhysicsBody>(activeBullets[bulletToShoot]);
	pBullet.SetPosition(b2Vec2(source));
	

	vec2 targetCoord(mouseLocation.x, mouseLocation.y);
	vec2 startCoord(pBullet.GetBody()->GetPosition().x, pBullet.GetBody()->GetPosition().y);
	b2Vec2 shotVect(targetCoord.x - startCoord.x, targetCoord.y - startCoord.y);
	b2Vec2 unitVect = shotVect;
	double divisor = sqrt(pow(shotVect.x, 2) + pow(shotVect.y, 2));
	unitVect.x /= divisor;
	unitVect.y /= divisor;

	vec3 velocity(unitVect.x * 500000000000, unitVect.y * 500000000000, 0);

	pBullet.SetVelocity(velocity);

	//std::cout << "pew!";
}


//artificial ameteurs arent at all amazing analytically i assault animate things broken barriers bounded by the bomb beat buildings are broken basically im bombarding 
//casually compete catastrophes casualties canceling cats got their canopies collapsing detonate a dime a dank daily doing dough demonstrations don dada on the down low
