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
#include "Slime.h"

Slime::Slime() : Enemy(enemy_type::SLIME)
{
	name.create("slime");
}

Slime::~Slime()
{}

// Called before the first frame
bool Slime::Start()
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

// Import all state data from config.xml
void Slime::ImportAllStates(pugi::xml_node &config)
{
	// Character stats
	maxLife = (ushort)config.child("life").attribute("value").as_uint();
	speed = { config.child("speed").attribute("x").as_float(), config.child("speed").attribute("y").as_float() };
	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };
	acceleration.x = config.child("accelerations").attribute("x").as_float();
	acceleration.y = config.child("accelerations").attribute("y").as_float();
	gravity = config.child("accelerations").attribute("gravity").as_float();
	canFly = config.child("canFly").attribute("value").as_bool();

	detectionRadius.x = config.child("detection").attribute("x").as_float();
	detectionRadius.y = config.child("detection").attribute("y").as_float();
	attackRange.x = config.child("attack").attribute("x").as_float();
	attackRange.y = config.child("attack").attribute("y").as_float();

	// Character status flags and directly related data
	airborne = config.child("airborne").attribute("value").as_bool();
	lookingRight = config.child("looking").attribute("right").as_bool();
	attackDelay = config.child("attackDelay").attribute("miliseconds").as_uint();
	hurtDelay = config.child("hurtDelay").attribute("miliseconds").as_uint();
	deathDelay = config.child("deathDelay").attribute("miliseconds").as_uint();

	//Slime
	dashMultiplier = config.child("dash").attribute("multiplier").as_uint();
}

// Import all sprite data using the above function for each animation
void Slime::ImportAllSprites(pugi::xml_node& first_sprite)
{
	ImportSpriteData("idle", &idleSprite, first_sprite);
	ImportSpriteData("move", &moveSprite, first_sprite);
	ImportSpriteData("attack", &attackSprite, first_sprite);
	ImportSpriteData("hurt", &hurtSprite, first_sprite);
	ImportSpriteData("dead", &deadSprite, first_sprite);
}

// Allocate all animations with previously recieved sprite data
void Slime::AllocAllAnimations()
{
	idleSprite.anim.AllocAnimation({ idleSprite.sheetPosition.x * spriteSize.x, idleSprite.sheetPosition.y * spriteSize.y }, spriteSize, idleSprite.numFrames);
	moveSprite.anim.AllocAnimation({ moveSprite.sheetPosition.x * spriteSize.x, moveSprite.sheetPosition.y * spriteSize.y }, spriteSize, moveSprite.numFrames);
	attackSprite.anim.AllocAnimation({ attackSprite.sheetPosition.x * spriteSize.x, attackSprite.sheetPosition.y * spriteSize.y }, spriteSize, attackSprite.numFrames);
	hurtSprite.anim.AllocAnimation({ hurtSprite.sheetPosition.x * spriteSize.x, hurtSprite.sheetPosition.y * spriteSize.y }, spriteSize, hurtSprite.numFrames);
	deadSprite.anim.AllocAnimation({ deadSprite.sheetPosition.x * spriteSize.x, deadSprite.sheetPosition.y * spriteSize.y }, spriteSize, deadSprite.numFrames);
}

void Slime::DashAttack()
{
	maxSpeed.x *= dashMultiplier;
	
	if (lookingRight)
		speed.x = maxSpeed.x;
	else
		speed.x = -maxSpeed.x;
}

void Slime::CheckState()
{
	if (!canFly && App->collision->CheckGroundCollision(hitbox) == false)
		airborne = true;

	if (!dead) {
		if (airborne) {
			status = enemy_state::FALLING;
		}

		switch (status) {
		case enemy_state::IDLE:
			if (airborne)
				status = enemy_state::FALLING;
			else if (playerDetected)
				status = enemy_state::FOLLOWING;
			break;
		case enemy_state::PATROLING:
			if (airborne)
				status = enemy_state::FALLING;
			else if (playerDetected)
				status = enemy_state::FOLLOWING;
			break;
		case enemy_state::FOLLOWING:
			if (airborne)
				status = enemy_state::FALLING;
			else if (!playerDetected)
				status = enemy_state::IDLE;
			else if (wantToAttack) {
				status = enemy_state::ATTACKING;
				attackTimer = SDL_GetTicks();
				DashAttack();
			}
			break;
		case enemy_state::ATTACKING:
			if (attackTimer < SDL_GetTicks() - attackDelay) {
				attackSprite.anim.Reset();
				maxSpeed.x /= dashMultiplier;
				if (playerDetected)
					status = enemy_state::FOLLOWING;
				else
					status = enemy_state::IDLE;
				LOG("Exiting Attack");
			}
			break;
		case enemy_state::FALLING:
			if (!airborne) {
				if (playerDetected)
					status = enemy_state::FOLLOWING;
				else
					status = enemy_state::IDLE;
			}
			break;
		case enemy_state::HURT:
			if (hurtTimer > SDL_GetTicks() - hurtDelay) {
				hurtSprite.anim.Reset();
				if (playerDetected)
					status = enemy_state::FOLLOWING;
				else
					status = enemy_state::IDLE;
			}
			break;
		}
	}
}

void Slime::ApplyState()
{
	if (status != enemy_state::ATTACKING)
		lookingRight = CheckOrientation(lookingRight);

	switch (status) {
	case enemy_state::IDLE:
		animPtr = &idleSprite.anim;
		break;
	case enemy_state::PATROLING:
		animPtr = &moveSprite.anim;
		break;
	case enemy_state::FOLLOWING:
		animPtr = &moveSprite.anim;
		break;
	case enemy_state::ATTACKING:
		animPtr = &attackSprite.anim;

		input.wantMoveUp = false;
		input.wantMoveDown = false;
		input.wantMoveRight = false;
		input.wantMoveLeft = false;
		break;
	case enemy_state::FALLING:
		animPtr = &idleSprite.anim;
		break;
	case enemy_state::HURT:
		if (dead) {
			animPtr = &deadSprite.anim;

			if (deadTimer < SDL_GetTicks() - deathDelay)
				mustDestroy = true;
		}
		else {
			animPtr = &hurtSprite.anim;
		}

		input.wantMoveUp = false;
		input.wantMoveDown = false;
		input.wantMoveRight = false;
		input.wantMoveLeft = false;
		break;
	}
}

void Slime::Move(float dt)
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

	// If on air, apply gravity
	if (airborne) {
		Fall(dt);
	}

	// Max Speeds
	LimitSpeed();

	// New position
	position.x += speed.x * dt;
	position.y += speed.y * dt;

	// Center position
	centerPosition.x = position.x + animRect.w / 2;
	centerPosition.y = position.y + animRect.h / 2;
}

void Slime::UpdateHitbox()
{
	switch (status) {
	case enemy_state::IDLE:
		hitboxOffset = ReshapeCollider(idleSprite);
		break;
	case enemy_state::PATROLING:
		hitboxOffset = ReshapeCollider(moveSprite);
		break;
	case enemy_state::FOLLOWING:
		hitboxOffset = ReshapeCollider(moveSprite);
		break;
	case enemy_state::ATTACKING:
		hitboxOffset = ReshapeCollider(attackSprite);
		break;
	case enemy_state::FALLING:
		hitboxOffset = ReshapeCollider(idleSprite);
		break;
	case enemy_state::HURT:
		if (dead)
			hitboxOffset = ReshapeCollider(deadSprite);
		else
			hitboxOffset = ReshapeCollider(hurtSprite);
		break;
	default:
		break;
	}
}