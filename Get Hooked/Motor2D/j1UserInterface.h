#ifndef __j1USERINTERFACE_H__
#define __j1USERINTERFACE_H__

#include "j1Module.h"
#include "p2List.h"
#include "Button.h"

#define CURSOR_WIDTH 2

class Image;
class Text;
enum class image_type;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Texture;

class j1UserInterface : public j1Module
{
public:

	j1UserInterface();

	// Destructor
	virtual ~j1UserInterface();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each frame (logic)
	bool UpdateTick(float dt);

	// Called each frame (graphic)
	bool Draw();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	
public:
	SDL_Texture* GetAtlas() const;
	
	void AddElement(Image* element);
	void DestroyElement(Image* element);

	Image* CreateText(fPoint position, const char* content, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, Image* parent = NULL);
	Image* CreateImage(fPoint position, SDL_Rect* texRect = NULL, SDL_Texture* tex = NULL, Text* label = NULL, Image* parent = NULL);
	Image* CreateTextBox(fPoint position, const char* content, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, Image* parent = NULL);


	Image* CreateText(const char* content, fPoint position, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, Image* parent = NULL);
	Image* CreateWindowBox(fPoint position, iPoint size, SDL_Rect texRect, p2List<Image*> elemList, SDL_Texture* tex = NULL, Image* parent = NULL);
	Image* CreateActionBox(Button<void>::buttonAction action, fPoint position, Text* label, SDL_Rect* spriteList[4], image_type type, SDL_Texture* tex = NULL, Image* parent = NULL);
	Image* CreateCheckBox();
	Image* CreateInputText();
	
private:
	pugi::xml_node configNode;
	p2List<Image*> screenElements;

	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1USERINTERFACE_H__