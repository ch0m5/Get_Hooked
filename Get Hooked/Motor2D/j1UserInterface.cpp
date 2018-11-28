#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1UserInterface.h"

j1UserInterface::j1UserInterface() : j1Module()
{
	name.create("userInterface");
}

// Destructor
j1UserInterface::~j1UserInterface()
{}

// Called before render is available
bool j1UserInterface::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1UserInterface::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1UserInterface::PreUpdate()
{
	return true;
}

// Called after all Updates
bool j1UserInterface::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1UserInterface::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

// const getter for atlas
const SDL_Texture* j1UserInterface::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

