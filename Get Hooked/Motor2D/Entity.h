#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__	// @CarlesHoms

#include "p2Point.h"

enum class state;
struct Collider;
enum class collision_type;

enum class entity_type
{
	NONE = -1,
	NPC,
	PLAYER,
	PLAYER_ATTACK,
	ENEMY,
	ENEMY_ATTACK,
	ITEM,
	CHECKPOINT,

	MAX_TYPES
};

//struct sprite_data {	//IMPROVE: Make into component?
//	SDL_Rect sheetPosition;
//	float animSpeed;
//	uint numFrames;
//};

class Entity
{
public:
	//Constructor
	Entity(entity_type type) : type(type) {}

	virtual ~Entity() {}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (logic)
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
	//Entity
	virtual void Input() {};
	virtual void Draw() {};
	//virtual fPoint GetPosition() const;

	////Dynamic Entity
	//virtual fPoint GetSpeed() const;
	//virtual fPoint GetAcceleration() const;
	//virtual state GetState() const;

	////Physical Entity
	//virtual Collider* GetCollider();
	virtual collision_type OnCollision(Collider*, Collider*) { return (collision_type)-1; }

	////Living Entity
	//virtual uint GetLife() const;
	//virtual void Kill();
	//virtual void Hurt();
	//virtual bool IsDead() const;
	
public:
	p2SString name;

private:
	//Entity
	entity_type type;
	fPoint position;
	//sprite_data sprite;
	//p2List<Animation> animations;

	////Dynamic Entity
	//fPoint speed;
	//fPoint acceleration;
	//state status;

	////Physical Entity
	//Collider hitbox;

	////Living Entity
	//int life;
	//bool dead;
};

#endif //__j1ENTITY_H__