#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Window.h"
#include "Text.h"

Window::Window(fPoint center, p2List<Image*> children, SDL_Rect* texRect, SDL_Texture* tex, Text* label, Image* parent) : Image(image_type::WINDOW, center, texRect, tex, label, parent)
{
	dynamic = true;

	if (children.start != nullptr) {
		for (p2List_item<Image*>* item = children.start; item != nullptr; item = item->next) {
			item->data->parent = this;
			this->children.add(item->data);
		}
	}
};

bool Window::UpdateTick(float dt)
{
	bool ret = true;

	if (dynamic && MouseOnImage() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {	//CHANGE/FIX: Make function
		iPoint mouseMov;
		App->input->GetMouseMotion(mouseMov.x, mouseMov.y);
		position.x += mouseMov.x;
		position.y += mouseMov.y;
		RelocateCenterByPos();

		for (p2List_item<Image*>* item = children.start; item != nullptr; item = item->next) {
			item->data->position.x += mouseMov.x;
			item->data->position.y += mouseMov.y;
			item->data->RelocateCenterByPos();
		}
	}

	return ret;
}

bool Window::Update()
{
	bool ret = true;

	Draw(currentSprite);

	for (p2List_item<Image*>* item = children.start; item != nullptr; item = item->next) {
		Draw(item->data->GetSprite());
	}

	return ret;
}

fPoint Window::DefaultLabelPos()
{
	this->label->MatchCenter(center);
	return center;
}