#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "Entity.h"
#include "j1Player.h"

// Constructor
j1EntityManager::j1EntityManager()
{
	name.create("entityManager");
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
	player = (j1Player*)App->entityManager->CreateEntity(entity_type::PLAYER);

	bool ret = true;

	p2List_item<Entity*>* item;
	item = entities.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Awake(config.child(item->data->name.GetString()));
		item = item->next;
	}

	return ret;
}

// Called before the first frame
bool j1EntityManager::Start()
{
	bool ret = true;

	p2List_item<Entity*>* item;
	item = entities.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	return ret;
}

// Called each loop iteration
bool j1EntityManager::PreUpdate()
{
	bool ret = true;
	p2List_item<Entity*>* item;
	item = entities.start;
	Entity* tmpEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->PreUpdate();
	}

	return ret;
}

// Called each frame (logic)
bool j1EntityManager::UpdateTick(float dt)
{
	accumulatedTime += dt;

	if (accumulatedTime >= MsToUpdate)
		mustCheckLogic = true;

	UpdateEntities(dt, mustCheckLogic);

	if (mustCheckLogic == true) {
		accumulatedTime = accumulatedTime - MsToUpdate;
		mustCheckLogic = false;
	}

	return true;
}

// Called each loop iteration (graphic)
bool j1EntityManager::Update()
{
	return true;
}

bool j1EntityManager::UpdateEntities(float dt, bool mustCheckLogic)
{
	bool ret = true;

	p2List_item<Entity*>* item;
	Entity* tmpEntity = nullptr;

	for (item = entities.start; item != nullptr && ret == true; item = item->next)
	{
		tmpEntity = item->data;

		/*if (tmpEntity->active == false) {
			continue;
		}*/

		if (mustCheckLogic) {
			ret = tmpEntity->UpdateTick(dt);
		}

		if (ret)
			ret = tmpEntity->Update();
	}

	return ret;
}

// Called each loop iteration
bool j1EntityManager::PostUpdate()
{
	bool ret = true;
	p2List_item<Entity*>* item;
	item = entities.start;
	Entity* tmpEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1EntityManager::CleanUp()
{
	return true;
}

// Save and Load
bool j1EntityManager::Load(pugi::xml_node& managerNode)
{
	bool ret = true;

	p2List_item<Entity*>* item = entities.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Load(managerNode.child(item->data->name.GetString()));
		item = item->next;
	}
	
	return true;
}

bool j1EntityManager::Save(pugi::xml_node& managerNode) const
{
	bool ret = true;

	p2List_item<Entity*>* item = entities.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Save(managerNode.append_child(item->data->name.GetString()));
		item = item->next;
	}

	return true;
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

Entity* j1EntityManager::CreateEntity(entity_type type)
{
	static_assert((int)entity_type::MAX_TYPES == 7, "Entity enum is not accurate");

	Entity* ret = nullptr;
	switch (type) {
	//case entity_type::NPC:
	//	ret = new NPC();
	//	break;¡
	case entity_type::PLAYER:
		ret = new j1Player();
		break;
	//case entity_type::PLAYER_ATTACK:
	//	ret = new PlayerAttack();
	//	break;
	case entity_type::ENEMY:
		//ret = new Enemy();
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

	if (ret != nullptr)
		entities.add(ret);

	return ret;
}

void j1EntityManager::DestroyEntity(Entity* entity)
{
	RELEASE(entity);
}