//#include "j1App.h"
//#include "j1Input.h"
//#include "CheckBox.h"
//
//CheckBox::CheckBox(void(*action)(bool*), bool* value, fPoint center, SDL_Rect spriteList[3], SDL_Texture* tex, bool dynamic, UIElement* parent, p2List<UIElement*>* children)
//	: Button<void, bool*>(action, ui_type::BUTTON_CHECK, center, spriteList[0], tex, dynamic, parent, children)
//{
//	stateSprites = new SDL_Rect[3];
//
//	for (int i = 0; i < 3; i++) {
//		stateSprites[i] = spriteList[i];
//	}
//}
//
//CheckBox::~CheckBox()
//{
//	RELEASE(sprite);
//	RELEASE_ARRAY(stateSprites);
//}
//
//button_state CheckBox::CheckCurrentState()
//{
//	switch (status) {
//	case button_state::IDLE:
//		if (MouseOnImage() == true) {
//			OnHover();
//			status = button_state::HOVERING;
//		}
//		break;
//	case button_state::HOVERING:
//		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
//			OnPress();
//		}
//		else if (MouseOnImage() == false) {
//			OnIdle();
//			status = button_state::IDLE;
//		}
//		break;
//	}
//
//	return status;
//}
//
//button_state CheckBox::ButtonStateEffects()
//{
//	switch (status) {
//	case button_state::IDLE:
//		WhileIdle();
//		break;
//	case button_state::HOVERING:
//		WhileHover();
//		break;
//	}
//
//	return status;
//}
//
//void CheckBox::OnIdle()
//{
//	if (*value == true) {
//		*sprite = stateSprites[(int)value_state::ON];
//	}
//	else {
//		*sprite = stateSprites[(int)value_state::OFF];
//	}
//}
//
//void CheckBox::OnHover()
//{
//	*sprite = stateSprites[(int)value_state::HOVERING];
//}
//
//void CheckBox::OnPress()
//{
//	DoAction(value);
//	if (*value == true) {
//		*sprite = stateSprites[(int)value_state::ON];
//	}
//	else {
//		*sprite = stateSprites[(int)value_state::OFF];
//	}
//}