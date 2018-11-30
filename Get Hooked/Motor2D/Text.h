#ifndef __TEXT_H__
#define __TEXT_H__

#include "UIElement.h"

struct _TTF_Font;
struct SDL_Color;

class Text : public UIElement	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//Constructor
	Text(const char* content, fPoint position, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	//Destructor
	virtual ~Text();

public:
	const char* GetText() const;
	SDL_Color GetColor() const;
	_TTF_Font* GetFont() const;

	const char* ChangeText(const char* string, SDL_Color color, _TTF_Font* font);
	p2SString ChangeText(p2SString string, SDL_Color color, _TTF_Font* font);
	void ChangeColor(SDL_Color color);

protected:
	p2SString content;
	SDL_Color color;
	_TTF_Font* font;

	//uint lineChars;
	//uint maxChars;
};

#endif //__TEXT_H_