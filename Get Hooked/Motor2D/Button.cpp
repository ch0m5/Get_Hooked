#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1UserInterface.h"
#include "Button.h"
#include "Text.h"
#include "Image.h"

Button::Button(button_action action,
	fPoint position,
	SDL_Rect texRect,
	const char* string,
	ui_element_type type,
	SDL_Texture* tex) : UIElement(type, position), action(action), texRect(texRect)
{
	size = { texRect.w, texRect.h };

	if (tex = NULL)
		graphics = App->ui->GetAtlas();
	else
		graphics = tex;

	image = new Image(position, texRect, graphics);
	idleText = new Text(string, position);

	idleText->MatchCenter(centerPosition);
	idleText->RelocatePosByCenter();
};

Button::~Button()
{
	RELEASE(idleText);
	RELEASE(image);
}

bool Button::MouseOnButton()
{
	iPoint mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);

	return !(position.y + size.y < mousePos.y || position.y > mousePos.y || position.x + size.x < mousePos.x || position.x > mousePos.x);
}

button_state Button::CheckCurrentState()
{
	switch (status) {
	case button_state::IDLE:
		break;
	case button_state::HOVERING:
		break;
	case button_state::PRESSING:
		break;
	default:
		break;
	}

	return status;
}

button_state Button::ButtonStateEffects()
{
	return status;
}

void Button::OnIdle()
{

}

void Button::OnHover()
{

}

void Button::OnPress()
{

}

void Button::WhileIdle()
{

}

void Button::WhileHover()
{

}

void Button::WhilePress()
{

}

void Button::Enable()
{
	status = button_state::IDLE;
}

void Button::Disable()
{
	status = button_state::DISABLED;
	//Add more?
}