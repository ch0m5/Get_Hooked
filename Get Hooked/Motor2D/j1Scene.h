#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

class Enemy;
struct SDL_Texture;

enum class scene_type {	//@Carles
	NONE = -1,
	MAIN_MENU,
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

public:
	// Changes to next scene
	void NextLevel();
	void RestartLevel();
	void RestartGame();

public:	// CHANGE/FIX: Check the need for public
	// Inputs
	SDL_Rect LimitCameraPos(fPoint playerPos);
	void CameraInput(float dt);
	void AudioInput();
	scene_type scene;
	fPoint playerStart;
	fPoint playerFinish;

	// Sam
	bool active = true;
	fPoint playerPos;

private:
	fPoint cameraSpeed;
	p2List<p2SString> maps;	//CHANGE/FIX: Should be in module map maybe?

	//Sam
	p2SString map;
};

#endif // __j1SCENE_H__