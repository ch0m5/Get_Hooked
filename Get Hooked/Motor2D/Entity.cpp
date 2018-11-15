#include "j1EntityManager.h"
#include "Entity.h"

fPoint Entity::GetPosition() const
{
	return position;
}

fPoint Entity::GetSpeed() const
{
	return speed;
}

fPoint Entity::GetAcceleration() const
{
	return acceleration;
}

state Entity::GetState() const
{
	return status;
}

Collider* Entity::GetCollider()
{
	return hitbox;
}

uint Entity::GetLife() const
{
	return life;
}

void Entity::Kill()
{
	life = 0;
}

void Entity::Hurt()
{
	life--;
}

bool Entity::IsDead() const
{
	bool ret = false;

	if (life <= 0)
		ret = true;

	return ret;
}

fPoint Entity::LimitSpeed()
{
	if (speed.x > 0)
		speed.x = MIN(speed.x, maxSpeed.x);
	else if (speed.x < 0)
		speed.x = MAX(speed.x, -maxSpeed.x);

	if (speed.y > 0)
		speed.y = MIN(speed.y, maxSpeed.y);
	else if (speed.y < 0)
		speed.y = MAX(speed.y, -maxSpeed.y);

	return speed;
}

void Entity::CheckMovement()
{
	if (speed.x > 0.0f) {
		movement.movingRight = true;
		movement.movingLeft = false;
	}
	else if (speed.x < 0.0f) {
		movement.movingLeft = true;
		movement.movingRight = false;
	}
	else if (speed.x == 0.0f) {
		movement.movingLeft = false;
		movement.movingRight = false;
	}

	if (speed.y < 0.0f) {
		movement.movingUp = true;
		movement.movingDown = false;
	}
	else if (speed.y > 0.0f) {
		movement.movingDown = true;
		movement.movingUp = false;
	}
	else if (speed.y == 0.0f) {
		movement.movingUp = false;
		movement.movingDown = false;
	}
}