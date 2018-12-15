#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Audio.h"
#include "p2List.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name.create("audio");
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	// Load volumes
	masterVolume = config.attribute("volume").as_uint();
	musicVolume = config.child("music").attribute("volume").as_uint();
	sfxVolume = config.child("sfx").attribute("volume").as_uint();

	// Load all audio files' data (names mainly)
	LoadAllMusic(config);
	LoadAllSfx(config);

	// Set channel volume
	SetSfxVolume();

	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	p2List_item<Mix_Chunk*>* item;
	for(item = fx.start; item != NULL; item = item->next)
		Mix_FreeChunk(item->data);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Load
bool j1Audio::Load(pugi::xml_node &data)
{
	pugi::xml_node tmpNode;

	masterVolume = data.attribute("volume").as_uint();
	musicVolume = data.child("music").attribute("volume").as_uint();
	sfxVolume = data.child("sfx").attribute("volume").as_uint();

	return true;
}

// Save
bool j1Audio::Save(pugi::xml_node &data) const
{
	pugi::xml_node tmpNode;

	data.append_attribute("volume") = masterVolume;

	tmpNode = data.append_child("music");
	tmpNode.append_attribute("volume") = musicVolume;
	
	tmpNode = data.append_child("sfx");
	tmpNode.append_attribute("volume") = sfxVolume;

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.add(chunk);
		ret = fx.count();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if(!active)
		return false;

	if(id > 0 && id <= fx.count())
	{
		Mix_PlayChannel(-1, fx[id - 1], repeat);
	}

	return ret;
}

//--------------------------------

void j1Audio::LoadAllMusic(pugi::xml_node& config) {	// @Carles
	musicFolder.create(config.child("music").child("folder").child_value());

	musicMainMenu.create("%s%s", musicFolder.GetString(), config.child("music").child("menus").child("mainMenu").child_value());
	musicMap1.create("%s%s", musicFolder.GetString(), config.child("music").child("maps").child("track1").child_value());
	musicMap2.create("%s%s", musicFolder.GetString(), config.child("music").child("maps").child("track2").child_value());
}

void j1Audio::LoadAllSfx(pugi::xml_node& config) {	// @Carles
	sfxFolder.create(config.child("sfx").child("folder").child_value());

	runSfx.name.create("%s%s", sfxFolder.GetString(), config.child("sfx").child("player").child("run").child_value());
	jumpSfx.name.create("%s%s", sfxFolder.GetString(), config.child("sfx").child("player").child("jump").child_value());
	slideSfx.name.create("%s%s", sfxFolder.GetString(), config.child("sfx").child("player").child("slide").child_value());
	hurtSfx.name.create("%s%s", sfxFolder.GetString(), config.child("sfx").child("player").child("hurt").child_value());

	LoadFx(runSfx.name.GetString());
	runSfx.id = SFX_WALK;

	LoadFx(jumpSfx.name.GetString());
	jumpSfx.id = SFX_JUMP;

	LoadFx(slideSfx.name.GetString());
	slideSfx.id = SFX_SLIDE;

	LoadFx(hurtSfx.name.GetString());
	hurtSfx.id = SFX_HURT;
}