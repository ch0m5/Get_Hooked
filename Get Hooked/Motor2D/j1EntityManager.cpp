#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Scene.h"

#include "j1EntityManager.h"
#include "Entity.h"
#include "Player.h"

#include "Enemy.h"
#include "Bat.h"
#include "Slime.h"

// Constructor
j1EntityManager::j1EntityManager() : j1Module()
{
	name.create("entityManager");
}

// Destructor
j1EntityManager::~j1EntityManager()
{
	p2List_item<Entity*>* item = entities.end;

	while (item != NULL)
	{
		item->data->CleanUp();
		RELEASE(item->data);
		item = item->prev;
	}

	entities.clear();
}

// Called before render is available
bool j1EntityManager::Awake(pugi::xml_node& config)
{
	bool ret = false;

	active = config.child("start").attribute("active").as_bool();
	logicPerSecond = config.child("logic").attribute("cooldown").as_uint();

	if (config.child("entities").empty() == false)
	{
		ret = true;

		p2List_item<Entity*>* item;
		item = entities.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child("entities").child(item->data->name.GetString()));
			item = item->next;
		}
	}

	return ret;
}

// Called before the first frame
bool j1EntityManager::Start()
{
	bool ret = true;

	p2List_item<Entity*>* item;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->Start();
	}

	return ret;
}

// Called each loop iteration
bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("Module EntityManager PreUpdate", Profiler::Color::ForestGreen);

	bool ret = true;

	p2List_item<Entity*>* item;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		if ( item->data->turnedOn == true && item->data != player) {	// Activate entities around player, deactivate the ones that aren't
			if (player->InsideRadius(item->data->GetPosition(), player->GetActivationRadius()) == true) {
				item->data->active = true;
			}
			else {
				item->data->active = false;
			}
		}
	}

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		if (item->data->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Called each frame (logic)
bool j1EntityManager::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Module EntityManager UpdateTick", Profiler::Color::Green);

	bool ret = true;

	if (App->scene->gamePaused == false) {
		accumulatedTime += dt * 1000;

		int delayTime = (1000 / App->GetFrameCap()) * (App->GetFrameCap() / logicPerSecond);

		/*if (App->FramerateCapped())
			delayTime = (1000 / App->GetFrameCap()) * (App->GetFrameCap() / logicPerSecond);	//CHANGE/FIX: Work on this and how to implement it
		else
			delayTime = (1000 / App->GetCurrentFPS()) * (App->GetCurrentFPS() / logicPerSecond);*/

		if (accumulatedTime >= delayTime)
			mustCheckLogic = true;

		p2List_item<Entity*>* item;
		for (item = entities.start; item != nullptr && ret == true; item = item->next)
		{
			if (item->data->active == false) {
				continue;
			}

			if (mustCheckLogic) {
				ret = item->data->UpdateLogic(dt);	//Update logic in intervals
			}

			if (ret)
				ret = item->data->UpdateTick(dt);	//Update dependant of framerate
		}

		if (mustCheckLogic == true) {
			accumulatedTime = accumulatedTime - delayTime;
			mustCheckLogic = false;
		}
	}

	return ret;
}

bool j1EntityManager::Draw()
{
	BROFILER_CATEGORY("Module EntityManager Update", Profiler::Color::LawnGreen);

	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr && ret == true; item = item->next)
	{
		if (item->data->active == false) {
			continue;
		}

		ret = item->data->Update();	//Update independant of framerate
	}

	return ret;
}

// Called each loop iteration
bool j1EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("Module EntityManager PostUpdate", Profiler::Color::LightGreen);

	bool ret = true;

	p2List_item<Entity*>* item;
	p2List_item<Entity*>* nextItem;

	for (item = entities.start; item != NULL && ret == true; item = nextItem)
	{
		nextItem = item->next;

		if (item->data->active == false) {
			if (item->data->mustDestroy) {	//Destroy all entities that are unactive and marked to be destroyed
				DestroyEntity(item);
			}
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1EntityManager::CleanUp()
{
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.end; item != NULL && ret == true; item = item->prev)
	{
		ret = item->data->CleanUp();
		RELEASE(item->data);
	}
	entities.clear();

	return ret;
}

bool j1EntityManager::CleanEntities()
{
	bool ret = true;

	p2List_item<Entity*>* item;

	for (item = entities.end; item != NULL && ret == true; item = item->prev)
	{
		if (item->data->GetType() != entity_type::PLAYER)	//IMPROVE: Could be done better
			DestroyEntity(item);
	}

	return ret;
}

bool j1EntityManager::RestartEnemies()
{
	bool ret = true;

	p2List_item<Entity*>* item;
	Enemy* itemData;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		if (item->data->GetType() == entity_type::BAT || item->data->GetType() == entity_type::SLIME) {	//IMPROVE: Could be done better
			itemData = (Enemy*)item->data;
			itemData->CleanUp();
			itemData->Start();
			itemData->Respawn();
			itemData->Init();
		}
	}

	return ret;
}

// Save and Load
bool j1EntityManager::Load(pugi::xml_node& managerNode)
{
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != NULL && ret == true; item = item->next)	//CHANGE/FIX: Implementation of enemies save and load needed
	{
		ret = item->data->Load(managerNode.child(item->data->name.GetString()));
	}

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& managerNode) const
{
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != NULL && ret == true; item = item->next)	//CHANGE/FIX: Implementation of enemies save and load needed
	{
		ret = item->data->Save(managerNode.append_child(item->data->name.GetString()));
	}

	return true;
}

Entity* j1EntityManager::CreateEntity(entity_type type, pugi::xml_node entitiesNode)
{
	static_assert((int)entity_type::MAX_TYPES == 9, "Entity enum is not accurate");

	Entity* ret = nullptr;

	switch (type) {
	case entity_type::PLAYER:
		ret = new Player();
		break;
	case entity_type::BAT:
		ret = new Bat();
		break;
	case entity_type::SLIME:
		ret = new Slime();
		break;
	}

	if (ret != nullptr) {
		entities.add(ret);
		ret->Init();
		ret->Awake(entitiesNode.child(ret->name.GetString()));
		ret->Start();
	}

	return ret;
}

void j1EntityManager::DestroyEntity(p2List_item<Entity*>* item)
{
	item->data->CleanUp();
	RELEASE(item->data);
	entities.del(item);
}