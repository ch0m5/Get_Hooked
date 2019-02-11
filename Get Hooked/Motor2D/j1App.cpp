#include <iostream> 

#include "Brofiler/Brofiler.h"

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityManager.h"
#include "j1UserInterface.h"
#include "j1Fonts.h"
#include "Player.h"			// CHANGE/FIX: Should be here?
#include "j1Collision.h"	// @Carles
#include "j1FadeScene.h"	// @Carles
#include "j1App.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(perfTimer);

	want_to_save = want_to_load = false;

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();
	pathfinding = new j1PathFinding();
	font = new j1Fonts();
	ui = new j1UserInterface();
	entityManager = new j1EntityManager();
	collision = new j1Collision();
	fade = new j1FadeScene();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(font);
	AddModule(ui);
	AddModule(entityManager);
	AddModule(collision);

	// Scene and fade right before render for management and blitting purposes
	AddModule(scene);
	AddModule(fade);

	// Render last to swap buffer
	AddModule(render);

	PERF_PEEK(perfTimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(perfTimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		name.create(app_config.child("name").child_value());
		organization.create(app_config.child("organization").child_value());

		save_game.create(app_config.child("save").child_value());	// @Carles
		load_game.create(app_config.child("load").child_value());	// @Carles

		fpsCap = app_config.attribute("fpsCap").as_uint();
		mustCapFPS = app_config.attribute("mustCap").as_bool();
	}

	if(ret == true)
	{
		p2List_item<j1Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	PERF_PEEK(perfTimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(perfTimer);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	gameTimer.Start();

	PERF_PEEK(perfTimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;

	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();

	if (mustShutDown)
		ret = false;

	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	BROFILER_CATEGORY("App PrepareUpdate", Profiler::Color::SlateGray);

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		mustCapFPS = !mustCapFPS;

	totalFrameCount++;
	currFPS++;
	dt = frameTimer.ReadSec();

	// Restart timers
	frameTimer.Start();
	delayTimer.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	BROFILER_CATEGORY("App FinishUpdate", Profiler::Color::Gray);

	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	FramerateLogic();
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	BROFILER_CATEGORY("App PreUpdate", Profiler::Color::GreenYellow);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	BROFILER_CATEGORY("App AllUpdates", Profiler::Color::Yellow);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	//if (App->scene->gamePaused == true)
		//dt = 0.0f;

	for (item = modules.start; item != NULL && ret == true; item = item->next)	//All logic
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->UpdateTick(dt);
	}

	for (item = modules.start; item != NULL && ret == true; item = item->next)	//All graphic
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->Update();
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	BROFILER_CATEGORY("App PostUpdate", Profiler::Color::YellowGreen);

	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(perfTimer);

	bool ret = true;

	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	PERF_PEEK(perfTimer);

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
}

// ---------------------------------------
void j1App::GetSaveGames(p2List<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	load_game = save_game.GetString();	// @Carles
	
	pugi::xml_parse_result result = data.load_file(load_game.GetString());

	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.GetString());

		root = data.child("game_state");

		p2List_item<j1Module*>* item = modules.start;
		ret = true;

		while(item != NULL && ret == true)
		{
			ret = item->data->Load(root.child(item->data->name.GetString()));
			item = item->next;
		}

		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	p2SString save_game_file = save_game.GetString();	// @Carles, IMPROVE: Could work with a list of saved files instead of having just one

	LOG("Saving Game State to %s...", save_game_file.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	p2List_item<j1Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if(ret == true)
	{
		data.save_file(save_game_file.GetString());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

void j1App::FramerateLogic() {
	if (secTimer.Read() > 1000) {
		secTimer.Start();
		prevFPS = currFPS;
		currFPS = 0;
	}

	avgFPS = float(totalFrameCount) / gameTimer.ReadSec();
	gameTime = gameTimer.ReadSec();
	lastFrameMs = frameTimer.Read();

	if (App->scene->debugMode == true) {
		App->win->SetTitle(DebugTitle().GetString());
	}
	else {
		App->win->SetTitle(DefaultTitle().GetString());
	}

	if (mustCapFPS) {
		int delayTime = (1000 / fpsCap) - lastFrameMs;
		if (delayTime > 0) {
			SDL_Delay((Uint32)delayTime);
			LOG("We waited for %u and got back in %f", delayTime, delayTimer.ReadMs());
		}
	}
}

p2SString j1App::DebugTitle()	// @Carles
{
	iPoint playerPos;
	if (App->entityManager->player != nullptr) {
		playerPos = { (int)App->entityManager->player->GetPosition().x, (int)App->entityManager->player->GetPosition().y, };
	}
	else {
		playerPos = { 0, 0 };
	}

	title.create("%s (FPS: %i / Av.FPS: %.2f / MsPF: %02u ms / fpsCap: %i / Vsync: %i / Play Time: %.3f / Position: %dx%d / Camera: %dx%d)",
		name.GetString(),
		prevFPS,
		avgFPS,
		lastFrameMs,
		(int)mustCapFPS,
		(int)App->render->Vsync,
		gameTime,
		playerPos.x, playerPos.y,
		App->render->camera.x, App->render->camera.y);

	return title;
}

p2SString j1App::DefaultTitle()	// @Carles
{
	title.create("%s (FPS: %i / Av.FPS: %.2f / MsPF: %02u ms / fpsCap: %i / Vsync: %i)",
		name.GetString(),
		prevFPS,
		avgFPS,
		lastFrameMs,
		(int)mustCapFPS,
		(int)App->render->Vsync);

	return title;
}