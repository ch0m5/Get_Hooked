#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1FadeScene.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"


j1FadeScene::j1FadeScene() : j1Module()
{
	name.create("fade");
}

j1FadeScene::~j1FadeScene()
{}

// Called before render is available
bool j1FadeScene::Awake(pugi::xml_node& config)
{
	delay = config.child("duration").attribute("seconds").as_float();

	return true;
}

// Called before the first frame, loads assets
bool j1FadeScene::Start()
{
	LOG("Preparing Fade Screen");

	int screenSize = App->win->GetScale();
	screen = App->win->GetWindowSize();

	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeScene::Update()
{
	if (step == fade_step::NONE)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (step)
	{
	case fade_step::FADING:
	{
		if (now >= total_time) {
			step = fade_step::FULLY_FADED;
		}	

	} break;
	case fade_step::FULLY_FADED:
	{
		total_time += total_time;
		start_time = SDL_GetTicks();
		step = fade_step::UNFADING;
	} break;
	case fade_step::UNFADING:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			step = fade_step::NONE;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point change one scene for another
bool j1FadeScene::FadeToBlack(float time, fade_type type)
{
	bool ret = false;

	this->type = type;

	if (step == fade_step::NONE)
	{
		step = fade_step::FADING;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		ret = true;
	}

	return ret;
}