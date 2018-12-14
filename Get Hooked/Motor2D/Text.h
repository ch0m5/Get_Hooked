#ifndef __TEXT_H__
#define __TEXT_H__

#include "Image.h"
#include "SDL\include\SDL_pixels.h"

struct _TTF_Font;

class Text : public Image	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//Constructor
	Text(const char* content, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, fPoint center = { 0, 0 }, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);

public:
	void operator+ (const char* addition) {
		content += addition;
	}

	void operator- (int timesBack) {
		content.Cut(0, timesBack);
	}

	const char* GetText() const;
	SDL_Color GetColor() const;
	_TTF_Font* GetFont() const;

	SDL_Rect LoadRectSize(const char* string, SDL_Color color, _TTF_Font* font);
	SDL_Texture* LoadTex(const char* string, SDL_Color color, _TTF_Font* font);
	SDL_Rect ChangeText(const char* string, SDL_Color color, _TTF_Font* font);
	SDL_Rect ChangeText(p2SString string, SDL_Color color, _TTF_Font* font);
	SDL_Rect ChangeColor(SDL_Color color);
	SDL_Rect ChangeFont(_TTF_Font* font);

protected:
	p2SString content;
	SDL_Color color;
	_TTF_Font* font = nullptr;
	
	//uint lineChars;
	//uint maxChars;
};

#endif //__TEXT_H_