#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "p2Animation.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Scene2.h"

#include "j1EntityManager.h"
#include "Player.h"
#include "Enemy.h"

// Called before render is available
bool Enemy::Awake(pugi::xml_node& entities)
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

// Called each loop iteration
bool Enemy::PreUpdate()
{
	bool ret = true;

	BROFILER_CATEGORY("Entity Enemy PreUpdate", Profiler::Color::IndianRed);

	CheckInput();		// Check player input
	CheckMovement();	// Check player current movement

	return ret;
}

// Called between a certain number of frames or times per second
bool Enemy::UpdateLogic(float dt)
{
	BROFILER_CATEGORY("Entity Enemy Update", Profiler::Color::PaleVioletRed);

	//TODO: Pathfinding

	return true;
}

// Called each frame (framerate dependant)
bool Enemy::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Entity Enemy UpdateTick", Profiler::Color::Red);

	bool ret = true;

	CheckState();	// Check player state
	ApplyState();	// Add state effects like movement restrictions, animation and sounds
	Move(dt);		// Move player position and calculate other movement related factors
	UpdateHitbox();	// Transform player collider depending on new position and state

	animRect = animPtr->GetCurrentFrame(dt);

	return ret;
}

// Called each loop iteration
bool Enemy::Update()
{
	BROFILER_CATEGORY("Entity Enemy Update", Profiler::Color::DarkRed);

	bool ret = true;

	Draw(&animRect);

	return ret;
}

bool Enemy::CleanUp()
{
	bool ret = true;

	LOG("Unloading enemy");

	App->tex->UnLoad(graphics);
	graphics = nullptr;

	hitbox->to_delete = true;	// @Carles, mark collider for destruction
	hitbox = nullptr;

	return ret;
}

// Save and Load
bool Enemy::Load(pugi::xml_node &entities)
{
	//TODO: Load enemy data of current alive enemies (enemyType, state, position, speed)
	return true;
}

bool Enemy::Save(pugi::xml_node &entities) const
{
	//TODO: Save enemy data of current alive enemies (enemyType, state, position, speed)
	return true;
}

// Called when colliding
collision_type Enemy::OnCollision(Collider* c1, Collider* c2)
{
	collision_type ret = collision_type::NONE;

	if (c1->GetType() == collider_type::COLLIDER_ENEMY && c2->GetType() == collider_type::COLLIDER_WALL) {
		ret = WallCollision(c1, c2);
	}

	if (App->entityManager->player->IsGod() == false) {
		if (c1->GetType() == collider_type::COLLIDER_ENEMY && c2->GetType() == collider_type::COLLIDER_PLAYER_ATTACK) {
			//ret = PlayerAttackCollision(c1, c2);
			Hurt();
			if (IsDead()) {
				deadTimer = SDL_GetTicks();
				dead = true;
			}
		}
	}

	return ret;
}

// Called when colliding
collision_type Enemy::WallCollision(Collider* c1, Collider* c2)
{
	collision_type ret = collision_type::UNDEFINED;

	SDL_Rect collisionOverlay;
	SDL_IntersectRect(&c1->rect, &c2->rect, &collisionOverlay);

	if (collisionOverlay.w >= collisionOverlay.h) {
		if (c1->rect.y + c1->rect.h > c2->rect.y && c1->rect.y < c2->rect.y && movement.movingDown == true) {	//Ground
			while (c1->CheckCollision(c2->rect) == true) {
				c1->rect.y--;
			}
			c1->rect.y++;
			speed.y = 0.0f;
			airborne = false;
			ret = collision_type::ON_BOTTOM;
		}
		else if (c1->rect.y < c2->rect.y + c2->rect.h && c1->rect.y + c1->rect.h > c2->rect.y + c2->rect.h && movement.movingUp == true) {	//Ceiling
			while (c1->CheckCollision(c2->rect) == true) {
				c1->rect.y++;
			}
			c1->rect.y--;		//IMPROVE: On high framerates, doesn't go down immediately after colliding, which it should
			speed.y = 0.0f;
			ret = collision_type::ON_TOP;
		}

		position.y = (float)(c1->rect.y - hitboxOffset.y);
	}
	else {
		if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x < c2->rect.x && movement.movingRight == true) {	//Right
			while (c1->CheckCollision(c2->rect) == true) {
				c1->rect.x--;
			}
			c1->rect.x++;
			speed.x = 0.0f;
			ret = collision_type::ON_RIGHT;
		}
		else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w > c2->rect.x + c2->rect.w && movement.movingLeft == true) {	//Left	//IMPROVE: movement.movingLeft makes the image lagg
			while (c1->CheckCollision(c2->rect) == true) {
				c1->rect.x++;
			}
			//c1->rect.x--;	//IMPROVE: Really wierd bug: uncommenting this and therefore readjusting the hitbox after a collision so it's jsut touching the wall
			//makes the input GetKey() scan the linked right movement key ALWAYS as IDLE until not longer in contact with the wall

			speed.x = 0.0f;
			ret = collision_type::ON_LEFT;
		}

		position.x = (float)(c1->rect.x - hitboxOffset.x);
	}

	return ret;
}

// Import all state data from config.xml
void Enemy::ImportAllStates(pugi::xml_node &config)
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

	// Character status flags and directly related data
	airborne = config.child("airborne").attribute("value").as_bool();
	lookingRight = config.child("looking").attribute("right").as_bool();
	deathDelay = config.child("deathDelay").attribute("miliseconds").as_uint();
}

void Enemy::CheckInput()	//IMPROVE: RADIUS SHOULD BE BASED ON ENEMY CENTER, right now is top-left corner
{
	//TODO: This should work with pathfinding
	
	fPoint playerPos = App->entityManager->player->GetPosition();
	playerInRadius = InsideRadius(playerPos);
	
	if (playerInRadius) {
		if (playerPos.x > position.x) {		// Move left input
			input.wantMoveRight = true;
		}
		else {
			input.wantMoveRight = false;
		}

		if (playerPos.x < position.x) {		// Move right input
			input.wantMoveLeft = true;
		}
		else {
			input.wantMoveLeft = false;
		}

		if (playerPos.y > position.y) {		// Move down input
			input.wantMoveUp = true;
		}
		else {
			input.wantMoveUp = false;
		}

		if (playerPos.y < position.y) {		// Move up input
			input.wantMoveDown = true;
		}
		else {
			input.wantMoveDown = false;
		}
	}
	else {
		input.wantMoveLeft = false;
		input.wantMoveRight = false;
		input.wantMoveDown = false;
		input.wantMoveUp = false;
	}
}

bool Enemy::InsideRadius(fPoint playerPos)
{
	return !(playerPos.x > position.x + detectionRadius.x || playerPos.x < position.x - detectionRadius.x
		|| playerPos.y > position.y + detectionRadius.y || playerPos.y < position.y - detectionRadius.y);
}