#ifndef __IMAGE_H_
#define __IMAGE_H_

#include "p2Animation.h"
#include "p2SString.h"

class Text;
struct SDL_Texture;

enum class image_type
{
	NONE = -1,
	BUTTON_ACTION,
	BUTTON_CHECK,
	BUTTON_INPUT,
	TEXT,
	IMAGE,
	WINDOW,

	MAX_TYPES
};

class Image	//IMPROVE: Change all ui class and file names to j2UiName
{
public:
	//Constructor
	Image(image_type type, fPoint position, SDL_Rect* texRect, SDL_Texture* tex = NULL, Text* label = NULL, Image* parent = NULL);

	//Destructor
	virtual ~Image();

	// Called on entity creation
	virtual void Init() {
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt) { return true; }

	// Called each loop iteration (graphic)
	virtual bool Update();

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp();

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual image_type GetType() const;
	virtual fPoint GetPosition() const;
	virtual fPoint GetCenterPosition() const;
	virtual iPoint GetSize() const;

	virtual fPoint RelocateCenterByPos();
	virtual fPoint RelocatePosByCenter();
	virtual fPoint MatchPosition(fPoint reference);
	virtual fPoint MatchCenter(fPoint reference);

protected:
	virtual void Draw(SDL_Rect* animRect) const;

public:
	p2SString name;
	p2SString folder;
	bool active;
	bool mustDestroy = false;
	bool lookingRight = true;	// IMPROVE: Change to an enum "orientation" for more blitting parameters

	fPoint position;
	fPoint centerPosition;

protected:
	SDL_Rect* currentSprite;
	SDL_Rect texRect;

	Text* label = nullptr;
	Image* parent = nullptr;

	SDL_Texture* graphics = nullptr;

private:
	image_type type;
};

#endif //__IMAGE_H__