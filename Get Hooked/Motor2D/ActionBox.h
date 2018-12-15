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

		for (int i = 0; i < (int)button_state::MAX_TYPES; i++) {
			stateSprites[i] = spriteList[i];
		}
	};

	virtual ~ActionBox()
	{
		RELEASE(sprite);
		RELEASE_ARRAY(stateSprites);
	}

public:
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
		App->audio->PlayFx(App->audio->buttonSfx.id, 0);
		*sprite = stateSprites[(int)button_state::PRESSING];
		DoAction(Args...);
	}

	virtual void WhileIdle() {}

	virtual void WhileHover() {}

	virtual void WhilePress() {}

private:
	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed
};

#endif //__ACTION_BOX_H__