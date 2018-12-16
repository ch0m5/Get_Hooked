#ifndef __COIN_H__
#define __COIN_H__

#include "Item.h"

class Coin : public Item
{
public:
	//Constructor
	Coin();

	bool Start();

private:
	void ImportAllSprites(pugi::xml_node&);
	void AllocAllAnimations();

	void UpdateHitbox();

protected:
	bool picked = false;

	sprite_data idleSprite;
};

#endif //__CREATURE_H__