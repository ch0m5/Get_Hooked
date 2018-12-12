#include "Brofiler/Brofiler.h"
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
#include "j1Collision.h"
#include "j1Timer.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "j1Fonts.h"
#include "j1UserInterface.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"

//Button actions	//CHANGE/FIX: Locate somewhere else, having this laying around is very dirty
void CloseGame()
{
	App->mustShutDown = true;
}

// ------------------------------------------------------------------------------

//Constructor
j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	loading = true;
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

	scene = (scene_type)config.child("scene").attribute("start").as_int();

	pugi::xml_node item;
	for (item = config.child("maps").first_child(); item != NULL; item = item.next_sibling()) {
		maps.add(item.child_value());
	}

	return ret;
}

// Called before the first frame
bool j1Scene::Start()	//TODO: Create enemies in their respective positions using XML
{
	bool ret = true;

	switch (scene) {	//CHANGE/FIX: Make function?
	case scene_type::LEVEL_1:
		App->map->Load(maps.At(0)->data.GetString());
		App->audio->PlayMusic(App->audio->musicMap1.GetString());
		playerStart = { 608, 250 };		//start = App->map->data.checkpoints.start->data;	//CHANGE/FIX: Get points close to the ground
		playerFinish = { 500, 500 };	//finish = App->map->data.checkpoints.end->data;
		break;
	case scene_type::LEVEL_2:
		App->map->Load(maps.At(1)->data.GetString());
		App->audio->PlayMusic(App->audio->musicMap2.GetString());
		playerStart = { 720, -400 };	//start = App->map->data.checkpoints.start->data;
		playerFinish = { 500, 500 };	//finish = App->map->data.checkpoints.end->data;
		break;
	/*case scene_type::LEVEL_3:
		App->map->Load(mapList.At(2)->data.name.GetString());
		break;
	case scene_type::LEVEL_4:
		break;
	case scene_type::LEVEL_5:
		break;*/
	default:
		ret = false;
	}

	App->audio->SetMusicVolume();

	//CHANGE/FIX: TEST
	//App->ui->CreateImage({ 500, 500 }, NULL, NULL, &Text("patata"), NULL);
	SDL_Rect pop = { 5, 112, 220, 63 };
	SDL_Rect arr[4] = { { 5, 112, 224, 63 }, { 5, 112, 224, 63 }, { 414, 170, 224, 63 }, { 648, 171, 224, 63 } };
	
	Image* parentImage = App->ui->CreateImage({ 200, 50 }, { 5, 112, 220, 63 });
	App->ui->CreateText(DEFAULT_POINT, "patatatatata", DEFAULT_COLOR, NULL, (UIElement*)parentImage);
	Button* parentButton = App->ui->CreateButton(&CloseGame, { 200, 150 }, arr);
	App->ui->CreateText(DEFAULT_POINT, "patatathesequel", DEFAULT_COLOR, NULL, (UIElement*)parentButton);

	//p2List<Image*> imageList;
	//imageList.add(&Image(image_type::IMAGE, { 100, 50 }, &pop));
	//imageList.add(&Image(image_type::IMAGE, { 200, 100 }, &pop, NULL, new Text("patata")));
	//imageList.add(&Text("patata2", { 250, 150 }));
	//App->ui->CreateWindowBox({ 50, 20 }, imageList, &pop, NULL);

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()	//IMPROVE: Full debug input here?
{
	BROFILER_CATEGORY("Module Scene PreUpdate", Profiler::Color::DarkOrange);

	return true;
}

// Called each frame (logic)
bool j1Scene::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Module Scene UpdateTick", Profiler::Color::OrangeRed);

	AudioInput();

	if (App->entityManager->player->debugMode == true) {
		CameraInput(dt);

		if (App->fade->GetStep() == fade_step::NONE) {
			if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
				App->fade->FadeToBlack(App->fade->GetDelay(), fade_type::RESTART_GAME);
			}

			if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
				App->fade->FadeToBlack(App->fade->GetDelay(), fade_type::RESTART_LEVEL);
			}

			if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
				App->fade->FadeToBlack(App->fade->GetDelay(), fade_type::NEXT_LEVEL);
			}
		}
	}

	return true;
}

// Called each loop iteration (graphic)
bool j1Scene::Update()
{
	BROFILER_CATEGORY("Module Scene Update", Profiler::Color::Orange);

	if (App->entityManager->player->CameraFree() == false) {
		LimitCameraPos(App->entityManager->player->GetPosition());	// Limit camera position
	}

	// All blitting in order
	App->map->Draw();
	App->entityManager->Draw();
	App->collision->Draw();
	App->ui->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("Module Scene PostUpdate", Profiler::Color::DarkOrange);

	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}															// @Carles
	else if (App->fade->GetStep() == fade_step::FULLY_FADED) {	// When game is fully faded, start game load and disable all entities for the next frame, then enable them.
		App->entityManager->active = false;						// This prevents the entities to use the massive dt value of the after-load frame on their calculations.

		switch (App->fade->GetType()) {
		case fade_type::NEXT_LEVEL:
			NextLevel();
			break;
		case fade_type::RESTART_LEVEL:
			RestartLevel();
			break;
		case fade_type::RESTART_GAME:
			ChangeScene(scene_type::LEVEL_1);
			break;
		default:
			break;
		}

		App->fade->ResetType();
		loading = true;
	}
	else if (loading) {
		App->entityManager->active = true;
		loading = false;
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()	//CHANGE/FIX: HEAVY MEMORY LEAKS WHEN CHANGING SCENE, FIRST SUSPECT: COLLIDER LIST, MAPS, AND TEXTURES
{
	LOG("Freeing scene");
	App->map->CleanUp();
	App->collision->CleanUp();

	return true;
}

void j1Scene::CameraInput(float dt)	// @Carles
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += (int)ceil(cameraSpeed.y * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= (int)ceil(cameraSpeed.y * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += (int)ceil(cameraSpeed.x * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= (int)ceil(cameraSpeed.x * dt);
	}
}

void j1Scene::AudioInput()	// @Carles
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && App->audio->masterVolume < 100)
			App->audio->masterVolume++;

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && App->audio->masterVolume > 0)
			App->audio->masterVolume--;

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && App->audio->musicVolume > 0)
			App->audio->musicVolume--;

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && App->audio->musicVolume < 100)
			App->audio->musicVolume++;

		App->audio->SetMusicVolume();
		App->audio->SetSfxVolume();
	}
}

void j1Scene::NextLevel()
{
	scene = (scene_type)((int)scene + 1);

	if (scene == scene_type::MAX_SCENES) {	//Restart Game
		ChangeScene(scene_type::LEVEL_1);
		App->entityManager->player->LifeToMax();
	}
	else {
		ChangeScene(scene);
	}
}

void j1Scene::RestartLevel()	//Restart enemies and values, nothing else (no full map reloading)
{
	App->entityManager->player->DeadReset();
	//App->entityManager->ReloadEnemies();
	App->entityManager->player->CleanUp();
	App->entityManager->player->LifeToStart();
	App->entityManager->player->Start();
}

void j1Scene::ChangeScene(scene_type scene)
{
	this->scene = scene;

	CleanUp();
	App->entityManager->player->CleanUp();
	Start();
	App->entityManager->player->Start();
}

SDL_Rect j1Scene::LimitCameraPos(fPoint playerPos)
{
	if (App->render->camera.x < (int)-(playerPos.x * App->win->GetScale() - 350)/* && mapRightLimit is not crossed*/) {	//left	// Improve: Map limits & eliminate magic numbers
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