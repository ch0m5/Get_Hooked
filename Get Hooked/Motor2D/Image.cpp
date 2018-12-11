#include "Brofiler/Brofiler.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1UserInterface.h"
#include "Image.h"
#include "Text.h"

Image::Image(ui_type type, fPoint center, SDL_Rect sprite, SDL_Texture* tex, UIElement* parent, p2List<UIElement*>* children) : UIElement(type, center, parent, children), graphics(tex)
{
	this->sprite = new SDL_Rect;
	*this->sprite = sprite;
	RelocatePosByCenter();

	if (parent != NULL) {
		MatchCenter(parent->GetCenter());
	}
}

Image::~Image()
{
	RELEASE(sprite);
}

bool Image::UpdateTick(float dt)
{
	bool ret = true;

	if (dynamic && MouseOnImage() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {	//CHANGE/FIX: Make function
		iPoint mouseMov;
		App->input->GetMouseMotion(mouseMov.x, mouseMov.y);
		position.x += mouseMov.x;
		position.y += mouseMov.y;
		RelocateCenterByPos();
	}

	return ret;
}

bool Image::Update()
{
	bool ret = true;

	Draw();

	for (p2List_item<UIElement*>* item = children.start; item != nullptr; item = item->next) {
		item->data->Draw();
	}

	return ret;
}

iPoint Image::GetSize() const
{
	iPoint size = { sprite->w, sprite->h };
	return size;
}

SDL_Rect* Image::GetSprite() const
{
	return sprite;
}

void Image::Draw() const
{
	if (lookingRight) {
		App->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, false);
	}
	else {
		App->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_HORIZONTAL, false);
	}
}

fPoint Image::RelocateCenterByPos()
{
	center = { position.x + (sprite->w / 2), position.y + (sprite->h / 2) };
	return center;
}

fPoint Image::RelocatePosByCenter()
{
	position = { center.x - (sprite->w / 2), center.y - (sprite->h / 2) };
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

bool Image::MouseOnImage() {	//IMPROVE: USEFUL SDL ALTERNATIVE ??? -> SDL_bool ret = SDL_PointInRect(&mousePos, sprite);
	SDL_Point mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);

	return !(position.y + sprite->h < mousePos.y || position.y > mousePos.y || position.x + sprite->w < mousePos.x || position.x > mousePos.x);
}