#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Image.h"

class Text;

class Window : public Image	//IMPROVE: Change all ui class and file names to j2UiName
{
public:
	//Constructor
	Window(fPoint position, p2List<Image*> children, SDL_Rect* texRect = NULL, SDL_Texture* tex = NULL, Text* label = NULL, Image* parent = NULL);
	
	//Destructor
	virtual ~Window() {};

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt) { return true; }

	// Called each loop iteration (graphic)
	virtual bool Update() { return true; }

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp() { return true; }

public:
	virtual iPoint MoveToTitlePos(Text* label);

protected:
	p2List<Image*> children;
	//Button* closeWindow;

protected:

};

#endif //__WINDOW_H__