#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include "p2List.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

enum sfx_id_list {	// @Carles: List of sfx as they are put on the sfx p2List
	SFX_WALK = 1,
	SFX_JUMP,
	SFX_SLIDE,
	SFX_HURT
};

struct sfx_file {	// @Carles
	p2SString name;
	uint id;
};

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

public:	// @Carles
	p2SString musicMap1;
	p2SString musicMap2;

	sfx_file runSfx;
	sfx_file jumpSfx;
	sfx_file slideSfx;
	sfx_file hurtSfx;

private:

	void LoadAllMusic(pugi::xml_node&);		// @Carles
	void LoadAllSfx(pugi::xml_node&);		// @Carles

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;

	p2SString musicFolder;	// @Carles
	p2SString sfxFolder;	// @Carles
};

#endif // __j1AUDIO_H__