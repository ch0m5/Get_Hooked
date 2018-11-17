#ifndef __BAT_H__
#define __BAT_H__	// @CarlesHoms

#include "Enemy.h"

class Bat : public Enemy
{
public:
	Bat();

	virtual ~Bat();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool UpdateLogic(float dt);
	bool UpdateTick(float dt);
	bool Update();

	//virtual bool CleanUp() defined on Enemy.h

	// Save and Load
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

private:	// @Carles
	//void ImportAllStates(pugi::xml_node&);		// Import all state data from config.xml
	void ImportAllSprites(pugi::xml_node&);		// Import all sprite data using the above function for each animation
	void AllocAllAnimations();					// Allocate all animations with previously recieved sprite data

	//Entity
	//void CheckInput();
	
	//Dynamic Entity
	void CheckState();		// Check player state
	void ApplyState();		// Add state effects like movement restrictions, animation and sounds
	void Move(float dt);	// Move player position and decide/calculate other movement related factors
	void UpdateHitbox();	// Transform player collider depending on new position and state

private:
	fPoint spawnPosition;

	bool airborne;			// Flag to mark if player is on air (not colliding with anything)

	uint deadTimer = 0;		// Timer used for player death
	ushort deathDelay;		// Time delay between death and start FadeToBlack

	bool mustReset = false;		// Flag used to restart animations when needed (skipping workflow steps)

	//Character sprites
	sprite_data followSprite;
	sprite_data idleSprite;
	sprite_data deadSprite;
};

#endif //__ENEMY_H__