//#ifndef __CHECK_BOX_H__
//#define __CHECK_BOX_H__
//
//#include "Button.h"
//#include "SDL/include/SDL_rect.h"
//
//struct SDL_Texture;
//
//enum class value_state
//{
//	OFF,
//	HOVERING,
//	ON,
//
//	MAX_TYPES
//};
//
//class CheckBox : public Button<void, bool*>
//{
//public:
//	//Constructor
//	CheckBox(void(*action)(bool*), bool* value, fPoint center, SDL_Rect spriteList[3], SDL_Texture* tex, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);
//
//	virtual ~CheckBox();
//
//public:
//	//Enable/Disable
//	virtual void Enable()
//	{
//		status = button_state::IDLE;
//		*sprite = stateSprites[(int)button_state::IDLE];
//	}
//
//	virtual void Disable()
//	{
//		status = button_state::DISABLED;
//		*sprite = stateSprites[(int)button_state::DISABLED];
//	}
//
//protected:
//	virtual button_state CheckCurrentState();
//	virtual button_state ButtonStateEffects();
//
//	virtual void OnIdle();
//	virtual void OnHover();
//	virtual void OnPress();
//
//	virtual void WhileIdle() {}
//
//	virtual void WhileHover() {}
//
//	virtual void WhilePress() {}
//
//private:
//	bool* value;
//	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed
//};
//
//#endif //__ACTION_BOX_H__