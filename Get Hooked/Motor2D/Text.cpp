#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "j1Fonts.h"
#include "Text.h"

Text::Text(const char* content,
	SDL_Color color,
	_TTF_Font* font,
	fPoint center,
	Image* parent) : Image(image_type::TEXT, center, &LoadText(content, color, font), App->font->Print(content, color, font), this, parent), color(color), font(font)
{
	this->content.create(content);	//@Carles: p2SString constructor restarts the string otherwise
}

Text::~Text()
{}

bool Text::UpdateTick(float dt)
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

bool Text::Update()
{
	bool ret = true;

	Draw(currentSprite);

	return ret;
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

SDL_Rect Text::LoadText(const char* string, SDL_Color color, _TTF_Font* font)
{
	SDL_Rect tmpRect = { 0, 0, 0, 0 };
	content.create(string);
	graphics = App->font->Print(string, color, font);
	App->tex->GetSize(graphics, (uint&)tmpRect.w, (uint&)tmpRect.h);
	return tmpRect;
}

SDL_Rect Text::ChangeText(const char* string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		App->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content.create(string);
	graphics = App->font->Print(string, color, font);
	App->tex->GetSize(graphics, (uint&)this->texRect.w, (uint&)this->texRect.h);
	return texRect;
}

SDL_Rect Text::ChangeText(p2SString string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		App->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content = string;
	graphics = App->font->Print(string.GetString(), color, font);
	App->tex->GetSize(graphics, (uint&)this->texRect.w, (uint&)this->texRect.h);
	texRect.x = texRect.y = 0;
	return texRect;
}

void Text::ChangeColor(SDL_Color color)
{
	graphics = App->font->Print(content.GetString(), color, font);
}