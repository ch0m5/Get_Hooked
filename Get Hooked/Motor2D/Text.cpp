#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "j1Fonts.h"
#include "Text.h"

Text::Text(const char* content, fPoint position, SDL_Color color, _TTF_Font* paramFont) : UIElement(ui_element_type::TEXT, position), color(color), font(paramFont)
{
	ChangeText(content, color, font);
}

Text::~Text()
{
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);
}

const char* Text::GetText() const
{
	return content.GetString();
}

SDL_Color Text::GetColor() const
{
	return color;
}

_TTF_Font* Text::GetFont() const
{
	return font;
}

const char* Text::ChangeText(const char* string, SDL_Color color, _TTF_Font* font)
{
	content.create(string);
	graphics = App->font->Print(string, color, font);
	App->tex->GetSize(graphics, (uint&)size.x, (uint&)size.y);
	return content.GetString();
}

p2SString Text::ChangeText(p2SString string, SDL_Color color, _TTF_Font* font)
{
	content = string;
	graphics = App->font->Print(string.GetString(), color, font);
	App->tex->GetSize(graphics, (uint&)size.x, (uint&)size.y);
	return content;
}

void Text::ChangeColor(SDL_Color color)
{
	graphics = App->font->Print(content.GetString(), color, font);
}