#include "Brofiler/Brofiler.h"
#include "j1EntityManager.h"
#include "Entity.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"

Entity::~Entity()
{
	if (hitbox != nullptr) {
		hitbox->to_delete = true;
		hitbox = nullptr;
	}	
}

entity_type Entity::GetType() const
{
	return type;
}

fPoint Entity::GetPosition() const
{
	return position;
}

fPoint Entity::GetCenterPosition() const
{
	return centerPosition;
}


fPoint Entity::GetSpeed() const
{
	return speed;
}

fPoint Entity::GetAcceleration() const
{
	return acceleration;
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

void Entity::Draw(SDL_Rect* animRect) const
{
	if (lookingRight == true) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, animRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, animRect, SDL_FLIP_HORIZONTAL);
	}
}

void Entity::ImportSpriteData(const char* spriteName, sprite_data* sprite, pugi::xml_node& first_sprite)
{
	sprite->sheetPosition.x = first_sprite.child(spriteName).attribute("column").as_int();
	sprite->sheetPosition.y = first_sprite.child(spriteName).attribute("row").as_int();
	sprite->numFrames = first_sprite.child(spriteName).attribute("frames").as_uint();
	sprite->anim.speed = first_sprite.child(spriteName).attribute("animSpeed").as_float();
	sprite->anim.loop = first_sprite.child(spriteName).attribute("loop").as_bool();

	sprite->colliderOffset.x = first_sprite.child(spriteName).child("offset").attribute("x").as_int();
	sprite->colliderOffset.y = first_sprite.child(spriteName).child("offset").attribute("y").as_int();
	sprite->colliderOffset.w = first_sprite.child(spriteName).child("offset").attribute("w").as_int();
	sprite->colliderOffset.h = first_sprite.child(spriteName).child("offset").attribute("h").as_int();
}

bool Entity::CheckOrientation(bool orientation)
{
	bool ret = orientation;

	if (input.wantMoveRight == true && input.wantMoveLeft == false) {
		orientation = true;
	}
	else if (input.wantMoveLeft == true && input.wantMoveRight == false) {
		orientation = false;
	}

	return orientation;
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

SDL_Rect Entity::ReshapeCollider(sprite_data sprite)
{
	hitbox->rect.x = (int)position.x + sprite.colliderOffset.x;
	hitbox->rect.y = (int)position.y + sprite.colliderOffset.y;
	hitbox->rect.w = sprite.colliderOffset.w;
	hitbox->rect.h = sprite.colliderOffset.h;

	return sprite.colliderOffset;
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