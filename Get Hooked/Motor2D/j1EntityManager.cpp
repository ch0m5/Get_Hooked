#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "Entity.h"
#include "Player.h"

// Constructor
j1EntityManager::j1EntityManager()
{
	name.create("entities");
	player = (Player*)CreateEntity(entity_type::PLAYER);
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
	logicPerSecond = config.child("logic").attribute("cooldown").as_uint();

	pugi::xml_document	entities_file;
	pugi::xml_node		entities_node;

	bool ret = false;

	entities_node = LoadEntitiesXML(entities_file);

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
	bool ret = true;

	p2List_item<Entity*>* item;
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
	accumulatedTime += dt * 1000;

	float delayTime = (1000 / App->GetFrameCap()) * (App->GetFrameCap() / logicPerSecond);

	/*if (App->FramerateCapped())
		delayTime = (1000 / App->GetFrameCap()) * (App->GetFrameCap() / logicPerSecond);
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
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr && ret == true; item = item->next)
	{
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
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
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

	return ret;
}

// Save and Load
bool j1EntityManager::Load(pugi::xml_node& managerNode)
{
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->Load(managerNode.child(item->data->name.GetString()));
	}

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& managerNode) const
{
	bool ret = true;

	p2List_item<Entity*>* item;
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->Save(managerNode.append_child(item->data->name.GetString()));
	}

	return true;
}

pugi::xml_node j1EntityManager::LoadEntitiesXML(pugi::xml_document& entities_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = entities_file.load_file("entities.xml");

	if (result == NULL)
		LOG("Could not load map xml file entities.xml. pugi error: %s", result.description());
	else
		ret = entities_file.child("entities");

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
		ret = new Player();
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