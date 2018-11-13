#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

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

	// Changes to next scene
	void ChangeScene();


public:	// @Carles
	// Inputs
	SDL_Rect LimitCameraPos(fPoint playerPos);
	void CameraInput(float dt);
	void AudioInput();
	bool active = true;

	fPoint playerPos;
private:
	fPoint cameraSpeed;	// @Carles
	

	p2SString map;
};

#endif // __j1SCENE_H__