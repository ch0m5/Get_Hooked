#ifndef __ACTION_BOX_H__
#define __ACTION_BOX_H__

#include "Button.h"

struct SDL_Texture;

template <class Ret, class... Args>
class ActionBox : public Button<Ret, Args...>
{
public:
	//Constructor
	template<class Ret, class... Args> ActionBox(Ret(*action)(Args...), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL)
		: Button<Ret, Args...>(action, ui_type::BUTTON_ACTION, center, spriteList[(int)button_state::IDLE], tex, dynamic, parent, children)
	{
		stateSprites = new SDL_Rect[(int)button_state::MAX_TYPES];

		for (int i = 0; i < 4; i++) {
			stateSprites[i] = spriteList[i];
		}
	};

	virtual ~ActionBox()
	{
		RELEASE(sprite);
		RELEASE_ARRAY(stateSprites);
	}

	//Button action calling
	Ret operator() (Args&... args) const
	{
		return (action)(args...);
	}

	Ret DoAction(Args&... args) const {
		return (action)(args...);
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
		DoAction(Args...);
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

private:
	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed
};

#endif //__ACTION_BOX_H__