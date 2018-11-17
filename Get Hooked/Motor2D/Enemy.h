#ifndef __ENEMY_H__
#define __ENEMY_H__	// @CarlesHoms

#include "Entity.h"

enum class enemy_type {
	NONE = -1,
	BAT,

	MAX_TYPES
};

enum class enemy_state {
	IDLE,
	PATROLING,
	FOLLOWING,
	ATTACKING,
	FALLING,
	HURT
};

class Enemy : public Entity
{
public:
	Enemy(enemy_type enemyType) : Entity(entity_type::ENEMY), enemyType(enemyType) {}

	virtual ~Enemy() {}

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
	//virtual bool PostUpdate();

	// Called before quitting
	virtual bool CleanUp();

	// Save and Load
	virtual bool Load(pugi::xml_node &entities);

	virtual bool Save(pugi::xml_node &entities) const;

	collision_type OnCollision(Collider*, Collider*);
	collision_type WallCollision(Collider* c1, Collider* c2);

protected:
	virtual void ImportAllStates(pugi::xml_node&);		// Import all state data from config.xml
	virtual void ImportAllSprites(pugi::xml_node&) {};	// Import all sprite data using the above function for each animation
	virtual void AllocAllAnimations() {};				// Allocate all animations with previously recieved sprite data

	//Entity
	virtual void CheckInput();
	virtual bool InsideRadius(fPoint playerPos);

protected:
	bool canFly;
	enemy_state status;
	fPoint spawnPosition;	// TODO: Load from map?
	bool airborne;			// Flag to mark if enemy is on air (not colliding with anything)

	fPoint detectionRadius;
	bool playerInRadius = false;

	uint deadTimer = 0;		// Timer used for death
	ushort deathDelay;		// Time delay between death and despawn/elimination

	bool mustReset = false;	// Flag used to restart animations when needed (skipping workflow steps)

private:
	enemy_type enemyType;
};

#endif //__ENEMY_H__