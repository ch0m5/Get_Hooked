#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Player.h"	// @Carles

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	cameraSpeed.x = config.child("cameraSpeed").attribute("x").as_float();
	cameraSpeed.y = config.child("cameraSpeed").attribute("y").as_float();

	title.create(config.child("title").child_value());	// @Carles
	App->win->SetTitle(title.GetString());

	current.create(config.child("map_one").attribute("name").as_string());

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->map->Load(current.GetString());	// SamAlert: Hardcoded map loading, should use a p2SString that copies a string from an xml file
	App->audio->PlayMusic(App->audio->musicMap1.GetString());	// SamAlert: Add map condition for playing music, this always calls the map 1 music
	App->audio->SetMusicVolume();

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

p2SString j1Scene::GetCurrent()
{
	return current;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	AudioInput();

	if (App->player->debugMode == true) {
		CameraInput();
		SetDebugTitle();
	}
	else {
		SetOrigTitle();
	}

	//App->render->Blit(img, 0, 0);
	App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::CameraInput()	// @Carles
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += cameraSpeed.y;

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= cameraSpeed.y;

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += cameraSpeed.x;

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= cameraSpeed.x;
	}
}

void j1Scene::AudioInput()	// @Carles
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && App->audio->masterVolume < 100)
			App->audio->masterVolume++;

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && App->audio->masterVolume > 0)
			App->audio->masterVolume--;

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && App->audio->musicVolume < 100)
			App->audio->musicVolume++;

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && App->audio->musicVolume > 0)
			App->audio->musicVolume--;

		App->audio->SetMusicVolume();
		App->audio->SetSfxVolume();
	}
}

void j1Scene::SetDebugTitle()	// @Carles
{
	p2SString title("%s (Position :%dx%d / Speed:%dx%d / Map:%dx%d / Tiles:%dx%d / Tilesets:%d)",
		title.GetString(),
		(int)App->player->GetPosition().x,
		(int)App->player->GetPosition().y,
		(int)App->player->GetSpeed().x,
		(int)App->player->GetSpeed().y,
		App->map->data.width, App->map->data.height,
		App->map->data.tile_width, App->map->data.tile_height,
		App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());
}

void j1Scene::SetOrigTitle()	// @Carles
{
	p2SString title("%s", title.GetString());

	App->win->SetTitle(title.GetString());
}