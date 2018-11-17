#ifndef __SLIME_H__
#define __SLIME_H__	// @CarlesHoms

#include "Enemy.h"

class Slime : public Enemy
{
public:
	Slime();

	virtual ~Slime();

	// Called before the first frame
	bool Start();

	// Save and Load
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

private:	// @Carles
	void DashAttack();

	void ImportAllStates(pugi::xml_node&);		// Import all state data from config.xml
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
	ushort dashMultiplier;

	//Character sprites
	sprite_data idleSprite;
	sprite_data moveSprite;
	sprite_data attackSprite;
	sprite_data hurtSprite;
	sprite_data deadSprite;
};

#endif //__SLIME_H__