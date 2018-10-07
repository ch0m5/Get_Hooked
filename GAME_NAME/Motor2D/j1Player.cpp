#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"		// Do we need it?
#include "j1Textures.h"		// Do we need it?
#include "j1Audio.h"		// Do we need it?
#include "j1Player.h"
#include "p2Animation.h"	// @Carles

j1Player::j1Player() {

}

j1Player::~j1Player() {

}

// Called before render is available
bool j1Player::Awake(pugi::xml_node&) {

	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Player::Start() {

	bool ret = true;

	return ret;
}

// Called each loop iteration
bool j1Player::PreUpdate() {

	bool ret = true;

	return ret;
}
bool j1Player::Update(float dt) {

	bool ret = true;

	return ret;
}
bool j1Player::PostUpdate() {

	bool ret = true;

	return ret;
}
// Called before quitting
bool j1Player::CleanUp() {

	bool ret = true;

	return ret;
}

// Save and Load
bool j1Player::Load(pugi::xml_node&) {

	bool ret = true;

	return ret;
}
bool j1Player::Save(pugi::xml_node&) const {

	bool ret = true;

	return ret;
}