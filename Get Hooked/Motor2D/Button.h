#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Image.h"
#include "j1Input.h"

class Text;
struct SDL_Texture;

//enum class button_action
//{
//	NONE = -1,
//	CHANGE_SCENE,
//	SWITCH_VALUE,
//	ENTER_TEXT,
//	SAVE_GAME,
//	LOAD_GAME,
//	CLOSE_GAME,
//
//	MAX_TYPES
//};

enum class button_state
{
	DISABLED = -1,
	IDLE,
	HOVERING,
	PRESSING,

	MAX_TYPES
};

template <class Ret, class... Args>
class Button : public Image	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	typedef Ret(*buttonAction)(Args...);

	//Constructor	//IMPROVE: Create a paralel "AnimatedButton" class holding an animation (or several)
	Button(buttonAction action,
		fPoint position,
		SDL_Rect* spriteList[4],
		image_type type,
		SDL_Texture* tex = NULL,
		Image* parent = NULL) : Image(type, position, spriteList[0], SDL_Texture* tex = NULL, Text* label = NULL, Image* parent = NULL), action(action), label(label)
	{
		for (int i = 0; i < 4; i++) {
			stateSprites[i] = spriteList[i];
		}
	};

	//Operators
	void operator() (Args&... args) const
	{
		(m_funcPtr)(args...);
	}

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt)
	{
		bool ret = true;

		if (status != button_state::DISABLED) {
			CheckCurrentState();
			ButtonStateEffects();
		}

		return ret;
	}

protected:
	virtual bool MouseOnButton()
	{
		iPoint mousePos;
		App->input->GetMousePosition(mousePos.x, mousePos.y);

		return !(position.y + texRect.h < mousePos.y || position.y > mousePos.y || position.x + texRect.w < mousePos.x || position.x > mousePos.x);
	}

	virtual button_state CheckCurrentState()
	{
		switch (status) {
		case button_state::IDLE:
			if (MouseOnButton() == true) {
				OnHover();
				status = button_state::HOVERING;
			}
			break;
		case button_state::HOVERING:
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				OnPress();
				status = button_state::PRESSING;
			}
			else if (MouseOnButton() == false) {
				OnIdle();
				status = button_state::IDLE;
			}
			break;
		case button_state::PRESSING:
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP || MouseOnButton() == false) {
				OnIdle();
				status = button_state::IDLE;
			}
			break;
		}

		return status;
	}

	virtual button_state ButtonStateEffects()
	{
		switch (status) {
		case button_state::IDLE:
			WhileIdle();
			break;
		case button_state::HOVERING:
			WhileHover();
			break;
		case button_state::PRESSING:
			WhilePress();
			break;
		}

		return status;
	}

	virtual void OnIdle()
	{
		texRect = *stateSprites[1];
	}

	virtual void OnHover()
	{
		texRect = *stateSprites[2];
	}

	virtual void OnPress()
	{
		texRect = *stateSprites[3];
	}

	virtual void WhileIdle() {}

	virtual void WhileHover() {}

	virtual void WhilePress() {}

	//Enable/Disable
	virtual void Enable()
	{
		status = button_state::IDLE;
		texRect = *stateSprites[1];
	}

	virtual void Disable()
	{
		status = button_state::DISABLED;
		texRect = *stateSprites[0];
	}

protected:
	SDL_Rect* stateSprites[4];	//Disabled, Idle, Hover, Pressed
	//p2SString stateTexts[4];

private:
	buttonAction action;
	button_state status;
	
};

#endif //__BUTTON_H__