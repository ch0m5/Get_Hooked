#include "Brofiler/Brofiler.h"
#include "j1EntityManager.h"
#include "Entity.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"

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

void Entity::Draw(SDL_Rect* animRect) const
{
	if (lookingRight) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, animRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, animRect, SDL_FLIP_HORIZONTAL);
	}
}