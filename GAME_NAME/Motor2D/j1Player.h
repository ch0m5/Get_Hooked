#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__	// @CarlesHoms

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"

struct SDL_Texture;

class j1Player : public j1Module	// CHANGE/FIX: Check useless methods (empty)
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

	// Called before quitting
	bool CleanUp();

	// Called when colliding
	void OnCollision(Collider* c1, Collider* c2);	// @Carles

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:	// @Carles
	void PlayerInput();
	void AllocAllAnimations();

private:
	p2SString folder;
	//p2SString characterSheet;


private:
	// Character stats
	uint life;
	fPoint position;			//CHANGE/FIX: Simple type values must be stored on the save_game and the config xml file
	fPoint speed;			// "" ""
	fPoint maxSpeed;			// "" ""
	float acceleration;
	float jumpVelocity;
	float gravity;

	// Character status flags
	bool dead;					// "" ""
	bool godmode;				// "" ""

	// Character Sprite Sheet
	SDL_Texture* graphics = nullptr;	// Pointer to character sheet texture
	p2SString characterSheet;
	iPoint spriteSize;			// Size of each sprite

	// Character animations
	Animation idle;

	Animation run;

	Animation jump;
	Animation fall;

	Animation hurt;
	Animation die;

	float defaultAnimSpeed;

	// Animation pointers
	Animation* anim = nullptr;

	// Player rectangles
	SDL_Rect rectAnim;
	SDL_Rect rectCollider;
	
	//Collider
	//Collider* playerHitbox = nullptr;
};

#endif //__j1PLAYER_H__

// All posible character animations for future use
	//Animation idle;
	//Animation idle_sword;

	//Animation draw_sword;
	//Animation sheathe_sword;

	//Animation run;
	//Animation slide;
	//Animation slide_up;
	//Animation crouch;

	//Animation jump;
	//Animation somersault;
	//Animation fall;

	//Animation attack_1;
	//Animation attack_2;
	//Animation attack_3;

	//Animation air_attack_1;
	//Animation air_attack_2;
	//Animation air_attack_3;
	//Animation air_attack_fall;
	//Animation air_attack_3_end;

	//Animation cast;
	//Animation cast_loop;

	//Animation use_item;

	//Animation corner_grab;
	//Animation corner_climb;
	//Animation ladder_climb;
	//Animation wall_slide;
	//Animation wall_jump;

	//Animation hurt;
	//Animation die;

// 2nd list for definitions
	//idle;
	//idle_sword;

	//draw_sword;
	//sheathe_sword;

	//run;
	//slide;
	//slide_up;
	//crouch;

	//jump;
	//somersault;
	//fall;

	//attack_1;
	//attack_2;
	//attack_3;

	//air_attack_1;
	//air_attack_2;
	//air_attack_3;
	//air_attack_3_fall;
	//air_attack_3_end;

	//cast;
	//cast_loop;

	//use_item;

	//corner_grab;
	//corner_climb;
	//ladder_climb;
	//wall_slide;
	//wall_jump;

	//hurt;
	//die;