#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include "p2List.h"

#define DEFAULT_MUSIC_FADE_TIME 0.0f	//CHANGE/FIX: Was 2.0f, but mantained the screen loading for too long	// IMPROVE: On xml?

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

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = 0.0f);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

public:	// @Carles
	uint SetMusicVolume() const {
		return Mix_VolumeMusic(masterVolume * (musicVolume * MIX_MAX_VOLUME / 100) / 100);
	}
	uint SetSfxVolume() const {
		return Mix_Volume(-1, masterVolume * (sfxVolume * MIX_MAX_VOLUME / 100) / 100);
	}

public:	// @Carles
	p2SString musicMainMenu;
	p2SString musicMap1;	// IMPROVE: Make list?
	p2SString musicMap2;

	sfx_file runSfx;	// IMPROVE: Make list?
	sfx_file jumpSfx;
	sfx_file slideSfx;
	sfx_file hurtSfx;
	
	ushort masterVolume;	// IMPROVE: Make list?
	ushort musicVolume;
	ushort sfxVolume;

private:

	void LoadAllMusic(pugi::xml_node&);		// @Carles
	void LoadAllSfx(pugi::xml_node&);		// @Carles

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;

	p2SString musicFolder;	// @Carles
	p2SString sfxFolder;	// @Carles
};

#endif // __j1AUDIO_H__