#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Window.h"
#include "Text.h"

Window::Window(fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic, UIElement* parent, p2List<UIElement*>* children, p2List<iPoint>* childPositions)
	: Image(ui_type::WINDOW, center, texRect, tex, dynamic, parent, children)
{};