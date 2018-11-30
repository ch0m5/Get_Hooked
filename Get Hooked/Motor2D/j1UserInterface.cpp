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
	configNode = config.child("userInterface");

	/*if (configNode.empty() == false)
	{
		ret = true;

		p2List_item<UIElement*>* item;
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

	p2List_item<UIElement*>* item;
	p2List_item<UIElement*>* nextItem;

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