#ifndef __CREATURE_H__
#define __CREATURE_H__

#include "PhysicalElement.h"

enum class collision_type;
struct Collider;
struct SDL_Texture;

class Creature : public PhysicalElement
{
public:
	//Constructor
	Creature(entity_type type) : PhysicalElement(type) {};

	//Destructor
	virtual ~Creature();

public:
	virtual uint GetLife() const;
	virtual uint LifeToMax();
	virtual void Kill();
	virtual void Hurt();
	virtual bool IsDead() const;

protected:
	//p2List<Collider*> attackColliders;
	int life;
	uint maxLife;
	fPoint hurtSpeed;
	bool dead;
};

#endif //__CREATURE_H__