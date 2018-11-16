#ifndef __BAT_H__
#define __BAT_H__	// @CarlesHoms

#include "Enemy.h"

class Bat : public Enemy
{
public:
	Bat();

	virtual ~Bat();

	//// Called before render is available
	//bool Awake(pugi::xml_node&);

	//// Called before the first frame
	//bool Start();

	//// Called each loop iteration
	//bool PreUpdate();
	//bool UpdateTick(float dt);
	//bool Update();

	//// Called before quitting
	//bool CleanUp();

	//// Called when colliding
	//collision_type OnCollision(Collider* c1, Collider* c2);	// @Carles
	//collision_type WallCollision(Collider* c1, Collider* c2);

	//// Save and Load
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

private:	// @Carles
	//void ImportAllStates(pugi::xml_node&);		// Import all state data from config.xml
	//void ImportAllSprites(pugi::xml_node&);		// Import all sprite data using the above function for each animation
	//void AllocAllAnimations();					// Allocate all animations with previously recieved sprite data

private:
	fPoint spawnPosition;

	bool airborne;			// Flag to mark if player is on air (not colliding with anything)

	uint deadTimer = 0;		// Timer used for player death
	ushort deathDelay;		// Time delay between death and start FadeToBlack

	bool mustReset = false;		// Flag used to restart animations when needed (skipping workflow steps)

	//Character sprites
	sprite_data idleSprite;
};

#endif //__ENEMY_H__