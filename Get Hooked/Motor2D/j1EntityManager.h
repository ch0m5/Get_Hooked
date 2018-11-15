#ifndef __j1ENTITYMANAGER__
#define __j1ENTITYMANAGER__	// @CarlesHoms

#include "j1Module.h"
#include "p2List.h"

class Entity;
class j1Player;
enum class entity_type;

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

	// Called each loop iteration (graphic)
	bool Update();

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	Entity* CreateEntity(entity_type type);
	void DestroyEntity(Entity* entity);
	bool UpdateEntities(float dt, bool mustCheckLogic);
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

public:
	p2List<Entity*> entities;
	j1Player* player;

private:
	float accumulatedTime;
	float MsToUpdate;
	bool mustCheckLogic = false;

};

#endif //__j1ENTITYMANAGER__