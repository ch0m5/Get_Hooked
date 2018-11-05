#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__	// @Carles

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLATFORM,
	COLLIDER_FALLING_PLATFORM,
	COLLIDER_PLAYER,
	COLLIDER_PLAYER_ATTACK,
	COLLIDER_ENEMY,
	COLLIDER_ENEMY_ATTACK,

	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;
	bool airborne;	// CHECK_ERIC

	Collider() :
		rect({ 0,0 }),
		type(COLLIDER_TYPE::COLLIDER_NONE),
		callback(nullptr)
	{}

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	COLLIDER_TYPE GetType() const { return type; }

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
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Save and Load
	bool Load(pugi::xml_document& map_file);
	//bool Save(pugi::xml_node&) const;

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback);	//@Carles
	void DebugDraw();

public:
	bool mustDebugDraw;
	p2List<Collider> colliders;

private:
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	SDL_Rect screen;
};

#endif	//__j1COLLISION_H__