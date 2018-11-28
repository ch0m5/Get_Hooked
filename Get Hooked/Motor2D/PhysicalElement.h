#ifndef __PHYSICAL_ELEMENT_H__
#define __PHYSICAL_ELEMENT_H__

#include "Entity.h"

enum class collision_type;
struct Collider;
struct SDL_Texture;

//Components
struct movement_flags {
	bool movingUp;
	bool movingRight;
	bool movingLeft;
	bool movingDown;
};

struct movement_input {
	bool wantMoveUp;
	bool wantMoveRight;
	bool wantMoveLeft;
	bool wantMoveDown;
};

struct sprite_data {	// @Carles, struct used to store xml data of the first sprite of an animation to then automatize the animation allocation process
	//p2SString id;	//IMPROVE: Make sprite_data lists and use this to mark them
	iPoint sheetPosition;
	SDL_Rect colliderOffset;
	uint numFrames;
	Animation anim;
};

class PhysicalElement : public Entity
{
public:
	//Constructor
	PhysicalElement(entity_type type) : Entity(type) {};

	//Destructor
	virtual ~PhysicalElement();

public:
	virtual bool InsideRadius(fPoint targetPosition, iPoint radius);

	virtual fPoint GetSpeed() const;
	virtual fPoint GetAcceleration() const;

	virtual Collider* GetCollider();
	virtual collision_type OnCollision(Collider*, Collider*) { return (collision_type)-1; }
	virtual bool GetCollisionIgnore() const;

protected:
	virtual void CheckInput() {};
	virtual void ImportSpriteData(const char* spriteName, sprite_data* sprite, pugi::xml_node&);	// Import sprite data from config.xml

	virtual void CheckMovement();		// Check player current movement
	virtual void CheckState() {};		// Check player state
	virtual void ApplyState() {};		// Add state effects like movement restrictions, animation and sounds
	virtual void Move(float dt) {};		// Move player position and decide/calculate other movement related factors
	virtual void UpdateHitbox() {};		// Transform player collider depending on new position and state
	virtual bool CheckOrientation();
	virtual fPoint LimitSpeed();
	SDL_Rect ReshapeCollider(sprite_data sprite);

protected:
	fPoint prevPosition;	//IMPROVE: NEW COLLISION SYSTEM
	movement_input input;
	movement_flags movement;

	iPoint spriteSize;
	Animation* animPtr = nullptr;
	SDL_Rect animRect;
	//p2List<sprite_data*> spriteList;

	fPoint speed;
	fPoint maxSpeed;
	fPoint acceleration;
	float gravity;

	Collider* hitbox;
	SDL_Rect hitboxOffset;
	bool onlyMapCollisions = false;
};

#endif //__PHYSICAL_ELEMENT_H__