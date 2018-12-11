#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Image.h"

class Text;

class Window : public Image	//IMPROVE: Change all ui class and file names to j2UiName
{
public:
	//Constructor
	Window(fPoint center, p2List<Image*> children, SDL_Rect* texRect = NULL, SDL_Texture* tex = NULL, Text* label = NULL, Image* parent = NULL);
	
	//Destructor
	virtual ~Window() {};

	// Called each loop iteration (logic)
	virtual bool UpdateTick(float dt);

	// Called each loop iteration (graphic)
	virtual bool Update();

public:
	virtual fPoint DefaultLabelPos();

protected:
	p2List<Image*> children;
	//Button* closeWindow;

protected:

};

#endif //__WINDOW_H__