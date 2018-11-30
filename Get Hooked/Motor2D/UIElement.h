#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "p2Animation.h"
#include "p2SString.h"

struct SDL_Texture;

enum class ui_element_type
{
	NONE = -1,
	BUTTON_NORMAL,
	BUTTON_CHECK,
	BUTTON_INPUT,
	TEXT,
	IMAGE,

	MAX_TYPES
};

class UIElement	//IMPROVE: Change all ui class and file names to j2UiName
{
public:
	//Constructor
	UIElement(ui_element_type type, fPoint position, iPoint size = { 0, 0 }) : type(type), position(position), size(size) {
		RelocateCenterByPos();
	};

	//Destructor
	virtual ~UIElement() {};

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
	virtual bool Update() { return true; }

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp() { return true; }

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual ui_element_type GetType() const;
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

protected:
	fPoint position;
	fPoint centerPosition;
	iPoint size;

	SDL_Texture* graphics = nullptr;

private:
	ui_element_type type;
};

#endif //__UI_ELEMENT_H__