#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "p2SString.h"
#include "p2Point.h"
#include "p2List.h"
#include "PugiXml/src/pugixml.hpp"

struct SDL_Rect;
struct SDL_Texture;

enum class ui_type
{
	NONE = -1,

	IMAGE,
	TEXT,
	WINDOW,
	BUTTON_ACTION,
	BUTTON_CHECK,
	BUTTON_INPUT,

	MAX_TYPES
};

class UIElement	//IMPROVE: Change all ui class and file names to j2UiName
{
public:
	//Constructor
	UIElement(ui_type type, fPoint position, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);

	//Destructor
	virtual ~UIElement();

	// Called on entity creation
	virtual void Init() {
		active = true;
	}

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt) { return true; };

	// Called each loop iteration (graphic)
	virtual bool Update() { return true; };

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp();

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual ui_type GetType() const;
	virtual UIElement* GetParent() const;
	virtual fPoint GetPosition() const;
	virtual fPoint GetCenter() const;

	virtual fPoint MatchPosition(fPoint reference);
	virtual fPoint MatchCenter(fPoint reference);

	virtual UIElement* AttachParent(UIElement* parent);
	virtual void DetachParent();

	virtual p2List_item<UIElement*>* AttachChild(UIElement* child);
	virtual void DetachChild(UIElement* child);

	virtual bool Draw() const = 0;
	virtual bool DebugDraw() const = 0;

public:
	p2SString name;

	bool active;
	bool mustDestroy = false;

	fPoint position;
	fPoint center;
	bool dynamic = false;

protected:
	UIElement* parent = nullptr;
	p2List<UIElement*> children;

private:
	ui_type type;
};

#endif //__UI_ELEMENT_H__