#include "Brofiler/Brofiler.h"
#include "j1UserInterface.h"
#include "UIElement.h"
#include "j1App.h"
#include "j1Render.h"

ui_element_type UIElement::GetType() const
{
	return type;
}

fPoint UIElement::GetPosition() const
{
	return position;
}

fPoint UIElement::GetCenterPosition() const
{
	return centerPosition;
}

iPoint UIElement::GetSize() const
{
	return size;
}

void UIElement::Draw(SDL_Rect* spriteRect) const
{
	if (lookingRight) {
		App->render->Blit(App->ui->GetAtlas(), size.x, size.y, spriteRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(App->ui->GetAtlas(), size.x, size.y, spriteRect, SDL_FLIP_HORIZONTAL);
	}
}

fPoint UIElement::RelocateCenterByPos()
{
	centerPosition = { position.x + (size.x / 2), position.y + (size.y / 2) };
	return centerPosition;
}

fPoint UIElement::RelocatePosByCenter()
{
	position = { centerPosition.x - (size.x / 2), centerPosition.y - (size.y / 2) };
	return position;
}

fPoint UIElement::MatchPosition(fPoint reference)
{
	position = reference;
	RelocateCenterByPos();
	return position;
}

fPoint UIElement::MatchCenter(fPoint reference)
{
	centerPosition = reference;
	RelocatePosByCenter();
	return centerPosition;
}