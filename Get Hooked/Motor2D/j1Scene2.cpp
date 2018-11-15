#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1FadeScene.h"
#include "j1Scene.h"
#include "j1Scene2.h"
#include "j1Player.h"	// @Carles
#include "j1Collision.h"
#include "j1EntityManager.h"

j1Scene2::j1Scene2() : j1Module()
{
	name.create("scene2");
}

// Destructor
j1Scene2::~j1Scene2()
{}

// Called before render is available
bool j1Scene2::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	cameraSpeed.x = config.child("cameraSpeed").attribute("x").as_float();
	cameraSpeed.y = config.child("cameraSpeed").attribute("y").as_float();

	map.create(config.child("map").attribute("name").as_string());
	playerPos.x = config.child("map").child("playerStartPos").attribute("x").as_float();
	playerPos.y = config.child("map").child("playerStartPos").attribute("y").as_float();
	return ret;
}

// Called before the first frame
bool j1Scene2::Start()
{
	if (App->scene->active == true) 
		active = false;

	if (active)
	{
		App->map->Load(map.GetString());	// SamAlert: Hardcoded map loading, should use a p2SString that copies a string from an xml file
		App->audio->PlayMusic(App->audio->musicMap1.GetString());	// SamAlert: Add map condition for playing music, this always calls the map 1 music
		App->audio->SetMusicVolume();
	}

	return true;
}

// Called each loop iteration
bool j1Scene2::PreUpdate()
{
	return true;
}

// Called each frame (logic)
bool j1Scene2::UpdateTick(float dt)
{
	AudioInput();

	if (App->entityManager->player->debugMode == true) {
		CameraInput(dt);

		/*if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)	//CHANGE/FIX: ChangeScene() doesn't work both sides, only 1 to 2
			ChangeScene();*/
	}

	return true;
}

// Called each loop iteration (graphic)
bool j1Scene2::Update()
{
	if (App->entityManager->player->CameraFree() == false) {
		LimitCameraPos(App->entityManager->player->GetPosition());	// Limit camera position
	}

	App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene2::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene2::CleanUp()
{
	LOG("Freeing scene");
	App->map->CleanUp();
	App->collision->CleanUp();
	App->tex->CleanUp();
	App->entityManager->player->CleanUp();

	return true;
}

void j1Scene2::CameraInput(float dt)	// @Carles
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += ceil(cameraSpeed.y * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= ceil(cameraSpeed.y * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += ceil(cameraSpeed.x * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= ceil(cameraSpeed.x * dt);
	}
}

void j1Scene2::AudioInput()	// @Carles
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

void j1Scene2::ChangeScene()
{
	
	App->scene->active = true;
	App->scene2->active = false;
	CleanUp();
	App->fade->FadeToBlack(App->scene2, App->scene);
	App->scene->Start();
	App->entityManager->player->Start();
	App->render->camera = { 0,0 };
	
}

SDL_Rect j1Scene2::LimitCameraPos(fPoint playerPos)
{
	if (App->render->camera.x < (int)-(playerPos.x * App->win->GetScale() - 350)/* && mapRightLimit is not crossed*/) {	//left	// Improve: Map limits & magic numbers
		App->render->camera.x = (int)-(playerPos.x * App->win->GetScale() - 350);
	}
	else if (App->render->camera.x > (int)-(playerPos.x * App->win->GetScale() - 500)/* && mapLeftLimit is not crossed*/) {	//right
		App->render->camera.x = (int)-(playerPos.x * App->win->GetScale() - 500);
	}

	if (App->render->camera.y < (int)-(playerPos.y * App->win->GetScale() - 300)/* && mapRightLimit is not crossed*/) {	//left
		App->render->camera.y = (int)-(playerPos.y * App->win->GetScale() - 300);
	}
	else if (App->render->camera.y > (int)-(playerPos.y * App->win->GetScale() - 400)/* && mapLeftLimit is not crossed*/) {	//right
		App->render->camera.y = (int)-(playerPos.y * App->win->GetScale() - 400);
	}

	return App->render->camera;
}