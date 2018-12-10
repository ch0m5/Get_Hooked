#include "Brofiler/Brofiler.h"

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"

#include "j1UserInterface.h"
#include "Image.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"
#include "Window.h"

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

	p2List_item<Image*>* item;

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

	p2List_item<Image*>* item;

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

	p2List_item<Image*>* item;
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

	p2List_item<Image*>* item;
	for (item = screenElements.start; item != nullptr && ret == true; item = item->next)
	{
		if (item->data->active == false) {
			continue;
		}

		if (ret)
			ret = item->data->Update();	//Update independant of framerate
	}

	return true;
}

// Called each loop iteration
bool j1UserInterface::PostUpdate()
{
	BROFILER_CATEGORY("Module UserInterface PostUpdate", Profiler::Color::LightPink);

	bool ret = true;

	p2List_item<Image*>* item;
	p2List_item<Image*>* nextItem;

	for (item = screenElements.start; item != NULL && ret == true; item = nextItem)
	{
		nextItem = item->next;
		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1UserInterface::CleanUp()
{
	bool ret = true;

	p2List_item<Image*>* item;
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

void j1UserInterface::AddElement(Image* element)
{
	screenElements.add(element);
}

void j1UserInterface::DestroyElement(Image* element)
{
	int pos = screenElements.find(element);
	
	if (pos > -1) {
		p2List_item<Image*>* item = screenElements.At(pos);
		RELEASE(item->data);
		screenElements.del(item);
	}
}

Image* j1UserInterface::CreateText(fPoint position, const char* content, SDL_Color color, _TTF_Font* font, Image* parent)
{
	Image* ret = nullptr;

	ret = new Image(image_type::TEXT, position, NULL, NULL, &Text(content, color, font, { 0, 0 }, parent), NULL);
	AddElement(ret);

	return ret;
}

Image* j1UserInterface::CreateImage(fPoint position, SDL_Rect* texRect, SDL_Texture* tex, Text* label, Image* parent)
{
	Image* ret = nullptr;

	if (texRect == NULL) {
		SDL_Rect tmpRect = { 0, 0, 0, 0 };
		texRect = &tmpRect;

		if (tex != NULL) {
			App->tex->GetSize(tex, (uint&)texRect->w, (uint&)texRect->h);
		}
		else if (label != NULL) {
			tex = App->font->Print(label->GetText(), label->GetColor(), label->GetFont());
			App->tex->GetSize(tex, (uint&)texRect->w, (uint&)texRect->h);
			label = NULL;	//@Carles: If text forms the image, the image IS the label, therefore it has no label property
		}
	}
	else if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Image(image_type::IMAGE, position, texRect, tex, label, parent);
	AddElement(ret);

	/*if (label != NULL) {
		AddElement(label);
	}*/

	return ret;
}

//Image* j1UserInterface::CreateWindowBox(fPoint position, iPoint size, SDL_Rect texRect, p2List<Image*> elemList, SDL_Texture* tex, Image* parent)
//{
//	Image* ret = nullptr;
//
//	ret = new Window(position, size, texRect, elemList, tex, parent);
//	screenElements.add(ret);
//
//	return ret;
//}
//
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