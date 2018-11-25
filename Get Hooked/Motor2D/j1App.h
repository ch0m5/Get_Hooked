#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1Scene;
class j1Map;
class j1PathFinding;
class j1EntityManager;
class j1Collision;	// @Carles
class j1FadeScene;	// @Carles

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

	// Framerate calculations (returns delayTime)
	void FramerateLogic();
	p2SString DefaultTitle();
	p2SString DebugTitle();

public:
	uint16 GetFrameCap() const {
		return fpsCap;
	}
	uint32 GetCurrentFPS() const {
		return currFPS;
	}
	bool FramerateCapped() const {
		return mustCapFPS;
	}

public:

	// Modules
	j1Window*			win;
	j1Input*			input;
	j1Render*			render;
	j1Textures*			tex;
	j1Audio*			audio;
	j1Scene*			scene;
	j1Map*				map;
	j1PathFinding*		pathfinding;
	j1EntityManager*	entityManager;
	j1Collision*		collision;	// @Carles
	j1FadeScene*		fade;		// @Carles

private:

	p2List<j1Module*>	modules;
	int					argc;
	char**				args;

	p2SString			name;
	p2SString			title;
	p2SString			organization;

	mutable bool		want_to_save;
	bool				want_to_load;
	p2SString			load_game;
	mutable p2SString	save_game;

	p2List<p2SString>	save_list;	// @Carles, list of save file names	//IMPROVE: Make the game work with a list of saved files
	char				save_number;	// @Carles, save file list order counter

	//Framerate
	bool				mustCapFPS;
	j1PerfTimer			perfTimer;
	j1PerfTimer			delayTimer;
	uint64				totalFrameCount = 0;
	j1Timer				gameTimer;
	j1Timer				frameTimer;
	j1Timer				secTimer;
	uint32				currFPS = 0;
	uint32				prevFPS = 0;
	uint16				fpsCap;
	float				dt = 0;

	// Readability variables (not essential, but simplifies reading)
	float avgFPS;
	float gameTime;
	uint32 lastFrameMs;
};

extern j1App* App;

#endif