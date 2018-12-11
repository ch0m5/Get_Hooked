#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Image.h"

class Text;

class Window : public Image	//IMPROVE: Change all ui class and file names to j2UiName
{
public:
	//Constructor
	Window(fPoint center, SDL_Rect texRect, SDL_Texture* tex, UIElement* parent = NULL, p2List<UIElement*>* children = NULL, p2List<iPoint>* childPositions = NULL);

	//Destructor
	~Window();

	// Called each loop iteration (logic)
	virtual bool UpdateTick(float dt);

protected:
	p2List<iPoint> childPositions;
	//Button* closeWindow;
};

#endif //__WINDOW_H__