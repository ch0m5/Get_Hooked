#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Animation.h"

struct SDL_Texture;

enum class entity_type
{
	NONE = -1,

	//Creatures
	NPC,
	PLAYER,
	ENEMY,

	//Enemies
	BAT,
	SLIME,

	//Projectiles
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE,

	//Items
	ITEM,

	//Environment
	CHECKPOINT,

	MAX_TYPES
};

class Entity	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//Constructor
	Entity(entity_type type) : type(type) {};

	//Destructor
	virtual ~Entity() {};

	// Called on entity creation
	virtual void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called between a certain number of frames or times per second
	virtual bool UpdateLogic(float dt) { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt) { return true; }

	// Called each loop iteration (graphic)
	virtual bool Update() { return true; }

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp() { return true; }

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual entity_type GetType() const;
	virtual fPoint GetPosition() const;
	virtual fPoint GetCenterPosition() const;

protected:
	virtual void Draw(SDL_Rect* animRect) const;

public:
	p2SString name;
	p2SString folder;
	bool active;
	bool mustDestroy = false;

protected:
	fPoint position;
	fPoint centerPosition;
	bool lookingRight;	// IMPROVE: Change to an enum "orientation" for more blitting parameters

	p2SString textureName;
	SDL_Texture* graphics = nullptr;

private:
	entity_type type;
};

#endif //__ENTITY_H__