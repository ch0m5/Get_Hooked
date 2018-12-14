#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Image.h"

#include "j1App.h"
#include "j1Input.h"

struct SDL_Texture;

enum class button_state
{
	DISABLED = 0,
	IDLE,
	HOVERING,
	PRESSING,

	MAX_TYPES
};

//normal button <void>
//check button <void, bool*>
//input text <void, Text*>

template <class Ret, class... Args>
class Button : public Image	//IMPROVE: Change all entity class and file names to j2EntityName
{
public:
	//typedef Ret(*buttonAction)(Args...);	//IMPROVE: Add this typedef instead of directly Ret(*action)(Args...)

	//Constructor	//IMPROVE: Create a paralel "AnimatedButton" class holding an animation (or several)
	template<class Ret, class... Args> Button(Ret(*action)(Args...), ui_type type, fPoint center, SDL_Rect spriteRect, SDL_Texture* tex, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL)
		: Image(type, center, spriteRect, tex, dynamic, parent, children), action(action), status(button_state::IDLE)
	{};

	virtual ~Button()
	{
		RELEASE(sprite);
	}

	//Button action calling
	Ret operator() (Args&... args) const
	{
		return (action)(args...);
	}

	Ret DoAction(Args&... args) const{
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
	{}

	virtual void OnHover()
	{}

	virtual void OnPress()
	{
		DoAction(Args...);
	}

	virtual void WhileIdle() {}

	virtual void WhileHover() {}

	virtual void WhilePress() {}

	//Enable/Disable
	virtual void Enable()
	{
		status = button_state::IDLE;
	}

	virtual void Disable()
	{
		status = button_state::DISABLED;
	}

protected:
	Ret(*action)(Args...);
	button_state status;
	
};

#endif //__BUTTON_H__