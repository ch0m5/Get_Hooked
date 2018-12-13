#include "Brofiler/Brofiler.h"

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"

#include "j1UserInterface.h"
#include "UIElement.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"
#include "ActionBox.h"

j1UserInterface::j1UserInterface() : j1Module()
{
	name.create("userInterface");
}

// Destructor
j1UserInterface::~j1UserInterface()
{}

// Called before render is available
bool j1UserInterface::Awake(pugi::xml_node& config)
{
	LOG("Loading UI atlas");
	bool ret = true;

	atlas_file_name = config.child("atlas").attribute("file").as_string("");
	configNode = config.child("screenElements");

	/*if (configNode.empty() == false)
	{
		ret = true;

		p2List_item<Image*>* item;
		item = screenElements.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->Awake(entitiesNode.child(item->data->name.GetString()));
			item = item->next;
		}
	}*/

	return ret;
}

// Called before the first frame
bool j1UserInterface::Start()
{
	bool ret = true;

	atlas = App->tex->Load(atlas_file_name.GetString());

	p2List_item<UIElement*>* item;

	for (item = screenElements.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->Start();
	}

	return ret;
}

// Called each loop iteration
bool j1UserInterface::PreUpdate()
{
	BROFILER_CATEGORY("Module UserInterface PreUpdate", Profiler::Color::Pink);

	bool ret = true;

	p2List_item<UIElement*>* item;

	for (item = screenElements.start; item != NULL && ret == true; item = item->next)
	{
		if (item->data->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Called each frame (logic)
bool j1UserInterface::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Module UserInterface UpdateTick", Profiler::Color::DeepPink);
	
	bool ret = true;

	p2List_item<UIElement*>* item;
	for (item = screenElements.start; item != nullptr && ret == true; item = item->next)
	{
		if (item->data->active == false) {
			continue;
		}

		if (ret)
			ret = item->data->UpdateTick(dt);	//Update dependant of framerate
	}

	return true;
}

// Called each frame (graphic)
bool j1UserInterface::Draw()
{
	BROFILER_CATEGORY("Module UserInterface Update", Profiler::Color::HotPink);

	bool ret = true;

	p2List_item<UIElement*>* item;
	for (item = screenElements.start; item != nullptr && ret == true; item = item->next)
	{
		if (item->data->active == false || item->data->GetParent() != NULL) {	// All elements are listed, but the parent handles the drawing for layer purposes
			continue;
		}

		if (ret)
			ret = item->data->Update();	//Update independant of framerate
	}

	if (mustDebugDraw) {
		p2List_item<UIElement*>* item;
		for (item = screenElements.start; item != nullptr && ret == true; item = item->next)
		{
			if (item->data->active == false) {	// All elements must have debug blitting AFTER standard blitting, no "blitting layers"
				continue;
			}

			if (ret)
				ret = item->data->DebugDraw();	//Update independant of framerate
		}
	}

	return true;
}

// Called each loop iteration
bool j1UserInterface::PostUpdate()
{
	BROFILER_CATEGORY("Module UserInterface PostUpdate", Profiler::Color::LightPink);

	bool ret = true;

	p2List_item<UIElement*>* item;
	p2List_item<UIElement*>* nextItem;

	for (item = screenElements.start; item != NULL && ret == true; item = nextItem)
	{
		nextItem = item->next;

		if (item->data->mustDestroy) {
			item->data->CleanUp();
			DestroyElement(item);
		}
		else {
			ret = item->data->PostUpdate();
		}
	}

	return ret;
}

// Called before quitting
bool j1UserInterface::CleanUp()
{
	bool ret = true;

	p2List_item<UIElement*>* item;
	for (item = screenElements.end; item != NULL && ret == true; item = item->prev)
	{
		ret = item->data->CleanUp();
		RELEASE(item->data);
	}
	screenElements.clear();

	return ret;
}

// const getter for atlas
SDL_Texture* j1UserInterface::GetAtlas() const
{
	return atlas;
}

void j1UserInterface::AddElement(UIElement* element)
{
	screenElements.add(element);
}

void j1UserInterface::DestroyElement(p2List_item<UIElement*>* element)
{
	RELEASE(element->data);
	screenElements.del(element);
}

UIElement* j1UserInterface::CreateImage(fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic, UIElement* parent, p2List<UIElement*>* children)
{
	Image* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}
	else if (SDL_RectEmpty(&texRect)) {
		SDL_Rect tmpRect = { 0, 0, 0, 0 };
		App->tex->GetSize(tex, (uint&)tmpRect.w, (uint&)tmpRect.h);
		texRect = tmpRect;
	}
	
	ret = new Image(ui_type::IMAGE, center, texRect, tex, dynamic, parent, children);
	AddElement((UIElement*)ret);

	return (UIElement*)ret;
}

UIElement* j1UserInterface::CreateText(fPoint center, const char* content, SDL_Color color, _TTF_Font* font, bool dynamic, UIElement* parent, p2List<UIElement*>* children)
{
	Text* ret = nullptr;

	ret = new Text(content, color, font, center, dynamic, parent, children);
	AddElement((UIElement*)ret);

	return (UIElement*)ret;
}

UIElement* j1UserInterface::CreateActionBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, bool dynamic, UIElement* parent, p2List<UIElement*>* children)
{
	UIElement* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new ActionBox<void>(action, center, spriteList, tex, dynamic, parent, children);
	AddElement(ret);

	return ret;
}

//void(*action)(...), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL

//Window* j1UserInterface::CreateWindowBox(fPoint center, p2List<Image*> children, SDL_Rect* texRect, SDL_Texture* tex, Text* label, Image* parent)
//{
//	Window* ret = nullptr;
//
//	ret = new Window(center, children, texRect, tex, label, parent);
//	AddElement((Image*)ret);
//
//	return ret;
//}

//Image* j1UserInterface::CreateActionBox(buttonAction action, fPoint position, Text* label, SDL_Rect* spriteList[4], image_type type, SDL_Texture* tex, Image* parent)
//{
//	Image* ret = nullptr;
//
//	ret = new ActionBox(position, texRect, tex, parent);
//	screenElements.add(ret);
//
//	return ret;
//}
//
//Image* j1UserInterface::CreateCheckBox()
//{
//	Image* ret = nullptr;
//
//	ret = new Image(position, texRect, tex, parent);
//	screenElements.add(ret);
//
//	return ret;
//}
//
//Image* j1UserInterface::CreateInputText()
//{
//	Image* ret = nullptr;
//
//	ret = new Image(position, texRect, tex, parent);
//	screenElements.add(ret);
//
//	return ret;
//}