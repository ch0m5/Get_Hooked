#ifndef __ENEMY_H__
#define __ENEMY_H__	// @CarlesHoms

#include "Creature.h"

enum class enemy_state {
	IDLE,
	PATROLING,
	FOLLOWING,
	ATTACKING,
	FALLING,
	HURT
};

class Enemy : public Creature
{
public:
	Enemy(entity_type enemyType) : Creature(enemyType) {};

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	//virtual bool Start();

	// Called each loop iteration
	virtual bool PreUpdate();

	// Called between a certain number of frames or times per second
	virtual bool UpdateLogic(float dt);

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
	collision_type WallCollision(Collider* c1, Collider* c2);

public:
	virtual void Spawn(int posX, int posY);
	virtual void Respawn();

protected:
	virtual void Hurt();
	virtual void ImportAllStates(pugi::xml_node&);		// Import all state data from config.xml
	virtual void ImportAllSprites(pugi::xml_node&) {};	// Import all sprite data using the above function for each animation
	virtual void AllocAllAnimations() {};				// Allocate all animations with previously recieved sprite data

	virtual void CheckInput();
	virtual bool InsideRadius(fPoint targetPosition, iPoint radius);

	// Add downwards acceleration to Y speed
	virtual void Fall(float dt);

protected:
	bool canFly;
	enemy_state status;
	bool airborne;			// Flag to mark if enemy is on air (not colliding with anything)

	iPoint detectionRadius;
	iPoint attackRange;
	bool playerDetected = false;
	bool playerInRange = false;
	bool wantToAttack = false;

	uint attackTimer = 0;
	uint hurtTimer = 0;
	uint deadTimer = 0;

	ushort attackDelay;	//Time to stay attacking
	ushort hurtDelay;	//Time to stay hurt
	ushort deathDelay;	//Time before despawn

	bool mustReset = false;	// Flag used to restart animations when needed (skipping workflow steps)
};

#endif //__ENEMY_H__