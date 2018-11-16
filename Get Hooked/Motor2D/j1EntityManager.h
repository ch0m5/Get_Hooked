#ifndef __j1ENTITYMANAGER__
#define __j1ENTITYMANAGER__	// @CarlesHoms

#include "j1Module.h"
#include "p2List.h"

class Entity;
class Player;
class Enemy;
enum class entity_type;
enum class enemy_type;

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

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	Entity* CreateEntity(entity_type type, enemy_type enemy = (enemy_type)-1);
	Enemy* CreateEnemy(enemy_type type);
	void DestroyEntity(Entity* entity);

private:
	bool UpdateEntities(float dt, bool mustCheckLogic);
	pugi::xml_node LoadEntitiesXML(pugi::xml_document&) const;
	
public:
	p2List<Entity*> entities;
	Player* player;
	p2List<Enemy*> enemies;

private:
	ushort logicPerSecond;	// Number of times logic is checked each second
	float accumulatedTime;
	bool mustCheckLogic = false;

};

#endif //__j1ENTITYMANAGER__