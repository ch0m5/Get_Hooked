#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Image.h"

#include "j1App.h"
#include "j1Input.h"

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
	DISABLED = 0,
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
	Button(void(*action)(void), ui_type type, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UIElement* parent = NULL, p2List<UIElement*>* children = NULL)
		: Image(type, center, spriteList[(int)button_state::IDLE], tex, parent, children), action(action), status(button_state::IDLE)
	{
		stateSprites = new SDL_Rect[(int)button_state::MAX_TYPES];

		for (int i = 0; i < 4; i++) {
			stateSprites[i] = spriteList[i];
		}
	};

	virtual ~Button()
	{
		RELEASE(sprite);
		RELEASE_ARRAY(stateSprites);
	}

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
		*sprite = stateSprites[(int)button_state::IDLE];
	}

	virtual void OnHover()
	{
		*sprite = stateSprites[(int)button_state::HOVERING];
	}

	virtual void OnPress()
	{
		*sprite = stateSprites[(int)button_state::PRESSING];
		action();
	}

	virtual void WhileIdle() {}

	virtual void WhileHover() {}

	virtual void WhilePress() {}

	//Enable/Disable
	virtual void Enable()
	{
		status = button_state::IDLE;
		*sprite = stateSprites[(int)button_state::IDLE];
	}

	virtual void Disable()
	{
		status = button_state::DISABLED;
		*sprite = stateSprites[(int)button_state::DISABLED];
	}

protected:
	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed

private:
	void(*action)(void) = nullptr;
	button_state status;
	
};

#endif //__BUTTON_H__