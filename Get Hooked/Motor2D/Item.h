#ifndef __ITEM_H__
#define __ITEM_H__

#include "PhysicalElement.h"

enum class collision_type;
struct Collider;
struct SDL_Texture;

class Item : public PhysicalElement
{
public:
	//Constructor
	Item(entity_type type) : PhysicalElement(type) {
		pickedUp = false;
	};

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();

	// Called each loop iteration
	virtual bool PreUpdate();

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt);

	// Called each loop iteration (graphic)
	virtual bool Update();

	// Called each loop iteration
	virtual bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Save and Load
	virtual bool Load(pugi::xml_node &entities);
	virtual bool Save(pugi::xml_node &entities) const;

	collision_type OnCollision(Collider*, Collider*);

public:
	virtual void Spawn(int posX, int posY);
	virtual void Respawn();

	virtual void ImportAllStates(pugi::xml_node&) {};		// Import all state data from config.xml
	virtual void ImportAllSprites(pugi::xml_node&) {};	// Import all sprite data using the above function for each animation
	virtual void AllocAllAnimations() {};				// Allocate all animations with previously recieved sprite data
	virtual void UpdateHitbox() {};

protected:
	bool pickedUp;
};

#endif //__CREATURE_H__