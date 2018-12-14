#include "Brofiler/Brofiler.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Image.h"
#include "Text.h"
#include "j1Window.h"

Image::Image(ui_type type, fPoint center, SDL_Rect sprite, SDL_Texture* tex, bool dynamic, UIElement* parent, p2List<UIElement*>* children) : UIElement(type, center, dynamic, parent, children), graphics(tex)
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

	if (dynamic && MouseOnImage() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		iPoint mousePos;
		App->input->GetMousePosition(mousePos.x, mousePos.y);

		if (setMouseGrabPos == false) {
			grabOffset = { (float)mousePos.x - position.x, (float)mousePos.y - position.y };
			setMouseGrabPos = true;
		}
	}
	else if (setMouseGrabPos == true && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		iPoint mousePos;
		App->input->GetMousePosition(mousePos.x, mousePos.y);

		fPoint newGrabOffset;
		newGrabOffset = { (float)mousePos.x - position.x, (float)mousePos.y - position.y };

		fPoint mouseMov = { newGrabOffset.x - grabOffset.x, newGrabOffset.y - grabOffset.y };
		position.x += mouseMov.x;
		position.y += mouseMov.y;
		center.x += mouseMov.x;
		center.y += mouseMov.y;

		for (p2List_item<UIElement*>* item = children.start; item != nullptr; item = item->next) {
			item->data->position.x += mouseMov.x;
			item->data->position.y += mouseMov.y;
			item->data->center.x += mouseMov.x;
			item->data->center.y += mouseMov.y;
		}
	}
	else {
		setMouseGrabPos = false;
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

bool Image::Draw() const
{
	bool ret = true;

	if (lookingRight) {
		ret = App->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, false);
	}
	else {
		ret = App->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_HORIZONTAL, false);
	}

	return ret;
}

bool Image::DebugDraw() const
{
	bool ret = true;

	/* COLORS
		Red: 255, 0, 0
		Green: 0, 255, 0
		Blue: 0, 0, 255
		Yellow: 255, 255, 0
		Magenta: 255, 0, 255
		Cyan: 0, 255, 255
		White: 255, 255, 255
		Black: 0, 0, 0
		Brown: 160, 128, 96
	*/

	// Top-Left Corner
	ret = App->render->DrawLine((int)position.x, (int)position.y, (int)(position.x + sprite->w), (int)position.y, 255, 0, 0, 255, false);	// Right
	ret = App->render->DrawLine((int)position.x, (int)position.y, (int)position.x, (int)position.y + sprite->h, 255, 0, 0, 255, false);	// Down
	// Down-Right Corner
	ret = App->render->DrawLine((int)(position.x + sprite->w), (int)(position.y + sprite->h), (int)position.x, (int)(position.y + sprite->h), 255, 0, 0, 255, false);	// Up
	ret = App->render->DrawLine((int)(position.x + sprite->w), (int)(position.y + sprite->h), (int)(position.x + sprite->w), (int)position.y, 255, 0, 0, 255, false);	// Left

	return ret;
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

bool Image::MouseOnImage() {	//IMPROVE: USEFUL SDL ALTERNATIVE -> SDL_bool ret = SDL_PointInRect(&mousePos, sprite);
	SDL_Point mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);

	return !(position.y + sprite->h < mousePos.y || position.y > mousePos.y || position.x + sprite->w < mousePos.x || position.x > mousePos.x);
}