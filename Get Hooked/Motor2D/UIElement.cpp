#include "Brofiler/Brofiler.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1UserInterface.h"
#include "UIElement.h"

UIElement::UIElement(ui_type type, fPoint position, bool dynamic, UIElement* parent, p2List<UIElement*>* children) : type(type), position(position), center(position), dynamic(dynamic), parent(parent)
{
	if (children != NULL) {
		this->children = *children;
	}

	if (parent != NULL) {
		parent->AddChild(this);
	}

	Init();
}

UIElement::~UIElement()
{}

bool UIElement::CleanUp()
{
	bool ret = true;

	for (p2List_item<UIElement*>* item = children.start; item != nullptr; item = item->next) {
		item->data->mustDestroy = true;
	}
	children.clear();

	return ret;
}

ui_type UIElement::GetType() const
{
	return type;
}

UIElement* UIElement::GetParent() const
{
	return parent;
}

fPoint UIElement::GetPosition() const
{
	return position;
}

fPoint UIElement::GetCenter() const
{
	return center;
}

fPoint UIElement::MatchPosition(fPoint reference)
{
	position = reference;
	return position;
}

fPoint UIElement::MatchCenter(fPoint reference)
{
	center = reference;
	return center;
}

p2List_item<UIElement*>* UIElement::AddChild(UIElement* child)
{
	return children.add(child);
}

void UIElement::RemoveChild(UIElement* child)
{
	int pos = children.find(child);

	if (pos > -1)
		children.del(children.At(pos));
}