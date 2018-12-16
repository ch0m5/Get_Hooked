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

#include "j1EntityManager.h"
#include "Player.h"
#include "Bat.h"

Bat::Bat() : Enemy(entity_type::BAT)
{
	name.create("bat");
}

// Called before the first frame
bool Bat::Start()
{
	dead = false;
	life = maxLife;
	speed = { 0, 0 };
	hitboxOffset = idleSprite.colliderOffset;
	status = enemy_state::IDLE;

	graphics = App->tex->Load(textureName.GetString());

	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_ENEMY, this);
	hitboxOffset = hitbox->rect;

	return true;
}

// Import all sprite data using the above function for each animation
void Bat::ImportAllSprites(pugi::xml_node& first_sprite)
{
	ImportSpriteData("follow", &followSprite, first_sprite);
	ImportSpriteData("idle", &idleSprite, first_sprite);
	ImportSpriteData("falling", &fallingSprite, first_sprite);
	ImportSpriteData("dead", &deadSprite, first_sprite);	//IMPROVE: Single frame animation
}

// Allocate all animations with previously recieved sprite data
void Bat::AllocAllAnimations()
{
	followSprite.anim.AllocAnimation({ followSprite.sheetPosition.x * spriteSize.x, followSprite.sheetPosition.y * spriteSize.y }, spriteSize, followSprite.numFrames);
	idleSprite.anim.AllocAnimation({ idleSprite.sheetPosition.x * spriteSize.x, idleSprite.sheetPosition.y * spriteSize.y }, spriteSize, idleSprite.numFrames);
	fallingSprite.anim.AllocAnimation({ fallingSprite.sheetPosition.x * spriteSize.x, fallingSprite.sheetPosition.y * spriteSize.y }, spriteSize, fallingSprite.numFrames);
	deadSprite.anim.AllocAnimation({ deadSprite.sheetPosition.x * spriteSize.x, deadSprite.sheetPosition.y * spriteSize.y }, spriteSize, deadSprite.numFrames);
}

void Bat::CheckState()
{
	if (!dead && App->collision->CheckGroundCollision(hitbox) == false)
		airborne = true;

	if (!dead) {
		switch (status) {
		case enemy_state::IDLE:
			if(playerDetected)
				status = enemy_state::FOLLOWING;
			break;
		case enemy_state::PATROLING:
			if (playerDetected)
				status = enemy_state::FOLLOWING;
			break;
		case enemy_state::FOLLOWING:
			if (!playerDetected)
				status = enemy_state::IDLE;
			break;
		case enemy_state::ATTACKING:
			break;
		case enemy_state::FALLING:
			break;
		case enemy_state::HURT:
			if (hurtTimer < SDL_GetTicks() - hurtDelay) {
				if (playerDetected)
					status = enemy_state::FOLLOWING;
				else
					status = enemy_state::IDLE;
			}
			break;
		}
	}
	
	if (position.y > 800) {	//CHANGE/FIX: Hardcoded pit
		active = false;
	}
}

void Bat::ApplyState()
{
	if (!(status == enemy_state::HURT))
		CheckOrientation();

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
		if (dead) {
			if (airborne) {
				animPtr = &fallingSprite.anim;
			}
			else {
				if (deadTimer < SDL_GetTicks() - deathDelay) {
					active = false;
					turnedOn = false;
					App->entityManager->player->AddScore(1);

					if (hitbox != nullptr) {
						hitbox->to_delete = true;
						hitbox = nullptr;
					}
				}

				animPtr = &deadSprite.anim;
			}
		}
		else {
			animPtr = &idleSprite.anim;
		}

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

	if (dead && airborne) {
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