#ifndef __j1FADESCENE_H__
#define __j1FADESCENE_H__	// @Carles

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

class j1FadeScene : public j1Module	//CHANGE/FIX: FadeToBlack has parameters that do nothing, needs rework
{
public:
	j1FadeScene();

	// Destructor
	virtual ~j1FadeScene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration (graphic)
	bool Update();

	bool FadeToBlack(j1Module* map_off, j1Module* map_on, float time = 2.0f);

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;

	j1Module* fade_out = nullptr;
	j1Module* fade_in = nullptr;

};

#endif //__FACESCENE_H__