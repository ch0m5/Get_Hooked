#ifndef __j1FADESCENE_H__
#define __j1FADESCENE_H__	// @Carles

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

enum class fade_type
{
	NONE = -1,
	START_GAME,
	MAIN_MENU,
	SETTINGS,
	CREDITS,

	NEXT_LEVEL,
	RESTART_LEVEL,
	RESTART_GAME,

	LEVEL_1,
	LEVEL_2,

	MAX_TYPES
};

enum class fade_step
{
	NONE = -1,
	FADING,
	FULLY_FADED,
	UNFADING
};

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

	bool FadeToBlack(float time = 2.0f, fade_type = fade_type::NONE);

public:
	fade_step GetStep() const {	//IMPROVE: PUT ON CPP
		return step;
	}
	fade_type GetType() const {
		return type;
	}
	fade_type ResetType() {
		type = fade_type::NONE;
		return type;
	}
	float GetDelay() const {
		return delay;
	}

private:
	float delay;
	fade_step step = fade_step::NONE;
	fade_type type;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;

};

#endif //__FACESCENE_H__