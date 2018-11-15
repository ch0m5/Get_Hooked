#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__	// @CarlesHoms

#include "p2Animation.h"

enum class state;
enum class collision_type;
struct Collider;

//Components
struct movement_flags {
	bool movingUp;
	bool movingRight;
	bool movingLeft;
	bool movingDown;
};

struct sprite_data {	// @Carles, struct used to store xml data of the first sprite of an animation to then automatize the animation allocation process
	iPoint sheetPosition;
	SDL_Rect colliderOffset;
	uint numFrames;
	Animation anim;
};

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

	// Called between a certain number of frames
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
	//Entity
	virtual fPoint GetPosition() const;

	//Dynamic Entity
	virtual fPoint GetSpeed() const;
	virtual fPoint GetAcceleration() const;
	virtual state GetState() const;

	//Physical Entity
	virtual Collider* GetCollider();
	virtual collision_type OnCollision(Collider*, Collider*) { return (collision_type)-1; }

	//Living Entity
	virtual uint GetLife() const;
	virtual void Kill();
	virtual void Hurt();
	virtual bool IsDead() const;

protected:
	//Entity
	virtual void CheckInput() {};
	virtual void Draw() {};

	//Dynamic Entity
	virtual void CheckMovement();	// Check player current movement
	virtual void CheckState() {};		// Check player state
	virtual void ApplyState() {};		// Add state effects like movement restrictions, animation and sounds
	virtual void Move(float dt) {};		// Move player position and decide/calculate other movement related factors
	virtual void UpdateHitbox() {};		// Transform player collider depending on new position and state
	virtual fPoint Entity::LimitSpeed();

public:
	p2SString name;
	p2SString folder;

protected:
	//Entity
	fPoint position;
	movement_flags movement;
	Animation* animPtr = nullptr;
	SDL_Rect posRect;
	SDL_Rect animRect;

	//Dynamic Entity
	fPoint speed;
	fPoint maxSpeed;
	fPoint acceleration;
	state status;
	bool lookingRight;	// Flag for blit flipping and hurt speed x direction

	//Physical Entity
	Collider* hitbox;
	SDL_Rect hitboxOffset;

	//Living Entity
	int life;
	uint maxLife;
	bool dead;

private:
	entity_type type;
};

#endif //__j1ENTITY_H__