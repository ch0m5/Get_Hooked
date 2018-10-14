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
	//HOOK
};

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

	// Called before quitting
	bool CleanUp();

	// Called when colliding
	void OnCollision(Collider* c1, Collider* c2);	// @Carles

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	bool IsDead() {
		return dead;
	}

private:	// @Carles
	//void MovePlayerOrig();
	void ImportSpriteData(const char* spriteName, player_sprite* sprite, pugi::xml_node&);
	void ImportAllSprites(pugi::xml_node&);
	void ImportAllStates(pugi::xml_node&);
	void AllocAllAnimations();
	
	// Player functions
	void Jump();		// Add Y speed when jump requested
	void Fall();		// Add acceleration to Y speed
	void Land();		// Stop Y speed
	void Hurt();		// Stop and move slightly up and opposite of current direction
	//void Hook();

	// Player update
	void PlayerInput();		// Check player input
	void PlayerMovement();	// Check player current movement
	void PlayerState();		// Check player state
	void PlayerEffects();	// Add state effects like movement restrictions, animation and sounds
	void MovePlayer();		// Move player position and calculate other movement related factors

	// Check possible new states in each state
	void IdleMoveCheck();
	void CrouchingMoveCheck();
	void RunningMoveCheck();
	void AirMoveCheck();
	void SlidingMoveCheck();
	void HurtMoveCheck();
	
	// Apply effects of each state
	void IdleEffects();
	void CrouchingEffects();
	void RunningEffects();
	void AirEffects();
	void SlidingEffects();
	void HurtEffects();
	void DeadEffects();

private:
	p2SString folder;

	// Character stats
	uint life;
	uint maxLife;
	fPoint position;
	fPoint speed;
	fPoint maxSpeed;
	fPoint hurtSpeed;
	float currentAcceleration;
	float normalAcceleration;
	float slideAcceleration;
	float jumpVelocity;
	float gravity;

	// Character status flags
	bool wantMoveUp;
	bool wantMoveRight;
	bool wantMoveLeft;
	bool wantMoveDown;
	
	bool movingUp;
	bool movingRight;
	bool movingLeft;
	bool movingDown;

	bool lookingRight;
	bool somersaultUsed;
	bool hurt;
	bool dead;
	int deadTimer = 0;
	int deathDelay;
	bool fading;
	int fadeDelay;
	bool playerReset;
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
	int runSfxDelay;
	bool playedSlideSfx;
	bool playedHurtSfx;

	// Player rectangles
	SDL_Rect animRect;		//SamAlert: For animation blit
	SDL_Rect colliderRect;	//SamAlert: For collider position, should(?) be based on animRect's position
	
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