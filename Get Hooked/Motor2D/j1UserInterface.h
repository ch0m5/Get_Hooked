#ifndef __j1USERINTERFACE_H__
#define __j1USERINTERFACE_H__

#include "j1Module.h"
#include "p2List.h"

#define CURSOR_WIDTH 2

class UIElement;
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

	// Called each frame
	bool UpdateTick(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions

public:
	SDL_Texture* GetAtlas() const;

	UIElement* CreateScreenElement() {};

private:
	pugi::xml_node configNode;
	p2List<UIElement*> screenElements;
	p2List<SDL_Rect*> imageRects;

	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1USERINTERFACE_H__