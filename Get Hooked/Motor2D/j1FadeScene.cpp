#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FadeScene.h"
#include "j1Render.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"


j1FadeScene::j1FadeScene()
{
	name.create("window");
}

j1FadeScene::~j1FadeScene()
{}

// Called before render is available
bool j1FadeScene::Awake(pugi::xml_node& config)
{
	int screenSize = config.child("resolution").attribute("scale").as_int();
	screen = { 0, 0, config.child("resolution").attribute("width").as_int() * screenSize, config.child("resolution").attribute("height").as_int() * screenSize };

	return true;
}

// Called before the first frame, loads assets
bool j1FadeScene::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeScene::Update()
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			// CHANGE/FIX
			// SamAlert: Here it previosuly enable/disable modules, what it does now is up to you Sam: Load game, change map, start map from beginning...
			//fade_out->Disable();
			//fade_in->Enable();
			//fade_out = nullptr;
			//fade_in = nullptr;

			// ---
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point change one scene for another
bool j1FadeScene::FadeToBlack(j1Module* map_off, j1Module* map_on, float time)	// Before: bool ModuleFadeToBlack::FadeToBlack(Module* module_off, Module* module_on, float time)
{
	bool ret = false;

	fade_out = map_off;
	fade_in = map_on;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		ret = true;
	}

	return ret;
}