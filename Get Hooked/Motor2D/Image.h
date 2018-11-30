#ifndef __IMAGE_H_
#define __IMAGE_H_

#include "UIElement.h"

class Image : public UIElement	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//Constructor
	Image(fPoint position, SDL_Rect texRect, SDL_Texture* tex = NULL);	//IMPROVE: Create a paralel "AnimatedImage" class holding an animation (or several)

	//Destructor
	virtual ~Image() {};

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
	SDL_Rect GetTexRect() const;

private:
	SDL_Rect texRect;
};

#endif //__IMAGE_H_