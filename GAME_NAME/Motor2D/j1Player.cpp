#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"		// Do we need it?
#include "j1Textures.h"		// Do we need it?
#include "j1Render.h"
#include "j1Audio.h"		// Do we need it?
#include "j1Player.h"
#include "p2Animation.h"	// @Carles

j1Player::j1Player()
{
	name.create("player");
}

j1Player::~j1Player()
{}

// Called before render is available
bool j1Player::Awake(pugi::xml_node&)
{
	bool ret = true;

	// Character Sprite Sheet
	graphics = nullptr;

	// Animation pointers
	playerAnim = nullptr;

	// Character stats
	life = 3;
	playerPos = { 100, 550 };
	playerSpeed = { 0, 0 };
	maxSpeed = { 1, 1 };

	// Character status flags
	dead = false;
	godmode = false;

	spriteWidth = 50;
	spriteHeight = 36;
	spriteMoveHorizontal = 50;
	spriteMoveVertical = 37;

	rectAnim.x = 50;
	rectAnim.y = 50;
	rectAnim.w = 50;
	rectAnim.h = 50;

	//Collider
	//Collider* playerHitbox = nullptr;

	// Character animations
	idle;
	//idle_sword;

	//draw_sword;
	//sheathe_sword;

	run;
	//slide;
	//slide_up;
	//crouch;

	jump;
	//somersault;
	fall;

	//attack_1;
	//attack_2;
	//attack_3;

	//air_attack_1;
	//air_attack_2;
	//air_attack_3_fall;
	//air_attack_3_end;

	//cast;
	//cast_loop;

	//use_item;

	//corner_grab;
	//corner_climb;
	//ladder_climb;
	//wall_slide;
	//wall_jump;

	hurt;
	die;

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;

	return ret;
}

// Called each loop iteration
bool j1Player::PreUpdate()
{
	bool ret = true;

	return ret;
}
bool j1Player::Update(float dt)
{
	bool ret = true;

	//App->render->Blit(graphics, position.x - 2, position.y - 3, &shipRect, 1.0f, false);

	playerInput();

	rectAnim.x = (int)playerPos.x;
	rectAnim.y = (int)playerPos.y;

	App->render->DrawQuad(rectAnim, 255, 0, 0, 100);

	return ret;
}
bool j1Player::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;
	
	LOG("Unloading player");

	//App->tex->UnLoad(graphics);
	//graphics = nullptr;

	return ret;
}

// Load Game State
bool j1Player::Load(pugi::xml_node& data)
{
	playerPos.x = data.child("position").attribute("x").as_int();
	playerPos.y = data.child("position").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = playerPos.x;
	pos.append_attribute("y") = playerPos.y;

	return true;
}

//------------------------------------------------

void j1Player::playerInput()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE
		||
		App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT
		&& App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (playerSpeed.x > 0.0f) {
			playerSpeed.x -= 0.01f;

			if (playerSpeed.x < 0.0f)
				playerSpeed.x = 0.0f;
		}
		else if (playerSpeed.x < 0.0f) {
			playerSpeed.x += 0.01f;

			if (playerSpeed.x > 0.0f)
				playerSpeed.x = 0.0f;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		playerSpeed.x += 0.01f;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		playerSpeed.x -= 0.01f;
	}
	
	if (playerSpeed.x > maxSpeed.x)
		playerSpeed.x = maxSpeed.x;

	else if (playerSpeed.x < -maxSpeed.x)
		playerSpeed.x = -maxSpeed.x;

	playerPos.x += playerSpeed.x;	// x movement
}