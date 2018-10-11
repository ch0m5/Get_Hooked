#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"			// CHANGE/FIX?
#include "j1Input.h"		// Do we need it?
#include "j1Textures.h"		// Do we need it?
#include "j1Render.h"
#include "j1Audio.h"		// Do we need it?
#include "j1Player.h"
#include "j1Collision.h"
#include "p2Animation.h"	// @Carles

j1Player::j1Player()
{
	name.create("player");
}

j1Player::~j1Player()
{}

// Called before render is available
bool j1Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player Data");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	characterSheet.create("%s%s", folder.GetString(), config.child("sprites").child("spriteSheet").child_value());
	
	spriteSize = { config.child("sprites").child("spriteSize").attribute("x").as_int(), config.child("sprites").child("spriteSize").attribute("y").as_int() };
	defaultAnimSpeed = config.child("sprites").child("animation").attribute("default_speed").as_float();

	// CHANGE/FIX: Load SFX sounds

	// Character stats
	life = config.child("life").attribute("value").as_uint();

	speed = { config.child("speed").attribute("x").as_float(), config.child("speed").attribute("y").as_float() };
	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };

	acceleration = config.child("acceleration").attribute("x").as_float();
	jumpVelocity = config.child("jump").attribute("forceY").as_float();
	gravity = config.child("gravity").attribute("accelerationY").as_float();

	// Character status flags
	dead = config.child("dead").attribute("value").as_bool();	//CHANGE/FIX?: life == 0 serves the same purpose unless we want something in the middle to happen
	godmode = config.child("godmode").attribute("value").as_bool();

	//Collider
	//Collider* playerHitbox = nullptr;

	// Animations
	AllocAllAnimations();	// Makes pushbacks of all player animations

	// CHANGE/FIX: Hardcoded start
	position = { 100, 550 };

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;

	state = player_state::IDLE;

	graphics = App->tex->Load(characterSheet.GetString());
	//hitbox = App->collision->AddCollider({ (int)position.x, (int)position.y, shipWidth, shipHeight }, COLLIDER_PLAYER, this);		//CHANGE/FIX: Collider must change with animation

	//CHANGE/FIX?: Add condition to load save file on startup?

	return ret;
}

// Called each loop iteration
bool j1Player::PreUpdate()	// CHANGE/FIX: Player input here?
{
	bool ret = true;

	return ret;
}
bool j1Player::Update(float dt)
{
	bool ret = true;

	PlayerInput();

	animRect.x = (int)position.x;
	animRect.y = (int)position.y;

	SDL_Rect playerRect = animPtr->GetCurrentFrame();
	if (lookingRight == true) {
		App->render->Blit(graphics, position.x, position.y, &playerRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(graphics, position.x, position.y, &playerRect, SDL_FLIP_HORIZONTAL);
	}

	return ret;
}

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;
	
	LOG("Unloading player");

	App->tex->UnLoad(graphics);
	graphics = nullptr;

	return ret;
}

// Called when colliding
void j1Player::OnCollision(Collider* c1, Collider* c2)
{

}

// Load Game State
bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_float();
	position.y = data.child("position").attribute("y").as_float();

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const	// CHANGE/FIX: Add all data that needs to be saved (speed, status, etc)
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

//------------------------------------------------

void j1Player::PlayerInput()
{
	// System: Each player state has a limited set of actions available, so we check the possible actions on each state
	switch (state) {
	case player_state::IDLE:
		animPtr = &idle;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			state = player_state::RUNNING;
		}
		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::ON_AIR;
			speed.y -= jumpVelocity;
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			state = player_state::CROUCHING;
		}
		break;
	case player_state::CROUCHING:
		animPtr = &crouch;

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) {
			state = player_state::IDLE;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::ON_AIR;
			speed.y -= jumpVelocity;
		}
		break;
	case player_state::RUNNING:
		animPtr = &run;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::ON_AIR;
			speed.y -= jumpVelocity;
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			state = player_state::SLIDING;
		}
		else {
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {
				speed.x -= acceleration;
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
				speed.x += acceleration;
			}
			else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE || App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE
				||
				App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				if (speed.x == 0.0f) {
					state = player_state::IDLE;	// CHANGE/FIX: Bugs out
				}
			}
		}
		break;
	case player_state::ON_AIR:

		speed.y += gravity;

		if (speed.y > 0.0f) {
			animPtr = &fall;
		}
		else if (somersaultUsed == false) {
			animPtr = &jump;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
			speed.y = -jumpVelocity;
			animPtr = &somersault;
			somersaultUsed = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {
			speed.x -= acceleration;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
			speed.x += acceleration;
		}

		if (position.y > 550) {		//CHANGE/FIX: Hardcoded values, this is where collision condition goes
			position.y = 550;
			
			speed.y = 0.0f;
			jump.Reset();
			somersault.Reset();
			somersaultUsed = false;

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT
				||
				App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
					state = player_state::SLIDING;
				}
				else {
					state = player_state::RUNNING;
				}
			}
			else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
				state = player_state::CROUCHING;
			}
			else {
				state = player_state::IDLE;
			}
		}
		break;
	case player_state::SLIDING:
		animPtr = &slide;
		
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::ON_AIR;
			animPtr = &jump;
			speed.y -= jumpVelocity;
			slide.Reset();
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) {

			slide.Reset();

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT
				||
				App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
				state = player_state::RUNNING;
				animPtr = &run;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {		
			if (speed.x < 0.0f) {
				speed.x += acceleration / 2.5;
			}
			else {
				speed.x = 0.0f;
				slide.Reset();
				state = player_state::CROUCHING;
				animPtr = &crouch;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
			if (speed.x > 0.0f) {
				speed.x -= acceleration / 2.5;
			}
			else {
				speed.x = 0.0f;
				slide.Reset();
				state = player_state::CROUCHING;
				animPtr = &crouch;
			}
		}
			
		break;
	case player_state::HOOK:
		//animPtr = &run;
		break;
	case player_state::HURT:
		//animPtr = &run;
		break;
	case player_state::DEAD:
		//animPtr = &run;
		break;
	}

	// Natural deacceleration
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE
		||
		App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (speed.x > 0.0f) {
			speed.x -= acceleration;

			if (speed.x < 0.0f)
				speed.x = 0.0f;
		}
		else if (speed.x < 0.0f) {
			speed.x += acceleration;

			if (speed.x > 0.0f)
				speed.x = 0.0f;
		}
	}

	// Max Speeds
	if (speed.x > maxSpeed.x)
		speed.x = maxSpeed.x;
	else if (speed.x < -maxSpeed.x)
		speed.x = -maxSpeed.x;

	if (speed.y > maxSpeed.y)
		speed.y = maxSpeed.y;
	else if (speed.y < -maxSpeed.y)
		speed.y = -maxSpeed.y;

	// Player direction
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT && state != player_state::SLIDING)
		lookingRight = false;
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT && state != player_state::SLIDING)
		lookingRight = true;

	// If player goes down he's falling and is not on the hook, he's falling
	if (speed.y > 0.0f && state != player_state::HOOK) {
		state = player_state::ON_AIR;
	}

	// New position
	position.x += speed.x;
	position.y += speed.y;
}

// @Carles: Allocates all animations using the AllocAnimation function and parameters related to their sprite sheet location
void j1Player::AllocAllAnimations() {

	idle.AllocAnimation({ 0, spriteSize.y * 0 }, spriteSize, defaultAnimSpeed, 4, true);
	run.AllocAnimation({ 0, spriteSize.y * 4 }, spriteSize, defaultAnimSpeed, 6, true);
	slide.AllocAnimation({ 0, spriteSize.y * 5 }, spriteSize, defaultAnimSpeed, 5, false);
	crouch.AllocAnimation({ 0, spriteSize.y * 6 }, spriteSize, defaultAnimSpeed, 4, true);
	jump.AllocAnimation({ 0, spriteSize.y * 7 }, spriteSize, defaultAnimSpeed * 2, 4, false);
	somersault.AllocAnimation({ 0, spriteSize.y * 8 }, spriteSize, defaultAnimSpeed, 4, true);
	fall.AllocAnimation({ spriteSize.x * 4, spriteSize.y * 7 }, spriteSize, defaultAnimSpeed, 2, true);
	hurt.AllocAnimation({ 0, spriteSize.y * 22 }, spriteSize, defaultAnimSpeed, 3, false);
	die.AllocAnimation({ 0, spriteSize.y * 0 }, spriteSize, defaultAnimSpeed, 7, false);
}