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
	COLLIDER_TYPE type;		//SamAlert: As mentioned before, if we use tiled delete this I guess?
	j1Module* callback = nullptr;
	//bool onGround;	// CHECK_ERIC
	//bool falling;		// CHECK_ERIC

	/*
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
	*/

	COLLIDER_TYPE getType() { return type; }

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
	fPoint AvoidCollision(fPoint speed, Collider& collider);		// CHECK_ERIC
	fPoint CollisionSpeed(SDL_Rect* collider1, SDL_Rect* Collider, fPoint speed);		// CHECK_ERIC
};

//struct ColliderData {
//	p2List<Collider*> colliders;
//};

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

	Collider* AddCollider(Collider* colliderPtr, SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback);	//@Carles
	void DebugDraw();
	p2List<Collider*> colliders;

private:
	bool collisionsLoaded;	// CHECK_ERIC
	
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	SDL_Rect screen;
	bool debug;									//SamAlert: Flag that marks "debug mode" not implemented yet (which let's you use all cheats)
};

#endif	//__j1COLLISION_H__