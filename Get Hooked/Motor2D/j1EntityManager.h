#ifndef __j1ENTITYMANAGER__
#define __j1ENTITYMANAGER__	// @CarlesHoms

#include "j1Module.h"
#include "p2List.h"

class Entity;

class PhysicalElement;
class Creature;
class Player;
class Enemy;

class UIElement;

enum class entity_type;

enum class entity_lists {	//@Carles, all entities are on the entities list. However, some are also registered in other lists for easier localization
	NONE = -1,
	ENTITIES,
	ENEMIES,
	ITEMS,

	MAX_TYPES
};

class j1EntityManager : public j1Module
{
public:
	//Constructor
	j1EntityManager();

	//Destructor
	virtual ~j1EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called each frame (logic)
	bool UpdateTick(float dt);

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	bool CleanEnemies();

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	Entity* CreateEntity(entity_type type);
	void DestroyEntity(p2List_item<Entity*>* item);

private:
	bool UpdateEntities(float dt, bool mustCheckLogic);
	
public:
	p2List<Entity*> entities;
	p2List<Enemy*>	enemies;
	//p2List<Entity*>	items;
	Player* player;

private:
	ushort logicPerSecond;	// Number of times logic is checked each second
	float accumulatedTime;
	bool mustCheckLogic = false;

	pugi::xml_node entitiesNode;
};

#endif //__j1ENTITYMANAGER__