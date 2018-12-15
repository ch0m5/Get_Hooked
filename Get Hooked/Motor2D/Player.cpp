#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "p2Animation.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1FadeScene.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1UserInterface.h"	//CHANGE/FIX: REMOVE ALL DEBUG INPUT FROM PLAYER, PUT IT ON SCENE

#include "j1EntityManager.h"
#include "Player.h"
#include "Enemy.h"

Player::Player() : Creature(entity_type::PLAYER)
{
	name.create("player");
}

Player::~Player()
{}

// Called before render is available
bool Player::Awake(pugi::xml_node& entities)
{
	LOG("Loading Player Data");
	bool ret = true;

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

	runSfxDelay = entities.child("audio").child("runSfxDelay").attribute("miliseconds").as_int();
	playedSlideSfx = entities.child("audio").child("slideSfx").attribute("played").as_bool();
	playedHurtSfx = entities.child("audio").child("hurtSfx").attribute("played").as_bool();

	//First start definition
	life = maxLife;

	return ret;
}

// Called before the first frame
bool Player::Start()
{
	bool ret = true;

	startLife = life;
	dead = false;
	speed = { 0, 0 };
	acceleration.x = normalAcceleration;
	hitboxOffset = idleSprite.colliderOffset;
	status = player_state::IDLE;

	position = lastGroundPosition = spawnPosition = App->scene->playerStart;

	graphics = App->tex->Load(textureName.GetString());
	
	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_PLAYER, this);
	hitboxOffset = hitbox->rect;

	return ret;
}

bool Player::LoadStart()
{
	bool ret = true;

	graphics = App->tex->Load(textureName.GetString());

	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_PLAYER, this);
	hitboxOffset = hitbox->rect;

	return ret;
}

// Called each loop iteration
bool Player::PreUpdate()
{
	BROFILER_CATEGORY("Entity Player PreUpdate", Profiler::Color::LightCyan);

	bool ret = true;

	if (App->scene->debugMode == true) {
		DebugInput();
	}

	CheckInput();		// Check player input
	CheckMovement();	// Check player current movement

	return ret;
}

//input and logic
bool Player::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Entity Player UpdateTick", Profiler::Color::Cyan);

	bool ret = true;

	CheckState();	// Check player state
	ApplyState();	// Add state effects like movement restrictions, animation and sounds
	Move(dt);		// Move player position and calculate other movement related factors

	if (hitbox != nullptr)
		UpdateHitbox();	// Transform player collider depending on new position and state

	animRect = animPtr->AdvanceAnimation(dt);

	return ret;
}

bool Player::Update()
{
	BROFILER_CATEGORY("Entity Player Update", Profiler::Color::DarkCyan);

	bool ret = true;

	Draw(&animRect);

	return ret;
}

// Called before quitting
bool Player::CleanUp()
{
	bool ret = true;

	LOG("Unloading player");

	App->tex->UnLoad(graphics);
	graphics = nullptr;

	if (hitbox != nullptr) {
		hitbox->to_delete = true;
		hitbox = nullptr;
	}

	if (attackCollider != nullptr) {
		attackCollider->to_delete = true;
		attackCollider = nullptr;
	}

	return ret;
}

// Called when colliding
collision_type Player::OnCollision(Collider* c1, Collider* c2)	//IMPROVE: REWORK ON COLLISION SYSTEM TO TAKE INTO ACCOUNT REVIOUS POSITION
{
	collision_type ret = collision_type::NONE;

	if (godMode == false) {
		if (c1->GetType() == collider_type::COLLIDER_PLAYER && c2->GetType() == collider_type::COLLIDER_WALL) {
			ret = WallCollision(c1, c2);
		}
		else if (damageCollision == true) {
			if (c1->GetType() == collider_type::COLLIDER_PLAYER && c2->GetType() == collider_type::COLLIDER_ENEMY) {

				if (c2->callback->CanDamageCollide() == true && status != player_state::HURT) {
					Hurt();
					status = player_state::HURT;
					ret = collision_type::UNDEFINED;
				}
			}
			else if (c1->GetType() == collider_type::COLLIDER_PLAYER && c2->GetType() == collider_type::COLLIDER_ENEMY_ATTACK) {

				if (status != player_state::HURT) {
					Hurt();
					status = player_state::HURT;
					ret = collision_type::UNDEFINED;
				}
			}
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
			c1->rect.y--;		//IMPROVE: On high framerates, player doesn't go down immediately after colliding, which it should
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
			c1->rect.x++;	//IMPROVE: Loading while touching a lateral collider makes the player not Load() position x & y correctly
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

// Load Game State	//CHANGE/FIX: Should be able to save and load from and to different scenes!
bool Player::Load(pugi::xml_node& data)
{
	if (attackCollider != nullptr) {	//IMPROVE: Maybe put this somewhere else, less dirty?
		attackCollider->to_delete = true;
		attackCollider = nullptr;
		attackColliderCreated = false;
	}

	position.x = data.child("position").attribute("x").as_float();
	position.y = data.child("position").attribute("y").as_float();
	lastGroundPosition.x = data.child("lastGround").attribute("x").as_float();
	lastGroundPosition.y = data.child("lastGround").attribute("y").as_float();

	speed.x = data.child("speed").attribute("x").as_float();
	speed.y = data.child("speed").attribute("y").as_float();
	life = (ushort)data.child("life").attribute("value").as_uint();
	acceleration.x = data.child("acceleration").attribute("type").as_float();
	status = (player_state)data.child("state").attribute("current").as_uint();
	airborne = data.child("airborne").attribute("value").as_bool();
	lookingRight = data.child("looking").attribute("right").as_bool();
	somersaultUsed = data.child("somersault").attribute("used").as_bool();
	dead = data.child("dead").attribute("value").as_bool();
	hurtTimer = data.child("hurtTimer").attribute("miliseconds").as_uint();
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

	tmpNode = data.append_child("hurtTimer");
	tmpNode.append_attribute("miliseconds") = hurtTimer;

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
	maxSpeed = { config.child("maxSpeed").attribute("x").as_float(), config.child("maxSpeed").attribute("y").as_float() };
	hurtSpeed = { config.child("hurtSpeed").attribute("x").as_float(), config.child("hurtSpeed").attribute("y").as_float() };
	normalAcceleration = config.child("accelerations").attribute("x").as_float();
	acceleration.y = config.child("accelerations").attribute("y").as_float();
	slideAcceleration = config.child("accelerations").attribute("slide").as_float();
	gravity = config.child("accelerations").attribute("gravity").as_float();

	// Character status flags and directly related data
	airborne = config.child("airborne").attribute("value").as_bool();
	lookingRight = config.child("looking").attribute("right").as_bool();
	somersaultUsed = config.child("somersault").attribute("used").as_bool();
	hurtDelay = config.child("hurtDelay").attribute("miliseconds").as_uint();
	deathDelay = config.child("deathDelay").attribute("miliseconds").as_uint();
	godMode = config.child("godMode").attribute("value").as_bool();
	freeCamera = config.child("freeCamera").attribute("value").as_bool();
	activationRadius = { config.child("processingRadius").attribute("x").as_int(), config.child("processingRadius").attribute("y").as_int() };
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

	ImportSpriteData("attack_1", &attack1Sprite, first_sprite);
	ImportAttackData("attack_1", &attack1Data, first_sprite);
}

void Player::ImportAttackData(const char* spriteName, attack_data* attack, pugi::xml_node& first_sprite)
{
	attack->offsetRight.x = first_sprite.child(spriteName).child("attackRightOffset").attribute("x").as_int();
	attack->offsetRight.y = first_sprite.child(spriteName).child("attackRightOffset").attribute("y").as_int();
	attack->offsetRight.w = first_sprite.child(spriteName).child("attackRightOffset").attribute("w").as_int();
	attack->offsetRight.h = first_sprite.child(spriteName).child("attackRightOffset").attribute("h").as_int();

	attack->offsetLeft.x = first_sprite.child(spriteName).child("attackLeftOffset").attribute("x").as_int();
	attack->offsetLeft.y = first_sprite.child(spriteName).child("attackLeftOffset").attribute("y").as_int();
	attack->offsetLeft.w = first_sprite.child(spriteName).child("attackLeftOffset").attribute("w").as_int();
	attack->offsetLeft.h = first_sprite.child(spriteName).child("attackLeftOffset").attribute("h").as_int();

	attack->startAttackFrame = first_sprite.child(spriteName).child("attackFrames").attribute("start").as_uint();
	attack->finishAttackFrame = first_sprite.child(spriteName).child("attackFrames").attribute("finish").as_uint();
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
	attack1Sprite.anim.AllocAnimation({ attack1Sprite.sheetPosition.x * spriteSize.x, attack1Sprite.sheetPosition.y * spriteSize.y }, spriteSize, attack1Sprite.numFrames);
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
	if (lookingRight) {
		speed.x = -hurtSpeed.x;
	}
	else {
		speed.x = hurtSpeed.x;
	}

	speed.y = -hurtSpeed.y;
	
	airborne = true;

	damageCollision = false;
	hurtTimer = SDL_GetTicks();

	if (--life < 1) {
		dead = true;
		deadTimer = SDL_GetTicks();
		deadSprite.anim.Reset();
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

	attack1Sprite.anim.Reset();

	if (attackCollider != nullptr) {
		attackCollider->to_delete = true;
		attackCollider = nullptr;
		attackColliderCreated = false;
	}
}

void Player::DeathByPit()
{
	life = 0;
	dead = true;
	deadSprite.anim.Reset();
	deadTimer = SDL_GetTicks();
	mustReset = true;
	playedHurtSfx = false;
}

//Check debug input
void Player::DebugInput()	//IMPROVE: Should the whole "debug" be in scene?
{
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && freeCamera == false) {	// Free camera
		freeCamera = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && freeCamera == true) {
		freeCamera = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && status != player_state::HURT && godMode == false) {	// Hurt player
		Hurt();
		status = player_state::HURT;
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && IsDead() == false && godMode == false) {	// GodMode
		godMode = true;
		speed = { 0, 0 };
		status = player_state::FALLING;
		mustReset = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && godMode == true) {
		godMode = false;
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

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {	// Attack
		wantAttack = true;
	}
	else {
		wantAttack = false;
	}

	//if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && debugMode == false) {	// Activate debug mode input	//IMPROVE: Add debug key that enables/disables all debug functionalities
	//	debugMode = true;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && debugMode == true) {	// Improve: Turning off debugMode shouldn't the change other flag values, just evaluate debugMode itself when their values are checked
	//	debugMode = false;
	//	godMode = false;
	//	freeCamera = false;
	//	App->collision->mustDebugDraw = false;
	//	App->ui->mustDebugDraw = false;
	//	App->win->scale = App->win->origScale;
	//}
}

// Check player state
void Player::CheckState() {	// For each state, check possible new states based on other parameters
	if (godMode == true) {
		airborne = true;

		if (position.y > 800) {		//CHANGE/FIX: Hardcoded fallen pit
			position = lastGroundPosition;
		}
	}
	else {
		if (App->collision->CheckGroundCollision(hitbox) == false) {
			airborne = true;
		}
		else if (hurtTimer < SDL_GetTicks() - hurtDelay) {	//CHANGE/FIX: Not perfect, it's checked on every update
			damageCollision = true;
		}

		switch (status) {
		case player_state::IDLE:
			status = IdleMoveCheck();
			break;
		case player_state::CROUCHING:
			status = CrouchingMoveCheck();
			break;
		case player_state::RUNNING:
			status = RunningMoveCheck();
			break;
		case player_state::JUMPING:
			status = JumpingMoveCheck();
			break;
		case player_state::FALLING:
			status = FallingMoveCheck();
			break;
		case player_state::SLIDING:
			status = SlidingMoveCheck();
			break;
		case player_state::HURT:
			status = HurtMoveCheck();
			break;
		case player_state::ATTACKING:
			status = AttackMoveCheck();
			break;
		}
	}
}

player_state Player::IdleMoveCheck()
{
	player_state ret = player_state::IDLE;

	if (airborne) {
		ret = player_state::FALLING;
	}
	else if (input.wantMoveRight == true && input.wantMoveLeft == false || input.wantMoveLeft == true && input.wantMoveRight == false) {
		ret = player_state::RUNNING;
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = player_state::JUMPING;
	}
	else if (input.wantMoveDown == true) {
		ret = player_state::CROUCHING;
	}
	else if (wantAttack == true) {
		attackFinalFrame = attack1Sprite.numFrames - 1;
		ret = player_state::ATTACKING;
	}

	return ret;
}

player_state Player::CrouchingMoveCheck()
{
	player_state ret = player_state::CROUCHING;

	if (airborne) {
		ret = player_state::FALLING;
	}
	else if (input.wantMoveDown == false) {
		if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true)
			ret = player_state::RUNNING;
		else {
			ret = player_state::IDLE;
		}
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = player_state::JUMPING;
	}
	else if (wantAttack == true) {
		attackFinalFrame = attack1Sprite.numFrames - 1;
		ret = player_state::ATTACKING;
	}

	return ret;
}

player_state Player::RunningMoveCheck()
{
	player_state ret = player_state::RUNNING;

	if (airborne) {
		ret = player_state::FALLING;
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = player_state::JUMPING;
	}
	else if (input.wantMoveDown == true) {
		ret = player_state::SLIDING;
	}
	else if (movement.movingLeft == false && movement.movingRight == false) {
		if (input.wantMoveRight == false && input.wantMoveLeft == false || input.wantMoveRight == true && input.wantMoveLeft == true) {
			ret = player_state::IDLE;
		}
	}
	else if (wantAttack == true) {
		attackFinalFrame = attack1Sprite.numFrames - 1;
		ret = player_state::ATTACKING;
	}

	return ret;
}

player_state Player::JumpingMoveCheck()
{
	player_state ret = player_state::JUMPING;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		Jump();
		somersaultSprite.anim.Reset();
		somersaultUsed = true;
	}
	else if (movement.movingDown == true) {
		ret = player_state::FALLING;
	}

	if (ret != player_state::JUMPING)
		jumpSprite.anim.Reset();

	return ret;
}

player_state Player::FallingMoveCheck()
{
	player_state ret = player_state::FALLING;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && somersaultUsed == false) {
		somersaultSprite.anim.Reset();
		Jump();
		ret = player_state::JUMPING;
		somersaultUsed = true;
	}
	else if (!airborne) {
		Land();

		if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true) {
			if (input.wantMoveDown == true) {
				ret = player_state::SLIDING;
			}
			else {
				ret = player_state::RUNNING;
			}
		}
		else if (input.wantMoveDown == true) {
			ret = player_state::CROUCHING;
		}
		else {
			ret = player_state::IDLE;
		}		
	}
	if (position.y > 800 && !dead) {	//CHANGE/FIX: Hardcoded fallen pit
		DeathByPit();
		ret = player_state::HURT;
	}

	return ret;
}

player_state Player::SlidingMoveCheck()
{
	player_state ret = player_state::SLIDING;

	if (airborne) {
		ret = player_state::FALLING;
	}
	else if (input.wantMoveUp == true) {
		Jump();
		ret = player_state::JUMPING;
	}
	else if (input.wantMoveDown == false) {

		if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true) {
			ret = player_state::RUNNING;
		}
		else {
			ret = player_state::IDLE;
		}
	}
	else if (movement.movingLeft == false && movement.movingRight == false) {

		if (input.wantMoveDown == true) {
			ret = player_state::CROUCHING;
		}
		else {
			ret = player_state::IDLE;
		}
	}

	if (ret != player_state::SLIDING)
		StandUp();

	return ret;
}

player_state Player::HurtMoveCheck()
{
	player_state ret = player_state::HURT;

	if (!airborne) {		//SamAlert: Hardcoded values, this condition should be "if feet collision", 
		Land();

		if (dead == false) {
			if (input.wantMoveRight == true || input.wantMoveLeft == true || movement.movingRight == true || movement.movingLeft == true) {
				if (input.wantMoveDown == true) {
					ret = player_state::SLIDING;
				}
				else {
					ret = player_state::RUNNING;
				}
			}
			else if (input.wantMoveDown == true) {
				ret = player_state::CROUCHING;
			}
			else {
				ret = player_state::IDLE;
			}
		}
	}
	if (position.y > 800 && !dead) {	//CHANGE/FIX: Hardcoded fallen pit
		DeathByPit();
		ret = player_state::HURT;
	}

	if (ret != player_state::HURT && hurtTimer < SDL_GetTicks() - hurtDelay) {
		damageCollision = true;
	}

	return ret;
}

player_state Player::AttackMoveCheck()
{
	player_state ret = player_state::ATTACKING;

	if (airborne) {
		ret = player_state::FALLING;
	}
	else if (animPtr->GetCurrentFrame() == attackFinalFrame) {
		if (input.wantMoveUp == true) {
			Jump();
			ret = player_state::JUMPING;
		}
		else if (input.wantMoveDown == true) {
			ret = player_state::CROUCHING;
		}
		else if (input.wantMoveRight == true && input.wantMoveLeft == false || input.wantMoveLeft == true && input.wantMoveRight == false) {
			ret = player_state::RUNNING;
		}
		else {
			ret = player_state::IDLE;
		}
	}

	if (ret != player_state::ATTACKING) {
		attack1Sprite.anim.Reset();

		if (attackCollider != nullptr) {
			attackCollider->to_delete = true;
			attackCollider = nullptr;
			attackColliderCreated = false;
		}
	}
		
	return ret;
}

// Add state effects like movement restrictions, animation and sounds
void Player::ApplyState()
{
	if (!(status == player_state::SLIDING || status == player_state::HURT || status == player_state::ATTACKING)) {
		CheckOrientation();
	}

	if (mustReset) {
		PlayerReset();
		mustReset = false;
	}

	if (!airborne) {
		lastGroundPosition = position;
	}

	switch (status) {
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
	case player_state::ATTACKING:
		AttackEffects();
		break;
	}
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
	if (App->fade->GetStep() == fade_step::NONE && deadTimer < SDL_GetTicks() - deathDelay) {
		App->fade->FadeToBlack(App->fade->GetDelay(), fade_type::RESTART_LEVEL);	//CHANGE/FIX: Hardcoded values
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

void Player::AttackEffects()
{
	input.wantMoveUp = false;
	input.wantMoveDown = false;
	input.wantMoveRight = false;
	input.wantMoveLeft = false;

	if (animPtr->GetCurrentFrame() == attack1Data.startAttackFrame && attackColliderCreated == false) {
		attackCollider = CreateAttackCollider(attack1Data);
		attackColliderCreated = true;
	}
	else if (animPtr->GetCurrentFrame() == attack1Data.finishAttackFrame && attackColliderCreated == true) {
		if (attackCollider != nullptr) {
			attackCollider->to_delete = true;
			attackCollider = nullptr;
		}

		attackColliderCreated = false;
	}

	animPtr = &attack1Sprite.anim;
}

// Move player position and decide/calculate other movement related factors
void Player::Move(float dt)
{
	if (godMode) {
		GodModeMovement(dt);
	}
	else {
		NormalMovement(dt);
	}

	// Max Speeds
	speed = LimitSpeed();

	// New position
	position.x += speed.x * dt;
	position.y += speed.y * dt;

	// Center position
	centerPosition.x = position.x + animRect.w / 2;
	centerPosition.y = position.y + animRect.h / 2;
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
	else if (!airborne) {	// Natural deacceleration when on ground
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

	return speed;
}

void Player::UpdateHitbox()
{
	switch (status) {
	case player_state::IDLE:
		hitboxOffset = ReshapeCollider(idleSprite);
		break;
	case player_state::CROUCHING:
		hitboxOffset = ReshapeCollider(crouchSprite);
		break;
	case player_state::RUNNING:
		hitboxOffset = ReshapeCollider(runSprite);
		break;
	case player_state::JUMPING:
		if (somersaultUsed == true)
			hitboxOffset = ReshapeCollider(somersaultSprite);
		else
			hitboxOffset = ReshapeCollider(jumpSprite);
		break;
	case player_state::FALLING:
		hitboxOffset = ReshapeCollider(fallSprite);
		break;
	case player_state::SLIDING:
		hitboxOffset = ReshapeCollider(slideSprite);
		break;
	case player_state::HURT:
		hitboxOffset = ReshapeCollider(hurtSprite);
		break;
	case player_state::ATTACKING:
		hitboxOffset = ReshapeCollider(attack1Sprite);

		if (attackColliderCreated)
			attackOffset = ReshapeAttackCollider(attack1Data);
		break;
	default:
		break;
	}
}

Collider* Player::CreateAttackCollider(attack_data attack)
{
	SDL_Rect tmpRect;

	if (lookingRight) {
		tmpRect.x = (int)position.x + attack.offsetRight.x;
		tmpRect.y = (int)position.y + attack.offsetRight.y;
		tmpRect.w = attack.offsetRight.w;
		tmpRect.h = attack.offsetRight.h;
	}
	else {
		tmpRect.x = (int)position.x + attack.offsetLeft.x;
		tmpRect.y = (int)position.y + attack.offsetLeft.y;
		tmpRect.w = attack.offsetLeft.w;
		tmpRect.h = attack.offsetLeft.h;
	}

	return App->collision->AddCollider(tmpRect, COLLIDER_PLAYER_ATTACK, nullptr);
}

SDL_Rect Player::ReshapeAttackCollider(attack_data attack)
{
	SDL_Rect tmpOffset;

	if (lookingRight) {
		attackCollider->rect.x = (int)position.x + attack.offsetRight.x;
		attackCollider->rect.y = (int)position.y + attack.offsetRight.y;
		attackCollider->rect.w = attack.offsetRight.w;
		attackCollider->rect.h = attack.offsetRight.h;

		tmpOffset = attack.offsetRight;
	}
	else {
		attackCollider->rect.x = (int)position.x + attack.offsetLeft.x;
		attackCollider->rect.y = (int)position.y + attack.offsetLeft.y;
		attackCollider->rect.w = attack.offsetLeft.w;
		attackCollider->rect.h = attack.offsetLeft.h;

		tmpOffset = attack.offsetLeft;
	}

	return tmpOffset;
}