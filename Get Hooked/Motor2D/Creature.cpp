#include "Brofiler/Brofiler.h"
#include "j1EntityManager.h"
#include "Creature.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"

Creature::~Creature()
{
	if (hitbox != nullptr) {
		hitbox->to_delete = true;
		hitbox = nullptr;
	}
}

uint Creature::GetLife() const
{
	return life;
}

uint Creature::LifeToMax()
{
	life = maxLife;
	return life;
}

void Creature::Kill()
{
	life = 0;
}

void Creature::Hurt()
{
	life--;
}

bool Creature::IsDead() const
{
	bool ret = false;

	if (life <= 0)
		ret = true;

	return ret;
}