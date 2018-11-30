#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Image.h"

Image::Image(fPoint position, SDL_Rect texRect, SDL_Texture* tex) : UIElement(ui_element_type::IMAGE, position)
{
	size = { texRect.w, texRect.h };

	if (tex = NULL)
		graphics = App->ui->GetAtlas();
	else
		graphics = tex;
};

SDL_Rect Image::GetTexRect() const
{
	return texRect;
}