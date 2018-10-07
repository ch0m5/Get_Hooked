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
	// Character Sprite Sheet
	graphics = nullptr;

	// Animation pointers
	character_animation = nullptr;

	// Character stats
	life = 3;
	position = { 100, 550 };
	speed = { 0, 0 };

	// Character status flags
	dead = false;
	godmode = false;

	sprite_width = 50;
	sprite_height = 36;
	sprite_move_horizontal = 50;
	sprite_move_vertical = 37;

	animation_rect.x = 50;
	animation_rect.y = 50;
	animation_rect.w = 50;
	animation_rect.h = 50;

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
}

j1Player::~j1Player()
{

}

// Called before render is available
bool j1Player::Awake(pugi::xml_node&)
{

	bool ret = true;

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

	App->render->DrawQuad(animation_rect, 255, 0, 0, 100);

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

// Save and Load
//bool j1Player::Load(pugi::xml_node&) {
//
//	bool ret = true;
//
//	return ret;
//}
//bool j1Player::Save(pugi::xml_node&) const {
//
//	bool ret = true;
//
//	return ret;
//}