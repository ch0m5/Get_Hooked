#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "j1Fonts.h"
#include "Text.h"

Text::Text(const char* content, SDL_Color color, _TTF_Font* font, fPoint center, bool dynamic, UIElement* parent, p2List<UIElement*>* children)
	: Image(ui_type::TEXT, center, LoadRectSize(content, color, font), LoadTex(content, color, font), dynamic, parent, children), color(color), font(font)	//CHANGE/FIX: Print done twice, needs fix
{
	this->content.create(content);	//@Carles: p2SString constructor restarts the string otherwise
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

SDL_Rect Text::LoadRectSize(const char* string, SDL_Color color, _TTF_Font* font)
{
	SDL_Rect tmp = { 0, 0, 0, 0 };
	content.create(string);
	graphics = App->font->Print(string, color, font);
	App->tex->GetSize(graphics, (uint&)tmp.w, (uint&)tmp.h);
	App->tex->UnLoad(graphics);
	graphics = nullptr;
	return tmp;
}

SDL_Texture* Text::LoadTex(const char* string, SDL_Color color, _TTF_Font* font)
{
	SDL_Rect tmp = { 0, 0, 0, 0 };
	content.create(string);
	graphics = App->font->Print(string, color, font);
	return graphics;
}

SDL_Rect Text::ChangeText(const char* string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		App->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content.create(string);
	graphics = App->font->Print(string, color, font);
	App->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeText(p2SString string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		App->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content = string;
	graphics = App->font->Print(string.GetString(), color, font);
	App->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeColor(SDL_Color color)
{
	if (graphics != nullptr) {
		App->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	this->color = color;
	graphics = App->font->Print(content.GetString(), color, font);
	return *sprite;
}

SDL_Rect Text::ChangeFont(_TTF_Font* font)
{
	if (graphics != nullptr) {
		App->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	this->font = font;
	graphics = App->font->Print(content.GetString(), color, font);
	App->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}