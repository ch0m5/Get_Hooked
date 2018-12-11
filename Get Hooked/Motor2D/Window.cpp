#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Window.h"
#include "Text.h"

Window::Window(fPoint center, SDL_Rect texRect, SDL_Texture* tex, UIElement* parent, p2List<UIElement*>* children, p2List<iPoint>* childPositions)
	: Image(ui_type::WINDOW, center, texRect, tex, parent)
{
	if (childPositions != NULL) {
		this->childPositions = *childPositions;
	}

	dynamic = true;	//CHANGE/FIX: Shouldn't be dynamic by default, needs Awake data
};

Window::~Window()
{}

bool Window::UpdateTick(float dt)
{
	bool ret = true;

	if (dynamic && MouseOnImage() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {	//CHANGE/FIX: Make function
		iPoint mouseMov;
		App->input->GetMouseMotion(mouseMov.x, mouseMov.y);
		position.x += mouseMov.x;
		position.y += mouseMov.y;
		RelocateCenterByPos();

		for (p2List_item<UIElement*>* item = children.start; item != nullptr; item = item->next) {
			item->data->position.x += mouseMov.x;
			item->data->position.y += mouseMov.y;
			item->data->center.x += mouseMov.x;
			item->data->center.y += mouseMov.y;
		}

	}

	return ret;
}