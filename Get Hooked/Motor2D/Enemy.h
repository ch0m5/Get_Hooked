#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__	// @CarlesHoms

//#include "j1Module.h"
//#include "Components.h"
//
//enum class entity_type
//{
//	NONE = -1,
//	NPC,
//	PLAYER,
//	PLAYER_ATTACK,
//	ENEMY,
//	ENEMY_ATTACK,
//	ITEM,
//	CHECKPOINT,
//
//	MAX_TYPES
//};
//
//class j1Entity
//{
//public:
//	//Constructor
//	j1Entity(entity_type type) : type(type) {}
//
//	virtual ~j1Entity();
//
//	// Called before render is available
//	virtual bool Awake(pugi::xml_node&);
//
//	// Called before the first frame
//	virtual bool Start();
//
//	// Called each loop iteration
//	virtual bool PreUpdate();
//
//	// Called each frame (logic)
//	virtual bool UpdateTick(float dt);
//
//	// Called each loop iteration (graphic)
//	virtual bool Update();
//
//	// Called each loop iteration
//	virtual bool PostUpdate();
//
//	// Called before quitting
//	virtual bool CleanUp();
//
//public:
//	virtual void Input();
//	virtual void Draw();
//
//private:	// @Carles
//	p2SString name;
//	entity_type type;
//	fPoint position;
//	sprite_data sprite;
//};
//
//#endif //__j1ENTITY_H__