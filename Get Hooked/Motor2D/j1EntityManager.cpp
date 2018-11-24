#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"

#include "j1EntityManager.h"
#include "Entity.h"
#include "Player.h"

#include "Enemy.h"
#include "Bat.h"
#include "Slime.h"

// Constructor
j1EntityManager::j1EntityManager()
{
	name.create("entityManager");
	player = (Player*)CreateEntity(entity_type::PLAYER);

	//Enemy spawns
	//pugi::xml_document configFile;
	//pugi::xml_node spawnPoints = LoadConfig(configFile);
	//spawnPoints = spawnPoints.child("entityManager").child("entities").child("spawnPoints");

	//Enemy* tmpPtr;
	//pugi::xml_node lastChild = spawnPoints.last_child();	//CHANGE/FIX: Terrible workaround, but couldn't do it any better in time. Works, but it shouldn't be here loading config out of nowhere.

	//for (spawnPoints = spawnPoints.first_child(); spawnPoints != lastChild; spawnPoints = spawnPoints.next_sibling()) {
	//	tmpPtr = (Enemy*)CreateEntity(entity_type::ENEMY, (enemy_type)spawnPoints.attribute("type").as_int());
	//	tmpPtr->spawnPosition.x = spawnPoints.attribute("xSpawn").as_int();
	//	tmpPtr->spawnPosition.y = spawnPoints.attribute("ySpawn").as_int();
	//}
}

// Destructor
j1EntityManager::~j1EntityManager()
{
	p2List_item<Entity*>* item = entities.end;

	while (item != NULL)
	{
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

	pugi::xml_node entities_node;

	entities_node = config.child("entities");

	if (entities_node.empty() == false)
	{
		ret = true;

		p2List_item<Entity*>* item;
		item = entities.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->Awake(entities_node.child(item->data->name.GetString()));
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
	Entity* tmpEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		if (item->data != player) {	// Activate entities around player, deactivate the ones that aren't
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

	accumulatedTime += dt * 1000;

	int delayTime = (1000 / App->GetFrameCap()) * (App->GetFrameCap() / logicPerSecond);

	/*if (App->FramerateCapped())
		delayTime = (1000 / App->GetFrameCap()) * (App->GetFrameCap() / logicPerSecond);	//CHANGE/FIX: Work on this and how to implement it
	else
		delayTime = (1000 / App->GetCurrentFPS()) * (App->GetCurrentFPS() / logicPerSecond);*/

	if (accumulatedTime >= delayTime)
		mustCheckLogic = true;

	UpdateEntities(dt, mustCheckLogic);

	if (mustCheckLogic == true) {
		accumulatedTime = accumulatedTime - delayTime;
		mustCheckLogic = false;
	}

	return true;
}

bool j1EntityManager::UpdateEntities(float dt, bool mustCheckLogic)
{
	BROFILER_CATEGORY("Update Entities", Profiler::Color::LawnGreen);

	bool ret = true;

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

		if (ret)
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
	}

	entities.clear();

	return ret;
}

bool j1EntityManager::CleanEnemies()
{
	bool ret = true;

	p2List_item<Entity*>* item;
	p2List_item<Entity*>* prevItem;

	for (item = entities.end; item != NULL && ret == true; item = prevItem)
	{
		prevItem = item->prev;

		if (item->data->GetType() == entity_type::ENEMY) {
			ret = item->data->CleanUp();
			entities.del(item);
		}
	}

	return ret;
}

pugi::xml_node j1EntityManager::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;
	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

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

Entity* j1EntityManager::CreateEntity(entity_type type, enemy_type enemy)
{
	static_assert((int)entity_type::MAX_TYPES == 7, "Entity enum is not accurate");

	Entity* ret = nullptr;
	switch (type) {
	//case entity_type::NPC:
	//	ret = new NPC();
	//	break;¡
	case entity_type::PLAYER:
		ret = new Player();
		break;
	//case entity_type::PLAYER_ATTACK:
	//	ret = new PlayerAttack();
	//	break;
	case entity_type::ENEMY:
		ret = CreateEnemy(enemy);
		break;
	//case entity_type::ENEMY_ATTACK:
	//	ret = new Enemy_Attack();
	//	break;
	//case entity_type::ITEM:
	//	ret = new Item();
	//	break;
	//case entity_type::CHECKPOINT:
	//	ret = new Checkpoint();
	//	break;
	}

	if (ret != nullptr) {
		entities.add(ret);
		ret->Init();
	}

	return ret;
}

Enemy* j1EntityManager::CreateEnemy(enemy_type enemy)
{
	static_assert((int)enemy_type::MAX_TYPES == 2, "Entity enum is not accurate");

	Enemy* ret = nullptr;
	switch (enemy) {
		case enemy_type::BAT:
			ret = new Bat();
			break;
		case enemy_type::SLIME:
			ret = new Slime();
			break;
		default:
			break;
	}

	pugi::xml_document config_file;	//IMPROVE: Make function?
	pugi::xml_node config = LoadConfig(config_file);
	ret->Awake(config.child("EntityManager").child("entities").child(ret->name.GetString()));

	return (Enemy*)ret;
}

void j1EntityManager::DestroyEntity(p2List_item<Entity*>* item)
{
	item->data->CleanUp();
	entities.del(item);
}