#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__	// @CarlesHoms

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"

struct SDL_Texture;

struct player_sprite {	// @Carles
	iPoint position;
	float animSpeed;
	uint frames;
	bool loop;
};

enum class player_state {	// @Carles
	IDLE,
	CROUCHING,
	RUNNING,
	AIRBORNE,
	SLIDING,
	//HOOK,
	HURT,
	DEAD
};

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

private:	// @Carles
	//void MovePlayerOrig();
	void ImportSpriteData(const char* spriteName, player_sprite* sprite, pugi::xml_node&);
	void ImportAllSprites(pugi::xml_node&);
	void AllocAllAnimations();
	
	// Player functions
	void Jump();		// Add Y speed when jump requested
	void Fall();		// Add acceleration to Y speed
	void Land();		// Stop Y speed
	//void Hook();

	// Player update
	void PlayerInput();		// Check player input
	void PlayerMovement();	// Check player current movement
	void PlayerState();		// Check player state
	void PlayerEffects();	// Add state effects like movement restrictions, animation and sounds
	void MovePlayer();		// Move player position and calculate other movement related factors

	// Check possible new states in each state
	void idleMoveCheck();
	void crouchingMoveCheck();
	void runningMoveCheck();
	void airMoveCheck();
	void slidingMoveCheck();
	//void hookMoveCheck();

private:
	p2SString folder;

	// Character stats
	uint life;
	fPoint position;
	fPoint speed;
	fPoint maxSpeed;
	float currentAcceleration;
	float normalAcceleration;
	float slideAcceleration;
	float jumpVelocity;
	float gravity;

	// Character status flags
	bool wantMoveUp = false;	// CHANGE/FIX: Hardcoded
	bool wantMoveRight = false;
	bool wantMoveLeft = false;
	bool wantMoveDown = false;
	
	bool movingUp = false;	// CHANGE/FIX: Hardcoded
	bool movingRight = false;
	bool movingLeft = false;
	bool movingDown = false;

	bool lookingRight = true;	// CHANGE/FIX: Hardcoded should be in xml or eliminated
	bool somersaultUsed = false;	// CHANGE/FIX: Hardcoded, needs to go to config and save xmls
	//bool hurt = false;	// CHANGE/FIX: HARDCODED
	//bool dead = false;
	bool godmode;
	player_state state;

	// Character Sprite Sheet
	SDL_Texture* graphics = nullptr;
	p2SString characterSheet;
	iPoint spriteSize;
	uint scale;

	//Character sprites
	player_sprite idleSprite;

	player_sprite runSprite;
	player_sprite slideSprite;
	player_sprite crouchSprite;

	player_sprite jumpSprite;
	player_sprite somersaultSprite;
	player_sprite fallSprite;

	player_sprite hurtSprite;
	player_sprite deadSprite;

	// Character animations
	Animation idleAnim;

	Animation runAnim;
	Animation slideAnim;
	Animation crouchAnim;

	Animation jumpAnim;
	Animation somersaultAnim;
	Animation fallAnim;

	Animation hurtAnim;
	Animation deadAnim;

	// Animation pointers
	Animation* animPtr = nullptr;

	// Audio
	int runSfxTimer = 0;
	bool playedSlideSfx = false;	//CHANGE/FIX: Hardcoded

	// Player rectangles
	SDL_Rect animRect;
	SDL_Rect colliderRect;
	
	//Collider
	Collider* hitbox = nullptr;
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