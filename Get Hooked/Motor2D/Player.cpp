#include "p2Defs.h"
#include "p2Log.h"
#include "p2Animation.h"
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
#include "Entity.h"
#include "Player.h"

Player::Player() : Entity(entity_type::PLAYER)
{
	name.create("player");
}

Player::~Player()
{}

// Called before render is available
bool Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player Data");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	// Character stats and flags
	ImportAllStates(config.child("stats"));	// Import all state data from config.xml

	// Sprites
	characterSheet.create("%s%s", folder.GetString(), config.child("sprites").child("spriteSheet").child_value());
	spriteSize = { config.child("sprites").child("spriteSize").attribute("x").as_int(), config.child("sprites").child("spriteSize").attribute("y").as_int() };

	pugi::xml_node first_sprite = config.child("sprites").child("first_sprite");
	ImportAllSprites(first_sprite);	// Import all sprite data

	// Animations
	AllocAllAnimations();	// Allocate all animations with previously recieved sprite data

	runSfxDelay = config.child("audio").child("runSfxDelay").attribute("miliseconds").as_int();
	playedSlideSfx = config.child("audio").child("slideSfx").attribute("played").as_bool();
	playedHurtSfx = config.child("audio").child("hurtSfx").attribute("played").as_bool();

	return ret;
}

// Called before the first frame
bool Player::Start()
{
	bool ret = true;

	dead = false;
	life = maxLife;
	acceleration.x = normalAcceleration;
	hitboxOffset = idleSprite.colliderOffset;
	status = state::IDLE;

	if (App->scene->active)
	{
		position = lastGroundPosition = respawnPosition = App->scene->playerPos;
	}
	else if (App->scene2->active)
	{
		position = lastGroundPosition = respawnPosition = App->scene2->playerPos;
	}

	graphics = App->tex->Load(characterSheet.GetString());
	
	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_PLAYER, this);
	hitboxOffset = hitbox->rect;

	return ret;
}

// Called each loop iteration
bool Player::PreUpdate()
{
	bool ret = true;

	if (debugMode == true) {
		DebugInput();	// Check debug input
	}

	CheckInput();		// Check player input
	CheckMovement();	// Check player current movement

	return ret;
}

//input and logic
bool Player::UpdateTick(float dt)
{
	bool ret = true;

	CheckState();	// Check player state
	ApplyState();	// Add state effects like movement restrictions, animation and sounds
	Move(dt);		// Move player position and calculate other movement related factors
	UpdateHitbox();	// Transform player collider depending on new position and state

	animRect = animPtr->GetCurrentFrame(dt);

	return ret;
}

bool Player::Update()
{
	bool ret = true;

	if (lookingRight == true) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, &animRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, &animRect, SDL_FLIP_HORIZONTAL);
	}

	return ret;
}

// Called before quitting
bool Player::CleanUp()
{
	bool ret = true;
	
	LOG("Unloading player");

	App->tex->UnLoad(graphics);
	position = respawnPosition;
	hitbox = nullptr;	// @Carles, Deassign collider from player for later CleanUp in j1Collision
	graphics = nullptr;

	return ret;
}

// Called when colliding
collision_type Player::OnCollision(Collider* c1, Collider* c2)
{
	collision_type ret = collision_type::NONE;

	if (godMode == false) {
		if (c1->GetType() == collider_type::COLLIDER_PLAYER && c2->GetType() == collider_type::COLLIDER_WALL) {
			ret = WallCollision(c1, c2);
		}
	}

	return ret;
}

collision_type Player::WallCollision(Collider* c1, Collider* c2)
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
			c1->rect.y--;
			speed.y = 0.0f;
			ret = collision_type::ON_TOP;	//IMPROVE: On high framerates, player doesn't go down immediately after colliding, which it should
		}

		position.y = c1->rect.y - hitboxOffset.y;
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

		position.x = c1->rect.x - hitboxOffset.x;
	}

	return ret;
}

// Load Game State
bool Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_float();
	position.y = data.child("position").attribute("y").as_float();
	lastGroundPosition.x = data.child("lastGround").attribute("x").as_float();
	lastGroundPosition.y = data.child("lastGround").attribute("y").as_float();
	respawnPosition.x = data.child("respawn").attribute("x").as_float();
	respawnPosition.y = data.child("respawn").attribute("y").as_float();

	speed.x = data.child("speed").attribute("x").as_float();
	speed.y = data.child("speed").attribute("y").as_float();
	life = (ushort)data.child("life").attribute("value").as_uint();
	acceleration.x = data.child("acceleration").attribute("type").as_float();
	status = (state)data.child("state").attribute("current").as_uint();
	airborne = data.child("airborne").attribute("value").as_bool();
	lookingRight = data.child("looking").attribute("right").as_bool();
	somersaultUsed = data.child("somersault").attribute("used").as_bool();
	dead = data.child("dead").attribute("value").as_bool();
	deadTimer = data.child("deadTimer").attribute("miliseconds").as_uint();
	mustReset = data.child("reset").attribute("value").as_bool();

	runSfxTimer = data.child("runSfxTimer").attribute("miliseconds").as_uint();
	playedSlideSfx = data.child("slideSfx").attribute("played").as_bool();
	playedHurtSfx = data.child("hurtSfx").attribute("played").as_bool();

	return true;
}

// Save Game State
bool Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node tmpNode;

	tmpNode = data.append_child("position");
	tmpNode.append_attribute("x") = position.x;
	tmpNode.append_attribute("y") = position.y;

	tmpNode = data.append_child("lastGround");
	tmpNode.append_attribute("x") = lastGroundPosition.x;
	tmpNode.append_attribute("y") = lastGroundPosition.y;

	tmpNode = data.append_child("respawn");
	tmpNode.append_attribute("x") = respawnPosition.x;
	tmpNode.append_attribute("y") = respawnPosition.y;

	tmpNode = data.append_child("speed");
	tmpNode.append_attribute("x") = speed.x;
	tmpNode.append_attribute("y") = speed.y;

	tmpNode = data.append_child("life");
	tmpNode.append_attribute("value") = life;

	tmpNode = data.append_child("acceleration");
	tmpNode.append_attribute("type") = acceleration.x;

	tmpNode = data.append_child("state");
	tmpNode.append_attribute("current") = (uint)status;

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
	tmpNode.append_attribute("value") = mustReset;

	tmpNode = data.append_child("runSfxTimer");
	tmpNode.append_attribute("miliseconds") = runSfxTimer;

	tmpNode = data.append_child("slideSfx");
	tmpNode.append_attribute("played") = playedSlideSfx;

	tmpNode = data.append_child("hurtSfx");
	tmpNode.append_attribute("played") = playedHurtSfx;

	return true;
}

//------------------------------------------------

// Import all state data from config.xml
void Player::ImportAllStates(pugi::xml_node& config)
{
	// Character stats
	maxLife = (ushort)config.child("life").attribute("value").as_uint();
	speed = { config.child("speed").attribute("x").as_float(), config.child("speed").attribute("y").as_float() };
	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };
	hurtSpeed = { config.child("hurtSpeed").attribute("x").as_float(), config.child("hurtSpeed").attribute("y").as_float() };
	normalAcceleration = config.child("accelerations").attribute("x").as_float();
	acceleration.y = config.child("accelerations").attribute("y").as_float();
	slideAcceleration = config.child("accelerations").attribute("slide").as_float();
	gravity = config.child("accelerations").attribute("gravity").as_float();

	// Character status flags and directly related data
	airborne = config.child("airborne").attribute("value").as_bool();
	lookingRight = config.child("looking").attribute("right").as_bool();
	somersaultUsed = config.child("somersault").attribute("used").as_bool();;
	deathDelay = config.child("deathDelay").attribute("miliseconds").as_uint();
	fadeDelay = config.child("fadeDelay").attribute("seconds").as_float();
	debugMode = config.child("debugMode").attribute("value").as_bool();
	godMode = config.child("godMode").attribute("value").as_bool();
	freeCamera = config.child("freeCamera").attribute("value").as_bool();
}

// Imports from the xml file all data of the first sprite of each animation and other important data like animation speed, frames and if it loops
void Player::ImportSpriteData(const char* spriteName, sprite_data* sprite, pugi::xml_node& first_sprite)
{
	sprite->sheetPosition.x = first_sprite.child(spriteName).attribute("column").as_int();
	sprite->sheetPosition.y = first_sprite.child(spriteName).attribute("row").as_int();
	sprite->numFrames = first_sprite.child(spriteName).attribute("frames").as_uint();
	sprite->anim.speed = first_sprite.child(spriteName).attribute("animSpeed").as_float();
	sprite->anim.loop = first_sprite.child(spriteName).attribute("loop").as_bool();

	sprite->colliderOffset.x = first_sprite.child(spriteName).child("offset").attribute("x").as_int();
	sprite->colliderOffset.y = first_sprite.child(spriteName).child("offset").attribute("y").as_int();
	sprite->colliderOffset.w = first_sprite.child(spriteName).child("offset").attribute("w").as_int();
	sprite->colliderOffset.h = first_sprite.child(spriteName).child("offset").attribute("h").as_int();
}

// Import all sprite data using the above function for each animation
void Player::ImportAllSprites(pugi::xml_node& first_sprite)
{
	ImportSpriteData("idle", &idleSprite, first_sprite);
	ImportSpriteData("run", &runSprite, first_sprite);
	ImportSpriteData("slide", &slideSprite, first_sprite);
	ImportSpriteData("crouch", &crouchSprite, first_sprite);
	ImportSpriteData("jump", &jumpSprite, first_sprite);
	ImportSpriteData("somersault", &somersaultSprite, first_sprite);
	ImportSpriteData("fall", &fallSprite, first_sprite);
	ImportSpriteData("hurt", &hurtSprite, first_sprite);
	ImportSpriteData("dead", &deadSprite, first_sprite);
}

// Allocates all animations using the AllocAnimation function and parameters related to their sprite sheet location extracted from the config.xml file
void Player::AllocAllAnimations()
{
	idleSprite.anim.AllocAnimation({ idleSprite.sheetPosition.x * spriteSize.x, idleSprite.sheetPosition.y * spriteSize.y }, spriteSize, idleSprite.numFrames);
	runSprite.anim.AllocAnimation({ runSprite.sheetPosition.x * spriteSize.x, runSprite.sheetPosition.y * spriteSize.y }, spriteSize, runSprite.numFrames);
	slideSprite.anim.AllocAnimation({ slideSprite.sheetPosition.x * spriteSize.x, slideSprite.sheetPosition.y * spriteSize.y }, spriteSize, slideSprite.numFrames);
	crouchSprite.anim.AllocAnimation({ crouchSprite.sheetPosition.x * spriteSize.x, crouchSprite.sheetPosition.y * spriteSize.y }, spriteSize, crouchSprite.numFrames);
	jumpSprite.anim.AllocAnimation({ jumpSprite.sheetPosition.x * spriteSize.x, jumpSprite.sheetPosition.y * spriteSize.y }, spriteSize, jumpSprite.numFrames);
	somersaultSprite.anim.AllocAnimation({ somersaultSprite.sheetPosition.x * spriteSize.x, somersaultSprite.sheetPosition.y * spriteSize.y }, spriteSize, somersaultSprite.numFrames);
	fallSprite.anim.AllocAnimation({ fallSprite.sheetPosition.x * spriteSize.x, fallSprite.sheetPosition.y * spriteSize.y }, spriteSize, fallSprite.numFrames);
	hurtSprite.anim.AllocAnimation({ hurtSprite.sheetPosition.x * spriteSize.x, hurtSprite.sheetPosition.y * spriteSize.y }, spriteSize, hurtSprite.numFrames);
	deadSprite.anim.AllocAnimation({ deadSprite.sheetPosition.x * spriteSize.x, deadSprite.sheetPosition.y * spriteSize.y }, spriteSize, deadSprite.numFrames);
}

//------------------------------------------------

// Player actions
// Add Y speed when jump requested
void Player::Jump()	
{
	speed.y = -acceleration.y;
	airborne = true;
	App->audio->PlayFx(App->audio->jumpSfx.id, 0);
}

// Add acceleration to Y speed
void Player::Fall(float dt)
{
	speed.y += gravity * dt;
}

// Stop X speed
void Player::LateralStop()
{
	speed.x = 0.0f;
}

// Stop Y speed
void Player::Land()
{
	speed.y = 0.0f;
	somersaultUsed = false;
	jumpSprite.anim.Reset();
}

void Player::StandUp() {
	acceleration.x = normalAcceleration;
	playedSlideSfx = false;
	slideSprite.anim.Reset();
}

// Stop and move slightly up and opposite of current direction, player state changes to AIRBORNE
void Player::Hurt()
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
	
	mustReset = true;
	playedHurtSfx = false;
}

//Restart all player values that need so when skipping steps on the workflow
void Player::PlayerReset()
{
	jumpSprite.anim.Reset();
	somersaultUsed = false;

	acceleration.x = normalAcceleration;
	playedSlideSfx = false;
	slideSprite.anim.Reset();
}

//Check debug input
void Player::DebugInput()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)	//CHANGE/FIX: Should be in scene?
	{
		if (App->scene2->active)
			App->scene2->ChangeScene();

		else if (App->scene->active)
		{
			App->scene->CleanUp();
			App->fade->FadeToBlack(App->scene, App->scene);
			App->scene->Start();
			App->entityManager->player->Start();
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)	//CHANGE/FIX: Should be in scene?
	{
		if (App->scene2->active)
		{
			App->scene2->CleanUp();
			App->fade->FadeToBlack(App->scene2, App->scene2);
			App->scene2->Start();
			App->entityManager->player->Start();
		}

		else if (App->scene->active)
		{
			App->scene->CleanUp();
			App->fade->FadeToBlack(App->scene, App->scene);
			App->scene->Start();
			App->entityManager->player->Start();
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && freeCamera == false) {	// Free camera
		freeCamera = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && freeCamera == true) {
		freeCamera = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && App->entityManager->player->IsDead() == false) {	// Save game
		App->SaveGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && App->entityManager->player->IsDead() == false) {	// Load game
		App->LoadGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && status != state::HURT && godMode == false) {	// Hurt player
		Hurt();
		status = state::HURT;
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->collision->mustDebugDraw == false) {	// Logic drawing
		App->collision->mustDebugDraw = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->collision->mustDebugDraw == true) {
		App->collision->mustDebugDraw = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && godMode == false) {	// GodMode
		godMode = true;
		speed = { 0, 0 };
		status = state::FALLING;
		mustReset = true;
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
void Player::CheckInput()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {		// Move left input
		input.wantMoveLeft = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		input.wantMoveLeft = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {		// Move right input
		input.wantMoveRight = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		input.wantMoveRight = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE) {		// Move up input
		input.wantMoveUp = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		input.wantMoveUp = true;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE) {		// Move down input
		input.wantMoveDown = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		input.wantMoveDown = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && debugMode == false) {	// Activate debug mode input
		debugMode = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && debugMode == true) {	// Improve: Turning off debugMode shouldn't the change other flag values, just evaluate debugMode itself when their values are checked
		debugMode = false;
		godMode = false;
		freeCamera = false;
		App->collision->mustDebugDraw = false;
		App->win->scale = App->win->origScale;
	}
}

//// Check player current movement	//CHANGE/FIX: ERASE?
//void Player::CheckMovement() {
//	if (speed.x > 0.0f) {
//		movement.movingRight = true;
//		movement.movingLeft = false;
//	}
//	else if (speed.x < 0.0f) {
//		movement.movingLeft = true;
//		movement.movingRight = false;
//	}
//	else if (speed.x == 0.0f) {
//		movement.movingLeft = false;
//		movement.movingRight = false;
//	}
//
//	if (speed.y < 0.0f) {
//		movement.movingUp = true;
//		movement.movingDown = false;
//	}
//	else if (speed.y > 0.0f) {
//		movement.movingDown = true;
//		movement.movingUp = false;
//	}
//	else if (speed.y == 0.0f) {
//		movement.movingUp = false;
//		movement.movingDown = false;
//	}
//}

// Check player state
void Player::CheckState() {	// For each state, check possible new states based on other parameters
	if (godMode == true) {
		airborne = true;

		if (position.y > 800) {		//CHANGE/FIX: Hardcoded fallen pit
			position = lastGroundPosition;
		}
	}
	else {
		if (App->collision->CheckGroundCollision(hitbox) == false)
			airborne = true;

		switch (status) {
		case state::IDLE:
			status = IdleMoveCheck();
			break;
		case state::CROUCHING:
			status = CrouchingMoveCheck();
			break;
		case state::RUNNING:
			status = RunningMoveCheck();
			break;
		case state::JUMPING:
			status = JumpingMoveCheck();
			break;
		case state::FALLING:
			status = FallingMoveCheck();
			break;
		case state::SLIDING:
			status = SlidingMoveCheck();
			break;
		case state::HURT:
			status = HurtMoveCheck();
			break;
		}
	}
}

state Player::IdleMoveCheck()
{
	state ret = state::IDLE;

	if (airborne == true) {
		ret = state::FALLING;
	}
	else if (input.wantMoveRight == true && input.wantMoveLeft == false || input.wantMoveLeft == true && input.wantMoveRight == false) {
		ret = state::RUNNING;
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = state::JUMPING;
	}
	else if (input.wantMoveDown == true) {
		ret = state::CROUCHING;
	}

	return ret;
}

state Player::CrouchingMoveCheck()
{
	state ret = state::CROUCHING;

	if (airborne == true) {
		ret = state::FALLING;
	}
	else if (input.wantMoveDown == false) {
		if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true)
			ret = state::RUNNING;
		else {
			ret = state::IDLE;
		}
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = state::JUMPING;
	}

	return ret;
}

state Player::RunningMoveCheck()
{
	state ret = state::RUNNING;

	if (airborne == true) {
		ret = state::FALLING;
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = state::JUMPING;
	}
	else if (input.wantMoveDown == true) {
		ret = state::SLIDING;
	}
	else if (movement.movingLeft == false && movement.movingRight == false) {
		if (input.wantMoveRight == false && input.wantMoveLeft == false || input.wantMoveRight == true && input.wantMoveLeft == true) {
			ret = state::IDLE;
		}
	}

	return ret;
}

state Player::JumpingMoveCheck()
{
	state ret = state::JUMPING;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		somersaultSprite.anim.Reset();
		Jump();
		ret = state::JUMPING;
		somersaultUsed = true;
	}
	else if (movement.movingDown == true) {
		ret = state::FALLING;
	}

	return ret;
}

state Player::FallingMoveCheck()
{
	state ret = state::FALLING;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		somersaultSprite.anim.Reset();
		Jump();
		ret = state::JUMPING;
		somersaultUsed = true;
	}
	else if (airborne == false) {		//SamAlert: Hardcoded values, this condition should be "if feet collision", 
		Land();

		if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true) {
			if (input.wantMoveDown == true) {
				ret = state::SLIDING;
			}
			else {
				ret = state::RUNNING;
			}
		}
		else if (input.wantMoveDown == true) {
			ret = state::CROUCHING;
		}
		else {
			ret = state::IDLE;
		}		
	}
	if (position.y > 800) {	//CHANGE/FIX: Hardcoded fallen pit	//CHANGE/FIX: Create pit function
		dead = true;
		deadTimer = SDL_GetTicks();
		mustReset = true;
		playedHurtSfx = false;
		ret = state::HURT;
	}

	return ret;
}

state Player::SlidingMoveCheck()
{
	state ret = state::SLIDING;

	if (airborne == true) {
		StandUp();
		ret = state::FALLING;
	}
	else if (input.wantMoveUp == true) {
		StandUp();
		Jump();
		ret = state::JUMPING;
	}
	else if (input.wantMoveDown == false) {
		StandUp();

		if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true) {
			ret = state::RUNNING;
		}
		else {
			ret = state::IDLE;
		}
	}
	else if (movement.movingLeft == false && movement.movingRight == false) {
		StandUp();

		if (input.wantMoveDown == true) {
			ret = state::CROUCHING;
		}
		else {
			ret = state::IDLE;
		}
	}

	return ret;
}

state Player::HurtMoveCheck()
{
	state ret = state::HURT;

	if (airborne == false) {		//SamAlert: Hardcoded values, this condition should be "if feet collision", 
		Land();

		if (dead == false) {
			if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true) {
				if (input.wantMoveDown == true) {
					ret = state::SLIDING;
				}
				else {
					ret = state::RUNNING;
				}
			}
			else if (input.wantMoveDown == true) {
				ret = state::CROUCHING;
			}
			else {
				ret = state::IDLE;
			}
		}
	}
	if (position.y > 800) {	//CHANGE/FIX: Hardcoded fallen pit	//CHANGE/FIX: Create pit function
		dead = true;
		deadTimer = SDL_GetTicks();
		mustReset = true;
		playedHurtSfx = false;
		ret = state::HURT;
	}

	return ret;
}

// Add state effects like movement restrictions, animation and sounds
void Player::ApplyState()
{
	if (status != state::SLIDING && status != state::HURT) {
		lookingRight = CheckPlayerOrientation(lookingRight);
	}

	if (mustReset) {
		PlayerReset();
		mustReset = false;
	}

	if (airborne == false) {
		lastGroundPosition = position;
	}

	switch (status) {
	case state::IDLE:
		IdleEffects();
		break;
	case state::CROUCHING:
		CrouchingEffects();
		break;
	case state::RUNNING:
		RunningEffects();
		break;
	case state::JUMPING:
		JumpingEffects();
		break;
	case state::FALLING:
		FallingEffects();
		break;
	case state::SLIDING:
		SlidingEffects();
		break;
	case state::HURT:
		HurtEffects();
		break;
	}
}

bool Player::CheckPlayerOrientation(bool orientation)
{
	bool ret = orientation;

	if (input.wantMoveRight == true && input.wantMoveLeft == false) {
		orientation = true;
	}
	else if (input.wantMoveLeft == true && input.wantMoveRight == false) {
		orientation = false;
	}

	return orientation;
}

void Player::IdleEffects()
{
	animPtr = &idleSprite.anim;
}

void Player::CrouchingEffects()
{
	input.wantMoveRight = false;
	input.wantMoveLeft = false;
	animPtr = &crouchSprite.anim;
}

void Player::RunningEffects()
{
	if (runSfxTimer < SDL_GetTicks() - runSfxDelay) {	// IMPROVE: Gets out of tempo after a few plays.
		App->audio->PlayFx(App->audio->runSfx.id, 0);
		runSfxTimer = SDL_GetTicks();
	}
	animPtr = &runSprite.anim;
}

void Player::JumpingEffects()
{
	if (somersaultUsed == true) {
		animPtr = &somersaultSprite.anim;
	}
	else {
		animPtr = &jumpSprite.anim;
	}
}

void Player::FallingEffects()
{
	animPtr = &fallSprite.anim;
}

void Player::SlidingEffects()
{
	input.wantMoveRight = false;
	input.wantMoveLeft = false;
	acceleration.x = slideAcceleration;

	if (playedSlideSfx == false) {
		App->audio->PlayFx(App->audio->slideSfx.id, 0);
		playedSlideSfx = true;
	}
	animPtr = &slideSprite.anim;
}

void Player::HurtEffects()
{
	input.wantMoveUp = false;
	input.wantMoveDown = false;
	input.wantMoveRight = false;
	input.wantMoveLeft = false;
	if (playedHurtSfx == false) {
		App->audio->PlayFx(App->audio->hurtSfx.id, 0);
		playedHurtSfx = true;
	}

	if (dead == true) {
		DeadEffects();
	}
	else {
		animPtr = &hurtSprite.anim;
	}
}

void Player::DeadEffects() {
	if (fading == false && deadTimer < SDL_GetTicks() - deathDelay) {
		App->fade->FadeToBlack(App->scene, App->scene, fadeDelay);
		fading = true;
	}
	else if (fading == true && deadTimer < SDL_GetTicks() - deathDelay - fadeDelay * 1000 / 2) {
		deadSprite.anim.Reset();
		playedHurtSfx = false;
		dead = false;
		fading = false;
		position = respawnPosition;
		
		if (App->scene2->active)
		{
			App->scene2->CleanUp();
			App->fade->FadeToBlack(App->scene2, App->scene2);
			App->scene2->Start();
			App->entityManager->player->Start();
		}

		else if (App->scene->active)
		{
			App->scene->CleanUp();
			App->fade->FadeToBlack(App->scene, App->scene);
			App->scene->Start();
			App->entityManager->player->Start();
		}
	}
	else {
		input.wantMoveUp = false;
		input.wantMoveDown = false;
		input.wantMoveRight = false;
		input.wantMoveLeft = false;
		if (playedHurtSfx == false) {
			App->audio->PlayFx(App->audio->hurtSfx.id, 0);
			playedHurtSfx = true;
		}
		animPtr = &deadSprite.anim;
	}
}

// Move player position and decide/calculate other movement related factors
void Player::Move(float dt)
{
	if (godMode == true) {
		GodModeMovement(dt);
	}
	else {
		NormalMovement(dt);
	}

	// Max Speeds
	LimitSpeed();

	// New position
	position.x += speed.x * dt;
	position.y += speed.y * dt;

	posRect.x = (int)position.x;
	posRect.y = (int)position.y;
}

fPoint Player::GodModeMovement(float dt)
{
	if (input.wantMoveRight == true && input.wantMoveLeft == false) {
		position.x += 200 * dt;
	}
	if (input.wantMoveLeft == true && input.wantMoveRight == false) {
		position.x -= 200 * dt;
	}
	if (input.wantMoveUp == true && input.wantMoveDown == false) {
		position.y -= 200 * dt;
	}
	if (input.wantMoveDown == true && input.wantMoveUp == false) {
		position.y += 200 * dt;
	}

	return position;
}

fPoint Player::NormalMovement(float dt)
{
	if (input.wantMoveRight == true && input.wantMoveLeft == false) {
		speed.x += acceleration.x * dt;
	}
	else if (input.wantMoveLeft == true && input.wantMoveRight == false) {
		speed.x -= acceleration.x * dt;
	}
	else if (airborne == false) {	// Natural deacceleration when on ground
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
	if (airborne == true) {
		Fall(dt);
	}

	return speed;
}

fPoint Player::LimitSpeed()
{
	if (speed.x > 0)
		speed.x = MIN(speed.x, maxSpeed.x);
	else if (speed.x < 0)
		speed.x = MAX(speed.x, -maxSpeed.x);

	if (speed.y > 0)
		speed.y = MIN(speed.y, maxSpeed.y);
	else if (speed.y < 0)
		speed.y = MAX(speed.y, -maxSpeed.y);

	return speed;
}

void Player::UpdateHitbox()
{
	switch (status) {
	case state::IDLE:
		hitboxOffset = ReshapeCollider(idleSprite);
		break;
	case state::CROUCHING:
		hitboxOffset = ReshapeCollider(crouchSprite);
		break;
	case state::RUNNING:
		hitboxOffset = ReshapeCollider(runSprite);
		break;
	case state::JUMPING:
		if (somersaultUsed == true)
			hitboxOffset = ReshapeCollider(somersaultSprite);
		else
			hitboxOffset = ReshapeCollider(jumpSprite);
		break;
	case state::FALLING:
		hitboxOffset = ReshapeCollider(fallSprite);
		break;
	case state::SLIDING:
		hitboxOffset = ReshapeCollider(slideSprite);
		break;
	case state::HURT:
		hitboxOffset = ReshapeCollider(hurtSprite);
		break;
	default:
		break;
	}
}

SDL_Rect Player::ReshapeCollider(sprite_data sprite)
{
	hitbox->rect.x = (int)position.x + sprite.colliderOffset.x;
	hitbox->rect.y = (int)position.y + sprite.colliderOffset.y;
	hitbox->rect.w = sprite.colliderOffset.w;
	hitbox->rect.h = sprite.colliderOffset.h;
	return sprite.colliderOffset;
}