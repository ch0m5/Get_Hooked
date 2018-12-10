#include "Brofiler/Brofiler.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1UserInterface.h"
#include "Image.h"
#include "Text.h"

Image::Image(image_type type, fPoint center, SDL_Rect* texRect, SDL_Texture* tex, Text* label, Image* parent) : type(type), center(center), texRect(*texRect), graphics(tex), parent(parent)
{
	if (texRect)
	currentSprite = &this->texRect;
	
	RelocatePosByCenter();

	if (label != NULL) {
		this->label = label;

		if (label != this) {
			this->label->MatchCenter(center);
			this->label->parent = this;
		}
	}

	Init();
}

Image::~Image()
{
	if (label != this)
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

fPoint Image::GetCenter() const
{
	return center;
}

iPoint Image::GetSize() const
{
	iPoint size = { texRect.w, texRect.h };
	return size;
}

void Image::Draw(SDL_Rect* spriteRect) const
{
	if (lookingRight) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, spriteRect, SDL_FLIP_NONE, false);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, spriteRect, SDL_FLIP_HORIZONTAL, false);
	}
}

fPoint Image::RelocateCenterByPos()
{
	center = { position.x + (texRect.w / 2), position.y + (texRect.h / 2) };
	return center;
}

fPoint Image::RelocatePosByCenter()
{
	position = { center.x - (texRect.w / 2), center.y - (texRect.h / 2) };
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
	center = reference;
	RelocatePosByCenter();
	return center;
}