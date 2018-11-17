#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__	// @Carles

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

class Entity;

enum collider_type {	// @Carles, collider types
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLATFORM,
	COLLIDER_FALLING_PLATFORM,
	COLLIDER_PLAYER,
	COLLIDER_PLAYER_ATTACK,
	COLLIDER_ENEMY,
	COLLIDER_ENEMY_ATTACK,
	COLLIDER_END,
	COLLIDER_HAZARD,

	COLLIDER_MAX
};

enum class collision_type {	//@Carles, enumerates collision types
	NONE = -1,
	UNDEFINED,
	ON_LEFT,
	ON_RIGHT,
	ON_TOP,
	ON_BOTTOM
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	collider_type type;
	Entity* callback = nullptr;
	bool airborne;	// CHANGE/FIX: We might need this for the enemies but who knows

	Collider() :
		rect({ 0,0 }),
		type(collider_type::COLLIDER_NONE),
		callback(nullptr)
	{}

	Collider(SDL_Rect rectangle, collider_type type, Entity* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	collider_type GetType() const { return type; }

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:
	j1Collision();

	virtual ~j1Collision();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called each loop iteration
	bool PreUpdate();
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Save and Load
	bool Load(pugi::xml_document& map_file);
	//bool Save(pugi::xml_node&) const;

	Collider* AddCollider(SDL_Rect rect, collider_type type, Entity* callback);	//@Carles
	void DestroyCollider(Collider* collider);

	void DebugDraw();

public:	// @Carles
	bool CheckGroundCollision(Collider* hitbox) const;

public:
	bool mustDebugDraw;
	p2List<Collider> colliders;

private:
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	SDL_Rect screen;
};

#endif	//__j1COLLISION_H__