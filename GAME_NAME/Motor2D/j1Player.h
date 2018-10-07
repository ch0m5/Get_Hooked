#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__	// @CarlesHoms

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"

struct SDL_Texture;

class j1Player : public j1Module
{
public:

	j1Player();

	virtual ~j1Player();

	// Called before render is available
	 bool Awake(pugi::xml_node&);

	// Called before the first frame
	 bool Start();

	// Called each loop iteration
	 bool PreUpdate();
	 bool Update(float dt);
	 bool PostUpdate();

	// Called before quitting
	 bool CleanUp();

	// Save and Load
	 bool Load(pugi::xml_node&);
	 bool Save(pugi::xml_node&) const;

private:

	// Character Sprite Sheet
	SDL_Texture* graphics = nullptr;

	// Character animations
	Animation idle;
	//Animation idle_sword;

	//Animation draw_sword;
	//Animation sheathe_sword;

	Animation run;
	//Animation slide;
	//Animation slide_up;
	//Animation crouch;

	Animation jump;
	//Animation somersault;
	Animation fall;

	//Animation attack_1;
	//Animation attack_2;
	//Animation attack_3;
	
	//Animation air_attack_1;
	//Animation air_attack_2;
	//Animation air_attack_3_fall;
	//Animation air_attack_3_end;

	//Animation cast;
	//Animation cast_loop;

	//Animation use_item;

	//Animation corner_grab;
	//Animation corner_climb;
	//Animation ladder_climb;
	//Animation wall_slide;
	//Animation wall_jump;
	
	Animation hurt;
	Animation die;

	// Animation pointers
	Animation* character_animation = nullptr;

	// Character stats
	uint life;
	p2Point<float> position;
	p2Point<float> speed;

	// Character status flags
	bool dead;
	bool godmode;

	// Player rectangles
	SDL_Rect animation_rect;
	SDL_Rect collider_rectt;
	
	//Collider
	//Collider* playerHitbox;

};

#endif //__j1PLAYER_H__