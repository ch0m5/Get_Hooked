#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Window.h"
#include "Text.h"

Window::Window(fPoint position, p2List<Image*> children, SDL_Rect* texRect, SDL_Texture* tex, Text* label, Image* parent) : Image(image_type::WINDOW, position, texRect, tex, label, parent)
{
	if (children.start != nullptr) {
		for (p2List_item<Image*>* item = children.start; item != nullptr; item = item->next) {
			this->children.add(item->data);
		}
	}

	if (label != NULL) {
		MoveToTitlePos(label);
	}
};

iPoint Window::MoveToTitlePos(Text* label)
{
	return { 0, 0 };
}