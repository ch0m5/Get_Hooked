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

	// Sprites
	characterSheet.create("%s%s", folder.GetString(), config.child("sprites").child("spriteSheet").child_value());
	spriteSize = { config.child("sprites").child("spriteSize").attribute("x").as_int(), config.child("sprites").child("spriteSize").attribute("y").as_int() };
	defaultAnimSpeed = config.child("sprites").child("animation").attribute("default_speed").as_float();

	pugi::xml_node first_sprite = config.child("sprites").child("first_sprite");
	ImportAllSprites(first_sprite);	// Imports all core sprite data for later animation allocation

	// Animations
	AllocAllAnimations();	// Makes pushbacks of all player animations

	// CHANGE/FIX: Load SFX sounds

	// Character stats
	life = config.child("life").attribute("value").as_uint();
	speed = { config.child("speed").attribute("x").as_float(), config.child("speed").attribute("y").as_float() };
	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };
	normalAcceleration = config.child("accelerations").attribute("x").as_float();
	slideAcceleration = config.child("accelerations").attribute("slide").as_float();
	jumpVelocity = config.child("jump").attribute("forceY").as_float();
	gravity = config.child("accelerations").attribute("gravity").as_float();

	// Character status flags
	lookingRight = config.child("lookingRight").attribute("value").as_bool();
	godmode = config.child("godmode").attribute("value").as_bool();
	//dead

	//Collider
	//Collider* playerHitbox = nullptr;

	// CHANGE/FIX: Hardcoded start
	position = { 100, 550 };

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;
	
	currentAcceleration = normalAcceleration;
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

	/*if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {	//CHANGE/FIX: Give a use to this
		if (--life > 0) {
			state = player_state::HURT;
		}
		else {
			state = player_state::DEAD;
		}
	}*/

	PlayerInput();		// Check player input
	PlayerMovement();	// Check player current movement
	PlayerState();		// Check player state
	PlayerAnimation();	// Pick animation based on state
	MovePlayer();		// Apply changes to player

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

// Imports from the xml file all data of the first sprite of each animation and other important data like animation speed, frames and if it loops
void j1Player::ImportSpriteData(const char* spriteName, player_sprite* sprite, pugi::xml_node& first_sprite)
{
	sprite->position.x = first_sprite.child(spriteName).attribute("column").as_int();
	sprite->position.y = first_sprite.child(spriteName).attribute("row").as_int();
	sprite->frames = first_sprite.child(spriteName).attribute("frames").as_uint();
	sprite->animSpeed = first_sprite.child(spriteName).attribute("animSpeed").as_float();
	sprite->loop = first_sprite.child(spriteName).attribute("loop").as_bool();
}

void j1Player::ImportAllSprites(pugi::xml_node& first_sprite)
{
	ImportSpriteData(first_sprite.child("idle").child_value(), &idleSprite, first_sprite);
	ImportSpriteData(first_sprite.child("run").child_value(), &runSprite, first_sprite);
	ImportSpriteData(first_sprite.child("slide").child_value(), &slideSprite, first_sprite);
	ImportSpriteData(first_sprite.child("crouch").child_value(), &crouchSprite, first_sprite);
	ImportSpriteData(first_sprite.child("jump").child_value(), &jumpSprite, first_sprite);
	ImportSpriteData(first_sprite.child("somersault").child_value(), &somersaultSprite, first_sprite);
	ImportSpriteData(first_sprite.child("fall").child_value(), &fallSprite, first_sprite);
	ImportSpriteData(first_sprite.child("hurt").child_value(), &hurtSprite, first_sprite);
	ImportSpriteData(first_sprite.child("dead").child_value(), &deadSprite, first_sprite);
}

// Allocates all animations using the AllocAnimation function and parameters related to their sprite sheet location extracted from the config.xml file
void j1Player::AllocAllAnimations()
{
	idleAnim.AllocAnimation({ idleSprite.position.x * spriteSize.x, idleSprite.position.y * spriteSize.y }, spriteSize, idleSprite.animSpeed, idleSprite.frames, idleSprite.loop);
	runAnim.AllocAnimation({ runSprite.position.x * spriteSize.x, runSprite.position.y * spriteSize.y }, spriteSize, runSprite.animSpeed, runSprite.frames, runSprite.loop);
	slideAnim.AllocAnimation({ slideSprite.position.x * spriteSize.x, slideSprite.position.y * spriteSize.y }, spriteSize, slideSprite.animSpeed, slideSprite.frames, slideSprite.loop);
	crouchAnim.AllocAnimation({ crouchSprite.position.x * spriteSize.x, crouchSprite.position.y * spriteSize.y }, spriteSize, crouchSprite.animSpeed, crouchSprite.frames, crouchSprite.loop);
	jumpAnim.AllocAnimation({ jumpSprite.position.x * spriteSize.x, jumpSprite.position.y * spriteSize.y }, spriteSize, jumpSprite.animSpeed, jumpSprite.frames, jumpSprite.loop);
	somersaultAnim.AllocAnimation({ somersaultSprite.position.x * spriteSize.x, somersaultSprite.position.y * spriteSize.y }, spriteSize, somersaultSprite.animSpeed, somersaultSprite.frames, idleSprite.loop);
	fallAnim.AllocAnimation({ fallSprite.position.x * spriteSize.x, fallSprite.position.y * spriteSize.y }, spriteSize, fallSprite.animSpeed, fallSprite.frames, fallSprite.loop);
	hurtAnim.AllocAnimation({ hurtSprite.position.x * spriteSize.x, hurtSprite.position.y * spriteSize.y }, spriteSize, hurtSprite.animSpeed, hurtSprite.frames, hurtSprite.loop);
	deadAnim.AllocAnimation({ deadSprite.position.x * spriteSize.x, deadSprite.position.y * spriteSize.y }, spriteSize, deadSprite.animSpeed, deadSprite.frames, deadSprite.loop);
}

//------------------------------------------------

// Player actions' speed effects
void j1Player::Jump()
{
	speed.y = -jumpVelocity;
}

void j1Player::Fall()
{
	speed.y += gravity;
}

void j1Player::Land()
{
	speed.y = 0.0f;
}

//Check player input
void j1Player::PlayerInput()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {
		wantMoveLeft = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		wantMoveLeft = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {
		wantMoveRight = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		wantMoveRight = true;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE) {
		wantMoveUp = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		wantMoveUp = true;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE) {
		wantMoveDown = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		wantMoveDown = true;
	}
}

void j1Player::PlayerMovement() {
	if (speed.x > 0.0f) {
		movingRight = true;
		movingLeft = false;
	}
	else if (speed.x < 0.0f) {
		movingLeft = true;
		movingRight = false;
	}
	else if (speed.x == 0.0f) {
		movingLeft = false;
		movingRight = false;
	}

	if (speed.y < 0.0f) {
		movingUp = true;
		movingDown = false;
	}
	else if (speed.y > 0.0f) {
		movingDown = true;
		movingUp = false;
	}
	else if (speed.y == 0.0f) {
		movingUp = false;
		movingDown = false;
	}
}

//Decide new player state
void j1Player::PlayerState() {
	switch (state) {
	case player_state::IDLE:
		idleMoveCheck();
		break;
	case player_state::CROUCHING:
		crouchingMoveCheck();
		break;
	case player_state::RUNNING:
		runningMoveCheck();
		break;
	case player_state::AIRBORNE:
		airMoveCheck();
		break;
	case player_state::SLIDING:
		slidingMoveCheck();
		break;
	case player_state::HURT:
		
		break;
	case player_state::DEAD:
		
		break;
	}
}

void j1Player::idleMoveCheck()
{
	if (wantMoveRight == true && wantMoveLeft == false || wantMoveLeft == true && wantMoveRight == false) {
		state = player_state::RUNNING;
	}
	else if (wantMoveUp == true) {
		Jump();
		state = player_state::AIRBORNE;
	}
	else if (wantMoveDown == true) {
		state = player_state::CROUCHING;
	}
}

void j1Player::crouchingMoveCheck()
{
	if (wantMoveDown == false) {
		if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true)
			state = player_state::RUNNING;
		else {
			state = player_state::IDLE;
		}
	}
	else if (wantMoveUp == true) {
		Jump();
		state = player_state::AIRBORNE;
	}
}

void j1Player::runningMoveCheck()
{
	if (wantMoveUp == true) {
		Jump();
		state = player_state::AIRBORNE;
	}
	else if (wantMoveDown == true) {
		state = player_state::SLIDING;
	}
	else if (movingLeft == false && movingRight == false) {
		if (wantMoveRight == false && wantMoveLeft == false || wantMoveRight == true && wantMoveLeft == true) {
			state = player_state::IDLE;
		}
	}
}

void j1Player::airMoveCheck()
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		somersaultAnim.Reset();
		Jump();
		somersaultUsed = true;
	}

	if (position.y > 550) {		//CHANGE/FIX: Hardcoded values, this condition should be "if feet collision"
		position.y = 550;

		Land();
		jumpAnim.Reset();
		somersaultUsed = false;

		if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true) {
			if (wantMoveDown == true) {
				state = player_state::SLIDING;
			}
			else {
				state = player_state::RUNNING;
			}
		}
		else if (wantMoveDown == true) {
			state = player_state::CROUCHING;
		}
		else {
			state = player_state::IDLE;
		}
	}
}

void j1Player::slidingMoveCheck()
{
	if (wantMoveUp == true) {
		Jump();
		currentAcceleration = normalAcceleration;
		slideAnim.Reset();
		state = player_state::AIRBORNE;
	}
	else if (wantMoveDown == false) {

		currentAcceleration = normalAcceleration;
		slideAnim.Reset();

		if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true) {
			state = player_state::RUNNING;
		}
		else {
			state = player_state::IDLE;
		}
	}
	else if (movingLeft == false && movingRight == false) {

		currentAcceleration = normalAcceleration;
		slideAnim.Reset();

		if (wantMoveDown == true) {
			state = player_state::CROUCHING;
		}
		else {
			state = player_state::IDLE;
		}
	}
}

// Pick animation based on state
void j1Player::PlayerAnimation()
{
	if (state != player_state::SLIDING) {
		if (wantMoveRight == true && wantMoveLeft == false) {
			lookingRight = true;
		}
		else if (wantMoveLeft == true && wantMoveRight == false) {
			lookingRight = false;
		}
	}

	switch (state) {
	case player_state::IDLE:
		animPtr = &idleAnim;
		break;
	case player_state::CROUCHING:
		wantMoveRight = false;
		wantMoveLeft = false;
		animPtr = &crouchAnim;
		break;
	case player_state::RUNNING:
		animPtr = &runAnim;
		break;
	case player_state::AIRBORNE:
		if (movingDown == true) {
			animPtr = &fallAnim;
		}
		else if (somersaultUsed == true) {
			animPtr = &somersaultAnim;
		}
		else {
			animPtr = &jumpAnim;
		}
		break;
	case player_state::SLIDING:
		currentAcceleration = slideAcceleration;
		animPtr = &slideAnim;
		break;
	}
}

// Move player
void j1Player::MovePlayer()
{
	if (state != player_state::SLIDING && wantMoveRight == true && wantMoveLeft == false) {
		speed.x += currentAcceleration;
	}
	else if (state != player_state::SLIDING && wantMoveLeft == true && wantMoveRight == false) {
		speed.x -= currentAcceleration;
	}
	else {	// Natural deacceleration
		if (movingRight == true) {
			speed.x -= currentAcceleration;

			if (speed.x < 0.0f)
				speed.x = 0.0f;
		}
		else if (movingLeft == true) {
			speed.x += currentAcceleration;

			if (speed.x > 0.0f)
				speed.x = 0.0f;
		}
	}

	if (state == player_state::AIRBORNE) {
		Fall();
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

	// New position
	position.x += speed.x;
	position.y += speed.y;
}

//OLD PLAYER MOVEMENT
/*
void j1Player::MovePlayerOrig()
{
	// System: Each player state has a limited set of actions available, so we check the possible actions on each state
	switch (state) {
	case player_state::IDLE:
		animPtr = &idleAnim;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT
			||
			App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
			state = player_state::RUNNING;
		}
		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::AIRBORNE;
			speed.y -= jumpVelocity;
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			state = player_state::CROUCHING;
		}
		break;
	case player_state::CROUCHING:
		animPtr = &crouchAnim;

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) {
			state = player_state::IDLE;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::AIRBORNE;
			speed.y -= jumpVelocity;
		}
		break;
	case player_state::RUNNING:
		animPtr = &runAnim;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::AIRBORNE;
			speed.y -= jumpVelocity;
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			state = player_state::SLIDING;
		}
		else {
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {
				speed.x -= currentAcceleration;
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
				speed.x += currentAcceleration;
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
	case player_state::AIRBORNE:

		speed.y += gravity;

		if (speed.y > 0.0f) {
			animPtr = &fallAnim;
		}
		else if (somersaultUsed == false) {
			animPtr = &jumpAnim;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
			speed.y = -jumpVelocity;
			animPtr = &somersaultAnim;
			somersaultUsed = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {
			speed.x -= currentAcceleration;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
			speed.x += currentAcceleration;
		}

		if (position.y > 550) {		//CHANGE/FIX: Hardcoded values, this is where collision condition goes
			position.y = 550;

			speed.y = 0.0f;
			jumpAnim.Reset();
			somersaultAnim.Reset();
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
		animPtr = &slideAnim;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			state = player_state::AIRBORNE;
			animPtr = &jumpAnim;
			speed.y -= jumpVelocity;
			slideAnim.Reset();
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) {

			slideAnim.Reset();

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT
				||
				App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT
				||
				speed.x < 0.0f
				||
				speed.x > 0.0f) {
				state = player_state::RUNNING;
				animPtr = &runAnim;
			}
			else {
				state = player_state::IDLE;
				animPtr = &idleAnim;
			}
		}
		else {
			if (speed.x == 0.0f) {
				slideAnim.Reset();
				state = player_state::CROUCHING;
				animPtr = &crouchAnim;
			}
			else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT) {
				if (speed.x < 0.0f) {
					speed.x += currentAcceleration / 2.5;
				}
				else {
					speed.x = 0.0f;
					slideAnim.Reset();
					state = player_state::CROUCHING;
					animPtr = &crouchAnim;
				}
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT) {
				if (speed.x > 0.0f) {
					speed.x -= currentAcceleration / 2.5;
				}
				else {
					speed.x = 0.0f;
					slideAnim.Reset();
					state = player_state::CROUCHING;
					animPtr = &crouchAnim;
				}
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
			speed.x -= currentAcceleration;

			if (speed.x < 0.0f)
				speed.x = 0.0f;
		}
		else if (speed.x < 0.0f) {
			speed.x += currentAcceleration;

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
	state = player_state::AIRBORNE;
}

// New position
position.x += speed.x;
position.y += speed.y;
}
*/