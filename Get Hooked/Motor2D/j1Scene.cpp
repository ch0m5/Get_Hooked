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
#include "Enemy.h"
#include "j1Fonts.h"
#include "j1UserInterface.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"

//Button actions	//CHANGE/FIX: Locate somewhere else, having this laying around is quite dirty, but putting them in a header creates wierd problems (Difficulty level: Rick didn't find the issue.
void StartGame()
{
	App->fade->FadeToBlack(App->fade->GetDelay(), fade_type::START_GAME);
}

void CloseGame()
{
	App->mustShutDown = true;
}

void Save()
{
	App->SaveGame();
}

void Load()
{
	App->LoadGame();
}

// ------------------------------------------------------------------------------

//Constructor
j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	loading = true;

	button = new SDL_Rect[4];		//CHANGE/FIX: Could be improved, probably with a list of structs that have an id.
	checkButton = new SDL_Rect[3];
	exit = new SDL_Rect[4];
	shutDown = new SDL_Rect[4];
	settings = new SDL_Rect[4];
}

// Destructor
j1Scene::~j1Scene()
{
	RELEASE_ARRAY(button);
	RELEASE_ARRAY(checkButton);
	RELEASE_ARRAY(exit);
	RELEASE_ARRAY(shutDown);
	RELEASE_ARRAY(settings);
}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	cameraSpeed.x = config.child("cameraSpeed").attribute("x").as_float();
	cameraSpeed.y = config.child("cameraSpeed").attribute("y").as_float();

	debugMode = config.child("debugMode").attribute("value").as_bool();
	scene = (scene_type)config.child("scene").attribute("start").as_int();
	
	pugi::xml_node item;
	for (item = config.child("maps").first_child(); item != NULL; item = item.next_sibling()) {
		maps.add(item.attribute("file").as_string());
	}

	//UI Data Awake
	item = config.child("ui").child("panel");
	panel = { item.attribute("x").as_int(), item.attribute("y").as_int(), item.attribute("w").as_int(), item.attribute("h").as_int() };
	
	item = config.child("ui").child("window");
	window = { item.attribute("x").as_int(), item.attribute("y").as_int(), item.attribute("w").as_int(), item.attribute("h").as_int() };
	
	RegisterButtonData(config.child("ui").child("button"), button);
	
	item = config.child("ui").child("checkButton");
	checkButton[0] = { item.attribute("x1").as_int(), item.attribute("y1").as_int(), item.attribute("w1").as_int(), item.attribute("h1").as_int() };
	checkButton[1] = { item.attribute("x2").as_int(), item.attribute("y2").as_int(), item.attribute("w2").as_int(), item.attribute("h2").as_int() };
	checkButton[2] = { item.attribute("x3").as_int(), item.attribute("y3").as_int(), item.attribute("w3").as_int(), item.attribute("h3").as_int() };

	RegisterButtonData(config.child("ui").child("exit"), exit);
	RegisterButtonData(config.child("ui").child("shutDown"), shutDown);
	RegisterButtonData(config.child("ui").child("settings"), settings);

	item = config.child("ui").child("healthBar");
	healthBar = { item.attribute("x").as_int(), item.attribute("y").as_int(), item.attribute("w").as_int(), item.attribute("h").as_int() };

	item = config.child("ui").child("healthChunck");
	healthChunck = { item.attribute("x").as_int(), item.attribute("y").as_int(), item.attribute("w").as_int(), item.attribute("h").as_int() };

	return ret;
}

void j1Scene::RegisterButtonData(pugi::xml_node& node, SDL_Rect* button)
{
	button[0] = { node.attribute("x1").as_int(), node.attribute("y1").as_int(), node.attribute("w1").as_int(), node.attribute("h1").as_int() };
	button[1] = { node.attribute("x2").as_int(), node.attribute("y2").as_int(), node.attribute("w2").as_int(), node.attribute("h2").as_int() };
	button[2] = { node.attribute("x3").as_int(), node.attribute("y3").as_int(), node.attribute("w3").as_int(), node.attribute("h3").as_int() };
	button[3] = { node.attribute("x4").as_int(), node.attribute("y4").as_int(), node.attribute("w4").as_int(), node.attribute("h4").as_int() };
}

// Called before the first frame
bool j1Scene::Start()	//TODO: Create enemies in their respective positions using XML
{
	bool ret = true;

	pugi::xml_document doc;
	App->LoadConfig(doc);
	pugi::xml_node config = doc.child("config");

	UIElement* parent;

	switch (scene) {	//CHANGE/FIX: Make function?
	case scene_type::MAIN_MENU:	//CHANGE/FIX: Lots of magic numbers, but making it based on the screen size gives a lot of problems (TYPE/int)
		// iPoint screenCenter = { App->win->GetWindowSize().w / (2 * App->win->GetScale()),  App->win->GetWindowSize().h / (2 * App->win->GetScale()) };
		App->ui->CreateText({ 1024 / 4, 100 }, "Get Hooked", DEFAULT_COLOR, App->font->titleFont, false);
		parent = App->ui->CreateActionBox(&StartGame, { 1024 / 4, 180 }, button, NULL, true);
		App->ui->CreateText(DEFAULT_POINT, "Start", DEFAULT_COLOR, NULL, false, parent);
		parent = App->ui->CreateActionBox(&CloseGame, { 1024 / 4, 225 }, button, NULL, true);
		App->ui->CreateText(DEFAULT_POINT, "Exit", DEFAULT_COLOR, NULL, false, parent);
		App->audio->PlayMusic(App->audio->musicMainMenu.GetString());
		break;
	case scene_type::LEVEL_1:
		App->map->Load(maps.At(0)->data.GetString());
		playerStart = { 608, 250 };		//start = App->map->data.checkpoints.start->data;	//CHANGE/FIX: Get points close to the ground
		playerFinish = { 500, 500 };	//finish = App->map->data.checkpoints.end->data;
		if (App->entityManager->player == nullptr) {
			App->entityManager->player = (Player*)App->entityManager->CreateEntity(entity_type::PLAYER, config.child("entityManager").child("entities"));
		}
		SpawnEntities(scene, config);
		App->audio->PlayMusic(App->audio->musicMap1.GetString());
		break;
	case scene_type::LEVEL_2:
		App->map->Load(maps.At(1)->data.GetString());
		playerStart = { 720, -400 };	//start = App->map->data.checkpoints.start->data;
		playerFinish = { 500, 500 };	//finish = App->map->data.checkpoints.end->data;
		SpawnEntities(scene, config);
		App->audio->PlayMusic(App->audio->musicMap2.GetString());
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

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()	//IMPROVE: Full debug input here?
{
	BROFILER_CATEGORY("Module Scene PreUpdate", Profiler::Color::DarkOrange);

	if (debugMode == true) {
		DebugInput();	// Check debug input
	}

	return true;
}

// Called each frame (logic)
bool j1Scene::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Module Scene UpdateTick", Profiler::Color::OrangeRed);

	AudioInput();

	if (debugMode == true && scene > scene_type::CREDITS) {
		CameraInput(dt);
	}

	return true;
}

// Called each loop iteration (graphic)
bool j1Scene::Update()
{
	BROFILER_CATEGORY("Module Scene Update", Profiler::Color::Orange);

	if (App->entityManager->player != nullptr && App->entityManager->player->CameraFree() == false) {
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
		case fade_type::START_GAME:
			App->ui->CleanUp();
			ChangeScene(scene_type::LEVEL_1);
			break;
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
bool j1Scene::CleanUp()	//IMPROVE: When changing scene a lot of new memory is allocated. Memory leaks?
{
	LOG("Freeing scene");
	App->map->CleanUp();
	App->collision->CleanUp();

	return true;
}

void j1Scene::DebugInput()
{
	if (scene > scene_type::CREDITS) {
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

			if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
				gamePaused = !gamePaused;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && App->entityManager->player->IsDead() == false) {	// Save game
			App->SaveGame();
		}

		if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && App->entityManager->player->IsDead() == false) {	// Load game
			App->LoadGame();
		}

		if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && App->ui->mustDebugDraw == false) {	// UI logic drawing
			App->ui->mustDebugDraw = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && App->ui->mustDebugDraw == true) {
			App->ui->mustDebugDraw = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->collision->mustDebugDraw == false) {	// Logic drawing
			App->collision->mustDebugDraw = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->collision->mustDebugDraw == true) {
			App->collision->mustDebugDraw = false;
		}

		// Change scale
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
			App->win->scale = 1;
		}
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
			App->win->scale = 2;
		}
		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
			App->win->scale = 3;
		}
		if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
			App->win->scale = 4;
		}
		if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) {
			App->win->scale = 5;
		}
	}
	else {
		if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && App->ui->mustDebugDraw == false) {	// UI logic drawing
			App->ui->mustDebugDraw = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && App->ui->mustDebugDraw == true) {
			App->ui->mustDebugDraw = false;
		}
	}
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

void j1Scene::RestartLevel()	//IMPROVE: Only reload entities, not the full map
{
	CleanUp();
	App->entityManager->player->CleanUp();
	App->entityManager->CleanEntities();
	Start();
	App->entityManager->player->LifeToStart();
	App->entityManager->player->Start();

	//OLD
	/*App->entityManager->player->DeadReset();
	App->entityManager->RestartEnemies();
	App->entityManager->player->CleanUp();
	App->entityManager->player->LifeToStart();
	App->entityManager->player->Start();*/
}

void j1Scene::ChangeScene(scene_type scene)
{
	this->scene = scene;

	if (App->entityManager->player != nullptr) {
		CleanUp();
		App->entityManager->player->CleanUp();
		App->entityManager->CleanEntities();
		Start();
		App->entityManager->player->Start();
	}
	else {
		CleanUp();
		Start();
	}
	
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

bool j1Scene::SpawnEntities(scene_type level, pugi::xml_node& config)
{
	bool ret = true;

	pugi::xml_node spawns = config.child("scene").child("maps");
	pugi::xml_node entities = config.child("entityManager").child("entities");

	switch (level)
	{
	case scene_type::LEVEL_1:
		spawns = spawns.child("level_1").child("spawns");
		break;
	case scene_type::LEVEL_2:
		spawns = spawns.child("level_2").child("spawns");
		break;
	default:
		return false;
	}

	SpawnEnemies(entities, spawns);

	return ret;
}

bool j1Scene::SpawnEnemies(pugi::xml_node& entitiesNode, pugi::xml_node& spawns)
{
	bool ret = true;

	Enemy* tmpPtr;
	pugi::xml_node lastChild = spawns.last_child();
	for (pugi::xml_node spawnPoints = spawns.first_child(); spawnPoints != lastChild; spawnPoints = spawnPoints.next_sibling()) {
		tmpPtr = (Enemy*)App->entityManager->CreateEntity((entity_type)spawnPoints.attribute("type").as_int(), entitiesNode);
		tmpPtr->Spawn(spawnPoints.attribute("xSpawn").as_int(), spawnPoints.attribute("ySpawn").as_int());
	}

	return ret;
}