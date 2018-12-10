#include "Brofiler/Brofiler.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1UserInterface.h"
#include "Image.h"
#include "Text.h"

Image::Image(image_type type, fPoint position, SDL_Rect* texRect, SDL_Texture* tex, Text* label, Image* parent) : type(type), position(position), texRect(*texRect), graphics(tex), parent(parent)
{
	currentSprite = &this->texRect;
	RelocateCenterByPos();

	if (label != NULL) {
		this->label = label;
		this->label->MatchCenter(centerPosition);
		this->label->RelocatePosByCenter();
		this->label->parent = this;
	}

	Init();
}

Image::~Image()
{
	RELEASE(label);
}

bool Image::Update()
{
	bool ret = true;

	Draw(currentSprite);

	if (label != NULL) {
		label->Draw(label->currentSprite);
	}

	return ret;
}

bool Image::CleanUp()
{
	bool ret = true;

	App->ui->DestroyElement(this);	//CHANGE/FIX: Implement dynamic memory}

	return ret;
}

image_type Image::GetType() const
{
	return type;
}

fPoint Image::GetPosition() const
{
	return position;
}

fPoint Image::GetCenterPosition() const
{
	return centerPosition;
}

iPoint Image::GetSize() const
{
	iPoint size = { texRect.w, texRect.h };
	return size;
}

void Image::Draw(SDL_Rect* spriteRect) const
{
	if (lookingRight) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, spriteRect, SDL_FLIP_NONE);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, spriteRect, SDL_FLIP_HORIZONTAL);
	}
}

fPoint Image::RelocateCenterByPos()
{
	centerPosition = { position.x + (texRect.w / 2), position.y + (texRect.h / 2) };
	return centerPosition;
}

fPoint Image::RelocatePosByCenter()
{
	position = { centerPosition.x - (texRect.w / 2), centerPosition.y - (texRect.h / 2) };
	return position;
}

fPoint Image::MatchPosition(fPoint reference)
{
	position = reference;
	RelocateCenterByPos();
	return position;
}

fPoint Image::MatchCenter(fPoint reference)
{
	centerPosition = reference;
	RelocatePosByCenter();
	return centerPosition;
}