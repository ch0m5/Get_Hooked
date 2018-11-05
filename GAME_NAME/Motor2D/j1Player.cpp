#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Player.h"
#include "j1Collision.h"
#include "j1FadeScene.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "p2Animation.h"

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

	pugi::xml_node first_sprite = config.child("sprites").child("first_sprite");
	ImportAllSprites(first_sprite);	// Import all sprite data

	// Animations
	AllocAllAnimations();	// Allocate all animations with previously recieved sprite data

	// Character stats and flags
	ImportAllStates(config.child("stats"));	// Import all state data from config.xml

	runSfxDelay = config.child("audio").child("runSfxDelay").attribute("miliseconds").as_int();
	playedSlideSfx = config.child("audio").child("slideSfx").attribute("played").as_bool();
	playedHurtSfx = config.child("audio").child("hurtSfx").attribute("played").as_bool();

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;

	pugi::xml_document map_data;
	pugi::xml_document config_data;
	pugi::xml_node root;

	pugi::xml_parse_result result = map_data.load_file("maps/testmap.tmx");	// SamAlert: Hardcoded string should get value from a xml file
	pugi::xml_parse_result result2 = config_data.load_file("config.xml");

	if (result != NULL && result2 != NULL)
	{
		root = map_data.first_child();

		respawnPosition.x = position.x = root.child("objectgroup").child("object").attribute("x").as_float();	// Put player on map initial position
		respawnPosition.y = position.y = root.child("objectgroup").child("object").attribute("y").as_float();

		root = config_data.child("player");

		offset.x = root.child("collision_offset").attribute("x").as_uint();	// CHANGE/FIX: This should probably be in Awake()
		offset.y = root.child("collision_ofset").attribute("y").as_uint();	// CHANGE/FIX: This should probably be in Awake()

		//hitbox->rect.x = (int)(position.x + offset.x);
		//hitbox->rect.y = (int)(position.y + offset.y);
		//hitbox->rect.w = root.child("hitbox").attribute("x").as_uint();
		//hitbox->rect.h = root.child("hitbox").attribute("y").as_uint();

		config_data.reset();
		map_data.reset();
	}
	else if (result == NULL)
		LOG("Map info not loaded. pugi error: %s", result.description());
	else
		LOG("conifg info not loaded. pugi error: %s", result.description());

	life = maxLife;
	currentAcceleration = normalAcceleration;
	state = player_state::IDLE;

	graphics = App->tex->Load(characterSheet.GetString());

	hitbox = App->collision->AddCollider({ (int)position.x + offset.x, (int)position.y + offset.y, spriteSize.x, spriteSize.y }, COLLIDER_PLAYER, this);		//CHANGE/FIX: Different collider sizes and adjusted sizes extracted from xml

	// CHANGE/FIX: HARDCODED TESTING
	position.x = 150;
	position.y = 500;

	return ret;
}

// Called each loop iteration
bool j1Player::PreUpdate()	// IMPROVE: Player input here?
{
	bool ret = true;

	return ret;
}

//input and logic
bool j1Player::Update(float dt)
{
	bool ret = true;

	if (debugMode == true) {
		DebugInput();	// Check debug input
	}

	PlayerInput();		// Check player input
	PlayerMovement();	// Check player current movement
	PlayerState();		// Check player state
	PlayerEffects();	// Add state effects like movement restrictions, animation and sounds
	MovePlayer();		// Move player position and calculate other movement related factors

	animRect.x = (int)position.x;
	animRect.y = (int)position.y;

	hitbox->rect.x = (int)(position.x + offset.x);
	hitbox->rect.y = (int)(position.y + offset.y);

	SDL_Rect playerRect = animPtr->GetCurrentFrame();

	if (lookingRight == true) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, &playerRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, &playerRect, SDL_FLIP_HORIZONTAL);
	}

	return ret;
}

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;
	
	LOG("Unloading player");

	App->tex->UnLoad(graphics);
	hitbox = nullptr;	// @Carles, Deassign collider from player for later CleanUp in j1Collision
	graphics = nullptr;

	return ret;
}

// Called when colliding
void j1Player::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->GetType() == COLLIDER_TYPE::COLLIDER_PLAYER && c2->GetType() == COLLIDER_TYPE::COLLIDER_WALL)
	{
		while (c1->CheckCollision(c2->rect) == true) {	//CHANGE/FIX: On Collision works to an extent!!!! NEEDS IMPROVEMENT AND FIXING
			c1->rect.y -= 0.1f;
		}

		position.y = c1->rect.y;	//CHANGE/FIX: Temporal workaround, needs fixing
		airborne = false;
	}
}

// Load Game State
bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_float();
	position.y = data.child("position").attribute("y").as_float();
	speed.x = data.child("speed").attribute("x").as_float();
	speed.y = data.child("speed").attribute("y").as_float();
	life = (ushort)data.child("life").attribute("value").as_uint();
	currentAcceleration = data.child("acceleration").attribute("type").as_float();
	state = (player_state)data.child("state").attribute("current").as_uint();
	airborne = data.child("airborne").attribute("value").as_bool();
	lookingRight = data.child("looking").attribute("right").as_bool();
	somersaultUsed = data.child("somersault").attribute("used").as_bool();
	dead = data.child("dead").attribute("value").as_bool();
	deadTimer = data.child("deadTimer").attribute("miliseconds").as_uint();
	playerReset = data.child("reset").attribute("value").as_bool();

	runSfxTimer = data.child("runSfxTimer").attribute("miliseconds").as_uint();
	playedSlideSfx = data.child("slideSfx").attribute("played").as_bool();
	playedHurtSfx = data.child("hurtSfx").attribute("played").as_bool();

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node tmpNode;

	tmpNode = data.append_child("position");
	tmpNode.append_attribute("x") = position.x;
	tmpNode.append_attribute("y") = position.y;

	tmpNode = data.append_child("speed");
	tmpNode.append_attribute("x") = speed.x;
	tmpNode.append_attribute("y") = speed.y;

	tmpNode = data.append_child("life");
	tmpNode.append_attribute("value") = life;

	tmpNode = data.append_child("acceleration");
	tmpNode.append_attribute("type") = currentAcceleration;

	tmpNode = data.append_child("state");
	tmpNode.append_attribute("current") = (uint)state;

	tmpNode = data.append_child("airborne");
	tmpNode.append_attribute("value") = airborne;

	tmpNode = data.append_child("looking");
	tmpNode.append_attribute("right") = lookingRight;

	tmpNode = data.append_child("somersault");
	tmpNode.append_attribute("used") = somersaultUsed;

	tmpNode = data.append_child("dead");
	tmpNode.append_attribute("value") = dead;

	tmpNode = data.append_child("deadTimer");
	tmpNode.append_attribute("miliseconds") = deadTimer;

	tmpNode = data.append_child("reset");
	tmpNode.append_attribute("value") = playerReset;

	tmpNode = data.append_child("runSfxTimer");
	tmpNode.append_attribute("miliseconds") = runSfxTimer;

	tmpNode = data.append_child("slideSfx");
	tmpNode.append_attribute("played") = playedSlideSfx;

	tmpNode = data.append_child("hurtSfx");
	tmpNode.append_attribute("played") = playedHurtSfx;

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

// Import all sprite data using the above function for each animation
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

// Import all state data from config.xml
void j1Player::ImportAllStates(pugi::xml_node& config)
{
	// Character stats
	maxLife = (ushort)config.child("life").attribute("value").as_uint();
	speed = { config.child("speed").attribute("x").as_float(), config.child("speed").attribute("y").as_float() };
	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };
	hurtSpeed = { config.child("hurtSpeed").attribute("x").as_float(), config.child("hurtSpeed").attribute("y").as_float() };
	normalAcceleration = config.child("accelerations").attribute("x").as_float();
	slideAcceleration = config.child("accelerations").attribute("slide").as_float();
	gravity = config.child("accelerations").attribute("gravity").as_float();
	jumpVelocity = config.child("jump").attribute("forceY").as_float();

	// Character status flags and directly related data
	airborne = config.child("airborne").attribute("value").as_bool();
	lookingRight = config.child("looking").attribute("right").as_bool();
	somersaultUsed = config.child("somersault").attribute("used").as_bool();
	dead = config.child("dead").attribute("value").as_bool();
	deathDelay = config.child("deathDelay").attribute("miliseconds").as_int();
	fading = config.child("fading").attribute("value").as_bool();
	fadeDelay = config.child("fadeDelay").attribute("seconds").as_float();
	playerReset = config.child("reset").attribute("value").as_bool();
	debugMode = config.child("debugMode").attribute("value").as_bool();
	godMode = config.child("godMode").attribute("value").as_bool();
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

// Player actions
// Add Y speed when jump requested
player_state j1Player::Jump()	
{
	speed.y = -jumpVelocity;
	airborne = true;
	App->audio->PlayFx(App->audio->jumpSfx.id, 0);
	return player_state::JUMPING;
}

// Add acceleration to Y speed
void j1Player::Fall()
{
	speed.y += gravity;
}

// Stop Y speed
void j1Player::Land()
{
	speed.y = 0.0f;
	somersaultUsed = false;
	jumpAnim.Reset();
}

void j1Player::StandUp() {
	currentAcceleration = normalAcceleration;
	playedSlideSfx = false;
	slideAnim.Reset();
}

// Stop and move slightly up and opposite of current direction, player state changes to AIRBORNE
player_state j1Player::Hurt()
{
	if (lookingRight == true) {
		speed.x = -hurtSpeed.x;
	}
	else {
		speed.x = hurtSpeed.x;
	}

	speed.y = -hurtSpeed.y;
	airborne = true;

	if (--life == 0) {
		dead = true;
		deadTimer = SDL_GetTicks();
	}
	
	playerReset = false;
	playedHurtSfx = false;
	return player_state::HURT;
}

//Check debug input
void j1Player::DebugInput()
{
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && state != player_state::HURT && godMode == false) {	// Hurt player
		state = Hurt();
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && App->player->IsDead() == false) {	// Load game
		App->LoadGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && App->player->IsDead() == false) {	// Save game
		App->SaveGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->collision->mustDebugDraw == false) {	// Logic drawing
		App->collision->mustDebugDraw = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->collision->mustDebugDraw == true) {
		App->collision->mustDebugDraw = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && godMode == false) {	// GodMode
		godMode = true;
		playerReset = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && godMode == true) {
		godMode = false;
	}

	// Change scale
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		App->win->scale = 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		App->win->scale = 2;
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		App->win->scale = 3;
	}
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
		App->win->scale = 4;
	}
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) {
		App->win->scale = 5;
	}
}

//Check player input
void j1Player::PlayerInput()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {		// Move left input
		wantMoveLeft = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		wantMoveLeft = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {		// Move right input
		wantMoveRight = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		wantMoveRight = true;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE) {		// Move up input
		wantMoveUp = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		wantMoveUp = true;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE) {		// Move down input
		wantMoveDown = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		wantMoveDown = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && debugMode == false) {	// Activate debug mode input	// CHANGE/FIX: Change README
		debugMode = true;
		App->scene->debugTitle();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && debugMode == true) {
		debugMode = false;
		godMode = false;
		App->collision->mustDebugDraw = false;
		App->win->scale = App->win->origScale;
		App->scene->origTitle();
	}
}

// Check player current movement
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

// Check player state
void j1Player::PlayerState() {	// For each state, check possible new states based on other parameters
	if (godMode == true) {	//if free movement ON

	}
	else {	// if no ground -> airborne = true;
		// CHANGE/FIX: To do this, we should need a pointer to the current player ground collider, the one directly below it. Either that or raycasting to the ground
	}
	switch (state) {
	case player_state::IDLE:
		state = IdleMoveCheck();	//CHECK_ERIC: All are missing a condition: if not touching the ground, state change to falling and maybe something else (ariborne = true)
		break;
	case player_state::CROUCHING:
		state = CrouchingMoveCheck();
		break;
	case player_state::RUNNING:
		state = RunningMoveCheck();
		break;
	case player_state::JUMPING:
		state = JumpingMoveCheck();
		break;
	case player_state::FALLING:
		state = FallingMoveCheck();
		break;
	case player_state::SLIDING:
		state = SlidingMoveCheck();
		break;
	case player_state::HURT:
		state = HurtMoveCheck();
		break;
	}
}

player_state j1Player::IdleMoveCheck()
{
	player_state ret = player_state::IDLE;

	if (airborne == true) {
		ret = player_state::FALLING;
	}
	else if (wantMoveRight == true && wantMoveLeft == false || wantMoveLeft == true && wantMoveRight == false) {
		ret = player_state::RUNNING;
	}
	else if (wantMoveUp == true) {
		ret = Jump();
	}
	else if (wantMoveDown == true) {
		ret = player_state::CROUCHING;
	}

	return ret;
}

player_state j1Player::CrouchingMoveCheck()
{
	player_state ret = player_state::CROUCHING;

	if (airborne == true) {
		ret = player_state::FALLING;
	}
	else if (wantMoveDown == false) {
		if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true)
			ret = player_state::RUNNING;
		else {
			ret = player_state::IDLE;
		}
	}
	else if (wantMoveUp == true) {
		ret = Jump();
	}

	return ret;
}

player_state j1Player::RunningMoveCheck()
{
	player_state ret = player_state::RUNNING;

	if (airborne == true) {
		ret = player_state::FALLING;
	}
	else if (wantMoveUp == true) {
		ret = Jump();
	}
	else if (wantMoveDown == true) {
		ret = player_state::SLIDING;
	}
	else if (movingLeft == false && movingRight == false) {
		if (wantMoveRight == false && wantMoveLeft == false || wantMoveRight == true && wantMoveLeft == true) {
			ret = player_state::IDLE;
		}
	}

	return ret;
}

player_state j1Player::JumpingMoveCheck()
{
	player_state ret = player_state::JUMPING;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		somersaultAnim.Reset();
		ret = Jump();
		somersaultUsed = true;
	}
	else if (movingDown == true) {
		ret = player_state::FALLING;
	}

	return ret;
}

player_state j1Player::FallingMoveCheck()
{
	player_state ret = player_state::FALLING;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		somersaultAnim.Reset();
		ret = Jump();
		somersaultUsed = true;
	}
	else if (airborne == false) {		//SamAlert: Hardcoded values, this condition should be "if feet collision", 
		Land();

		if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true) {
			if (wantMoveDown == true) {
				ret = player_state::SLIDING;
			}
			else {
				ret = player_state::RUNNING;
			}
		}
		else if (wantMoveDown == true) {
			ret = player_state::CROUCHING;
		}
		else {
			ret = player_state::IDLE;
		}		
	}
	if (position.y > 800) {	//CHANGE/FIX: Hardcoded fallen pit	//CHANGE/FIX: Create pit function
		if (godMode == false) {
			dead = true;
			deadTimer = SDL_GetTicks();
			playerReset = false;
			playedHurtSfx = false;
			ret = player_state::HURT;
		}
		else {
			position = lastGroundPosition;
		}
	}

	return ret;
}

player_state j1Player::SlidingMoveCheck()
{
	player_state ret = player_state::SLIDING;

	if (airborne == true) {
		StandUp();
		ret = player_state::FALLING;
	}
	else if (wantMoveUp == true) {
		StandUp();
		ret = Jump();
	}
	else if (wantMoveDown == false) {
		StandUp();

		if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true) {
			ret = player_state::RUNNING;
		}
		else {
			ret = player_state::IDLE;
		}
	}
	else if (movingLeft == false && movingRight == false) {
		StandUp();

		if (wantMoveDown == true) {
			ret = player_state::CROUCHING;
		}
		else {
			ret = player_state::IDLE;
		}
	}

	return ret;
}

player_state j1Player::HurtMoveCheck()
{
	player_state ret = player_state::HURT;

	if (airborne == false) {		//SamAlert: Hardcoded values, this condition should be "if feet collision", 
		Land();

		if (dead == false) {
			if (wantMoveRight == true || wantMoveLeft == true || movingRight == true || movingLeft == true) {
				if (wantMoveDown == true) {
					ret = player_state::SLIDING;
				}
				else {
					ret = player_state::RUNNING;
				}
			}
			else if (wantMoveDown == true) {
				ret = player_state::CROUCHING;
			}
			else {
				ret = player_state::IDLE;
			}
		}
	}
	if (position.y > 800) {	//CHANGE/FIX: Hardcoded fallen pit	//CHANGE/FIX: Create pit function
		if (godMode == false && dead == false) {
			dead = true;
			deadTimer = SDL_GetTicks();
			playerReset = false;
			playedHurtSfx = false;
		}
		else if (godMode == true) {
			position = lastGroundPosition;
		}
	}

	return ret;
}

// Add state effects like movement restrictions, animation and sounds
void j1Player::PlayerEffects()
{
	if (state != player_state::SLIDING && state != player_state::HURT) {		//CHANGE/FIX: Make function
		if (wantMoveRight == true && wantMoveLeft == false) {
			lookingRight = true;
		}
		else if (wantMoveLeft == true && wantMoveRight == false) {
			lookingRight = false;
		}
	}

	if (playerReset == false) {	//CHANGE/FIX: Make function
		jumpAnim.Reset();
		somersaultUsed = false;

		currentAcceleration = normalAcceleration;
		playedSlideSfx = false;
		slideAnim.Reset();

		playerReset = true;
	}

	if (airborne == false) {
		lastGroundPosition = position;
	}

	switch (state) {
	case player_state::IDLE:
		IdleEffects();
		break;
	case player_state::CROUCHING:
		CrouchingEffects();
		break;
	case player_state::RUNNING:
		RunningEffects();
		break;
	case player_state::JUMPING:
		JumpingEffects();
		break;
	case player_state::FALLING:
		FallingEffects();
		break;
	case player_state::SLIDING:
		SlidingEffects();
		break;
	case player_state::HURT:
		HurtEffects();
		break;
	}
}

void j1Player::IdleEffects()
{
	animPtr = &idleAnim;
}

void j1Player::CrouchingEffects()
{
	wantMoveRight = false;
	wantMoveLeft = false;
	animPtr = &crouchAnim;
}

void j1Player::RunningEffects()
{
	if (runSfxTimer < SDL_GetTicks() - runSfxDelay) {	// IMPROVE: Gets out of tempo after a few plays.
		App->audio->PlayFx(App->audio->runSfx.id, 0);
		runSfxTimer = SDL_GetTicks();
	}
	animPtr = &runAnim;
}

void j1Player::JumpingEffects()
{
	if (somersaultUsed == true) {
		animPtr = &somersaultAnim;
	}
	else {
		animPtr = &jumpAnim;
	}
}

void j1Player::FallingEffects()
{
	animPtr = &fallAnim;
}

void j1Player::SlidingEffects()
{
	wantMoveRight = false;
	wantMoveLeft = false;
	currentAcceleration = slideAcceleration;

	if (playedSlideSfx == false) {
		App->audio->PlayFx(App->audio->slideSfx.id, 0);
		playedSlideSfx = true;
	}
	animPtr = &slideAnim;
}

void j1Player::HurtEffects()
{
	wantMoveUp = false;
	wantMoveDown = false;
	wantMoveRight = false;
	wantMoveLeft = false;
	if (playedHurtSfx == false) {
		App->audio->PlayFx(App->audio->hurtSfx.id, 0);
		playedHurtSfx = true;
	}

	if (dead == true) {
		DeadEffects();
	}
	else {
		animPtr = &hurtAnim;
	}
}

void j1Player::DeadEffects() {
	if (fading == false && deadTimer < SDL_GetTicks() - deathDelay) {
		App->fade->FadeToBlack(fadeDelay);
		fading = true;
	}
	else if (fading == true && deadTimer < SDL_GetTicks() - deathDelay - fadeDelay * 1000 / 2) {
		deadAnim.Reset();
		playedHurtSfx = false;
		dead = false;
		fading = false;
		App->LoadGame();	// SamAlert: For now it loads the last save when it's fully faded to black, decide what happens
	}
	else {
		wantMoveUp = false;
		wantMoveDown = false;
		wantMoveRight = false;
		wantMoveLeft = false;
		if (playedHurtSfx == false) {
			App->audio->PlayFx(App->audio->hurtSfx.id, 0);
			playedHurtSfx = true;
		}
		animPtr = &deadAnim;
	}
}

// Move player position and decide/calculate other movement related factors
void j1Player::MovePlayer()
{
	if (wantMoveRight == true && wantMoveLeft == false) {
		speed.x += currentAcceleration;
	}
	else if (wantMoveLeft == true && wantMoveRight == false) {
		speed.x -= currentAcceleration;
	}
	else if (airborne == false) {	// Natural deacceleration when on ground	//CHANGE/FIX: Make better condition
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

	// If on air, apply gravity
	if (airborne == true) {	//CHANGE/FIX: MAKE BETTER CONDITION
		Fall();
	}
	
	// Max Speeds	//CHANGE/FIX: Make function
	if (speed.x > 0)
		speed.x = MIN(speed.x, maxSpeed.x);
	else if (speed.x < 0)
		speed.x = MAX(speed.x, -maxSpeed.x);

	if (speed.y > 0)
		speed.y = MIN(speed.y, maxSpeed.y);
	else if (speed.y < 0)
		speed.y = MAX(speed.y, -maxSpeed.y);

	//speed = hitbox->AvoidCollision(speed, *hitbox);	//CHECK_ERIC: COLLISION FUNCTIONALITY

	// New position
	position.x += speed.x;
	position.y += speed.y;

	// Move camera with player within set limits	//CHANGE/FIX: MAKE FUNCTION
	if (App->render->camera.x < (int)-(position.x * App->win->GetScale() - 350)/* && mapRightLimit is not crossed*/) {	//left
		App->render->camera.x = (int)-(position.x * App->win->GetScale() - 350);
	}
	else if (App->render->camera.x > (int)-(position.x * App->win->GetScale() - 500)/* && mapLeftLimit is not crossed*/) {	//right
		App->render->camera.x = (int)-(position.x * App->win->GetScale() - 500);
	}

	if (App->render->camera.y < (int)-(position.y * App->win->GetScale() - 300)/* && mapRightLimit is not crossed*/) {	//left
		App->render->camera.y = (int)-(position.y * App->win->GetScale() - 300);
	}
	else if (App->render->camera.y > (int)-(position.y * App->win->GetScale() - 400)/* && mapLeftLimit is not crossed*/) {	//right
		App->render->camera.y = (int)-(position.y * App->win->GetScale() - 400);
	}
}