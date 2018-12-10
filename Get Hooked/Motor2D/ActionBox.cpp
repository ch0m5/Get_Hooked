#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "ActionBox.h"
#include "Text.h"

//ActionBox::ActionBox(button_action action,
//	fPoint position,
//	Text* label,
//	p2List<SDL_Rect*> spriteList,
//	scene_type scene,
//	SDL_Texture* tex,
//	Image* parent) : Button(action, position, label, spriteList, image_type::BUTTON_ACTION, tex, parent), sceneChange(scene)
//{
//	this->label = label;
//	this->label->MatchCenter(centerPosition);
//	this->label->RelocatePosByCenter();
//};
//
//ActionBox::~ActionBox()
//{
//	
//}
//
//void ActionBox::OnIdle()
//{
//	animRect = stateSprites[1];
//}
//
//void ActionBox::OnHover()
//{
//	animRect = stateSprites[2];
//}
//
//void ActionBox::OnPress()
//{
//	animRect = stateSprites[3];
//}
//
//void ActionBox::WhileIdle()
//{
//
//}
//
//void ActionBox::WhileHover()
//{
//
//}
//
//void ActionBox::WhilePress()
//{
//
//}
