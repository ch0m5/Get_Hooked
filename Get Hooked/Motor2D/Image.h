#ifndef __IMAGE_H_
#define __IMAGE_H_

#include "UIElement.h"

class Image	: public UIElement
{
public:
	//Constructor
	Image(ui_type type, fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);

	//Destructor
	virtual ~Image();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt);

	// Called each loop iteration (graphic)
	virtual bool Update();

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual iPoint GetSize() const;
	virtual SDL_Rect* GetSprite() const;

	virtual fPoint RelocateCenterByPos();
	virtual fPoint RelocatePosByCenter();
	virtual fPoint MatchPosition(fPoint reference);
	virtual fPoint MatchCenter(fPoint reference);

	virtual bool MouseOnImage();

	virtual bool Draw() const;
	virtual bool DebugDraw() const;

public:
	p2SString folder;

	bool lookingRight = true;	// IMPROVE: Change to an enum "orientation" for more blitting parameters

protected:
	SDL_Rect* sprite = nullptr;
	SDL_Texture* graphics = nullptr;

	fPoint grabOffset;
	bool setMouseGrabPos = false;
};

#endif //__IMAGE_H__