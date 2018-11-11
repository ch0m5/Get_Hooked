#ifndef __j1SCENE2_H__
#define __j1SCENE2_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene2 : public j1Module	//IMPROVE: Scene should be a single level-changing module
{
public:

	j1Scene2();

	// Destructor
	virtual ~j1Scene2();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Changes to next scene
	void ChangeScene();
	

public:	// @Carles
	// Inputs
	void CameraInput();
	void AudioInput();

	bool active = false;
	fPoint playerPos;
private:
	fPoint cameraSpeed;	// @Carles


	p2SString map;
	
};

#endif // __j1SCENE_H__