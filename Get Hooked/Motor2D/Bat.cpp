#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1FadeScene.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Scene2.h"

#include "j1EntityManager.h"
#include "Player.h"
#include "Bat.h"

Bat::Bat() : Enemy(enemy_type::BAT)
{
	name.create("bat");
}

Bat::~Bat()
{}

// Called before render is available
bool Bat::Awake(pugi::xml_node& entities)
{
	bool ret = true;

	LOG("Loading Enemy Data");

	folder.create(entities.child("folder").child_value());

	// Character stats and flags
	ImportAllStates(entities.child("stats"));	// Import all state data from config.xml

	// Sprites
	textureName.create("%s%s", folder.GetString(), entities.child("sprites").child("spriteSheet").child_value());
	spriteSize = { entities.child("sprites").child("spriteSize").attribute("x").as_int(), entities.child("sprites").child("spriteSize").attribute("y").as_int() };

	pugi::xml_node first_sprite = entities.child("sprites").child("first_sprite");
	ImportAllSprites(first_sprite);	// Import all sprite data

	// Animations
	AllocAllAnimations();	// Allocate all animations with previously recieved sprite data

	return ret;
}

// Called before the first frame
bool Bat::Start()
{
	dead = false;
	life = maxLife;
	hitboxOffset = idleSprite.colliderOffset;
	status = enemy_state::IDLE;

	graphics = App->tex->Load(textureName.GetString());

	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_ENEMY, this);
	hitboxOffset = hitbox->rect;

	position = spawnPosition;

	return true;
}

// Called each loop iteration
bool Bat::PreUpdate()
{
	bool ret = true;

	CheckInput();		// Check player input
	CheckMovement();	// Check player current movement

	return ret;
}

// Called between a certain number of frames or times per second
bool Bat::UpdateLogic(float dt)
{
	//TODO: Pathfinding

	return true;
}

// Called each frame (framerate dependant)
bool Bat::UpdateTick(float dt)
{
	bool ret = true;

	CheckState();	// Check player state
	ApplyState();	// Add state effects like movement restrictions, animation and sounds
	Move(dt);		// Move player position and calculate other movement related factors
	UpdateHitbox();	// Transform player collider depending on new position and state

	animRect = animPtr->GetCurrentFrame(dt);

	return ret;
}

// Called each loop iteration
bool Bat::Update()
{
	bool ret = true;

	Draw(&animRect);

	return ret;
}

//// Import all state data from config.xml
//void Bat::ImportAllStates(pugi::xml_node &config)
//{
//	// Character stats
//	maxLife = (ushort)config.child("life").attribute("value").as_uint();
//	speed = { config.child("speed").attribute("x").as_float(), config.child("speed").attribute("y").as_float() };
//	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };
//	acceleration.x = config.child("accelerations").attribute("x").as_float();
//	acceleration.y = config.child("accelerations").attribute("y").as_float();
//	gravity = config.child("accelerations").attribute("gravity").as_float();
//
//	detectionRadius.x = config.child("detection").attribute("x").as_float();
//	detectionRadius.y = config.child("detection").attribute("y").as_float();
//
//	// Character status flags and directly related data
//	airborne = config.child("airborne").attribute("value").as_bool();
//	lookingRight = config.child("looking").attribute("right").as_bool();
//	deathDelay = config.child("deathDelay").attribute("miliseconds").as_uint();
//}

// Import all sprite data using the above function for each animation
void Bat::ImportAllSprites(pugi::xml_node& first_sprite)
{
	ImportSpriteData("follow", &followSprite, first_sprite);
	ImportSpriteData("idle", &idleSprite, first_sprite);
	ImportSpriteData("dead", &deadSprite, first_sprite);
}

// Allocate all animations with previously recieved sprite data
void Bat::AllocAllAnimations()
{
	followSprite.anim.AllocAnimation({ followSprite.sheetPosition.x * spriteSize.x, followSprite.sheetPosition.y * spriteSize.y }, spriteSize, followSprite.numFrames);
	idleSprite.anim.AllocAnimation({ idleSprite.sheetPosition.x * spriteSize.x, idleSprite.sheetPosition.y * spriteSize.y }, spriteSize, idleSprite.numFrames);
	deadSprite.anim.AllocAnimation({ deadSprite.sheetPosition.x * spriteSize.x, deadSprite.sheetPosition.y * spriteSize.y }, spriteSize, deadSprite.numFrames);
}

void Bat::CheckState()
{
	if (!canFly && App->collision->CheckGroundCollision(hitbox) == false)
		airborne = true;

	if (dead)
		status = enemy_state::HURT;
	else
	{
		switch (status) {
		case enemy_state::IDLE:
			if(playerInRadius)
				status = enemy_state::FOLLOWING;
			break;
		case enemy_state::PATROLING:
			if (playerInRadius)
				status = enemy_state::FOLLOWING;
			break;
		case enemy_state::FOLLOWING:
			if (!playerInRadius)
				status = enemy_state::PATROLING;
			break;
		case enemy_state::ATTACKING:
			break;
		case enemy_state::FALLING:
			break;
		case enemy_state::HURT:
			break;
		}
	}
}

void Bat::ApplyState()
{
	lookingRight = CheckOrientation(lookingRight);

	switch (status) {
	case enemy_state::IDLE:
		animPtr = &idleSprite.anim;
		break;
	case enemy_state::PATROLING:
		animPtr = &idleSprite.anim;
		break;
	case enemy_state::FOLLOWING:
		animPtr = &followSprite.anim;
		break;
	case enemy_state::ATTACKING:
		break;
	case enemy_state::FALLING:
		break;
	case enemy_state::HURT:
		if (deadTimer < SDL_GetTicks() - deathDelay) {
			mustDestroy = true;
		}

		animPtr = &deadSprite.anim;

		input.wantMoveUp = false;
		input.wantMoveDown = false;
		input.wantMoveRight = false;
		input.wantMoveLeft = false;
		break;
	}
}

void Bat::Move(float dt)
{
	if (input.wantMoveRight == true && input.wantMoveLeft == false) {
		speed.x += acceleration.x * dt;
	}
	else if (input.wantMoveLeft == true && input.wantMoveRight == false) {
		speed.x -= acceleration.x * dt;
	}
	else {	// Natural deacceleration
		if (movement.movingRight == true) {
			speed.x -= acceleration.x * dt;

			if (speed.x < 0.0f)
				speed.x = 0.0f;
		}
		else if (movement.movingLeft == true) {
			speed.x += acceleration.x * dt;

			if (speed.x > 0.0f)
				speed.x = 0.0f;
		}
	}

	if (input.wantMoveUp == true && input.wantMoveDown == false) {
		speed.y += acceleration.y * dt;
	}
	else if (input.wantMoveDown == true && input.wantMoveUp == false) {
		speed.y -= acceleration.y * dt;
	}
	else {	// Natural deacceleration
		if (movement.movingDown == true) {
			speed.y -= acceleration.y * dt;

			if (speed.y < 0.0f)
				speed.y = 0.0f;
		}
		else if (movement.movingUp == true) {
			speed.y += acceleration.y * dt;

			if (speed.y > 0.0f)
				speed.y = 0.0f;
		}
	}

	// Max Speeds
	LimitSpeed();

	// New position
	position.x += speed.x * dt;
	position.y += speed.y * dt;

	posRect.x = (int)position.x;
	posRect.y = (int)position.y;
}

void Bat::UpdateHitbox()
{
	switch (status) {
	case enemy_state::IDLE:
		hitboxOffset = ReshapeCollider(idleSprite);
		break;
	case enemy_state::PATROLING:
		hitboxOffset = ReshapeCollider(idleSprite);
		break;
	case enemy_state::FOLLOWING:
		hitboxOffset = ReshapeCollider(followSprite);
		break;
	case enemy_state::ATTACKING:
		break;
	case enemy_state::FALLING:
		break;
	case enemy_state::HURT:
		hitboxOffset = ReshapeCollider(deadSprite);
		break;
	default:
		break;
	}
}