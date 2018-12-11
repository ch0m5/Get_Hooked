#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Image.h"

#include "j1App.h"
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

//template <class Ret, class... Args>
class Button : public Image	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//typedef Ret(*buttonAction)(Args...);

	//Constructor	//IMPROVE: Create a paralel "AnimatedButton" class holding an animation (or several)
	Button(void(*action)(void),
		image_type type,
		fPoint center,
		SDL_Rect spriteList[4],
		Text* label = NULL,
		SDL_Texture* tex = NULL,
		Image* parent = NULL) : Image(type, center, &spriteList[0], tex, label, parent), action(action), status(button_state::IDLE)
	{
		for (int i = 0; i < 4; i++) {
			stateSprites[i] = spriteList[i];
		}
	};

	//Operators
	/*void operator() (Args&... args) const
	{
		(m_funcPtr)(args...);
	}*/

	void operator() () const
	{
		(action)();
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
	virtual button_state CheckCurrentState()
	{
		switch (status) {
		case button_state::IDLE:
			if (MouseOnImage() == true) {
				OnHover();
				status = button_state::HOVERING;
			}
			break;
		case button_state::HOVERING:
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				OnPress();
				status = button_state::PRESSING;
			}
			else if (MouseOnImage() == false) {
				OnIdle();
				status = button_state::IDLE;
			}
			break;
		case button_state::PRESSING:
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP || MouseOnImage() == false) {
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
		texRect = stateSprites[1];
	}

	virtual void OnHover()
	{
		texRect = stateSprites[2];
	}

	virtual void OnPress()
	{
		texRect = stateSprites[3];
		action();
	}

	virtual void WhileIdle() {}

	virtual void WhileHover() {}

	virtual void WhilePress() {}

	//Enable/Disable
	virtual void Enable()
	{
		status = button_state::IDLE;
		texRect = stateSprites[1];
	}

	virtual void Disable()
	{
		status = button_state::DISABLED;
		texRect = stateSprites[0];
	}

protected:
	SDL_Rect stateSprites[4];	//Disabled, Idle, Hover, Pressed
	//p2SString stateTexts[4];

private:
	void(*action)(void);
	button_state status;
	
};

#endif //__BUTTON_H__