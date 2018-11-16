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
	PATROLLING,
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

protected:
	enemy_state status;
	fPoint spawnPosition;
	bool airborne;			// Flag to mark if enemy is on air (not colliding with anything)

	uint deadTimer = 0;		// Timer used for death
	ushort deathDelay;		// Time delay between death and despawn/elimination

	bool mustReset = false;	// Flag used to restart animations when needed (skipping workflow steps)

private:
	enemy_type enemyType;
};

#endif //__ENEMY_H__