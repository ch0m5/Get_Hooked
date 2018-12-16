#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"

class Enemy;
class UIElement;
struct SDL_Texture;

enum class scene_type {	//@Carles
	NONE = -1,
	MAIN_MENU,
	SETTINGS,
	CREDITS,

	LEVEL_1,
	LEVEL_2,

	MAX_SCENES
};
								//CHANGE/FIX: Scene, both 1 and 2, have a very buggy functionality and need reworking, also reduced to a single module
class j1Scene : public j1Module	//CHANGE/FIX: Remake maps 1 and 2, implement correspondant debug functionalities correctly, and if enough time make maps work on the same scene
{
public:
	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each frame (logic)
	bool UpdateTick(float dt);

	// Called each loop iteration (graphic)
	bool Update();

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	//Spawns	CHANGE/FIX: This should probably be on the map module and managed with Tiled data, not congif.xml reading
	bool SpawnEntities(scene_type level, pugi::xml_node&);
	bool SpawnLevelEntities(pugi::xml_node& entities, pugi::xml_node& spawns);

	// Scene changes
	void NextLevel();
	void RestartLevel();
	void ChangeScene(scene_type scene);

	// Inputs
	void DebugInput();
	void AudioInput();
	void CameraInput(float dt);
	SDL_Rect LimitCameraPos(fPoint playerPos);
	
	void HealthToUI(int life);
	void HurtUI();

private:
	void SetupLevel(pugi::xml_node& config);
	void RegisterButtonData(pugi::xml_node&, SDL_Rect* button);

public:	// CHANGE/FIX: Check the need for public
	bool debugMode;		// Flag that marks if debug functionalities are available
	bool gamePaused = false;
	scene_type scene;
	fPoint playerStart;
	fPoint playerFinish;

	UIElement* settingsWindow;
	UIElement* health[3];
	UIElement* timer;
	UIElement* score;
	int playerLife;

private:
	bool loading = false;
	bool saveFix = false;			//CHANGE/FIX: Temporary fix for a strange Save/Load bug
	fPoint cameraSpeed;
	p2List<p2SString> maps;	//CHANGE/FIX: Should be in module map maybe?

	//Default UI data list
	SDL_Rect panel;
	SDL_Rect window;
	SDL_Rect* button;
	SDL_Rect* checkButton;

	//Specific UI Data
	SDL_Texture* backgroundTexPtr;	//CHANGE/FIX: Gotta reach those requirements, but this shouldn't just be lying here
	p2SString menuBackgroundTex;
	SDL_Rect* exit;
	SDL_Rect* shutDown;
	SDL_Rect* settings;
	SDL_Rect* back;
	SDL_Rect healthBar;
	SDL_Rect healthChunck;
};

#endif // __j1SCENE_H__