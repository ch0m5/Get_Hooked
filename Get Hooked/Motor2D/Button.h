#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "UIElement.h"

class Image;
class Text;
struct SDL_Texture;

enum class button_action
{
	NONE = -1,
	CHANGE_SCENE,
	SWITCH_VALUE,
	ENTER_TEXT,
	SAVE_GAME,
	LOAD_GAME,
	CLOSE_GAME,

	MAX_TYPES
};

enum class button_state
{
	DISABLED = -1,
	IDLE,
	HOVERING,
	PRESSING,

	MAX_TYPES
};

class Button : public UIElement	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//Constructor
	Button(button_action action, fPoint position, SDL_Rect texRect, const char* string, ui_element_type = ui_element_type::BUTTON_NORMAL, SDL_Texture* = NULL); 	//IMPROVE: Create a paralel "AnimatedButton" class holding an animation (or several)

	//Destructor
	virtual ~Button();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called between a certain number of frames or times per second
	virtual bool UpdateLogic(float dt) { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt) { return true; }

	// Called each loop iteration (graphic)
	virtual bool Update() { return true; }

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp() { return true; }

public:

protected:
	virtual bool MouseOnButton();
	virtual button_state CheckCurrentState();
	virtual button_state ButtonStateEffects();
	
	virtual void OnIdle();
	virtual void OnHover();
	virtual void OnPress();
	virtual void WhileIdle();
	virtual void WhileHover();
	virtual void WhilePress();

	virtual void Enable();
	virtual void Disable();

protected:


public:


protected:
	SDL_Rect texRect;
	Image* image = nullptr;
	Text* idleText = nullptr;

private:
	button_action action;
	button_state status;
	
};

#endif //__BUTTON_H__