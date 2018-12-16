#ifndef __PLAYER_H__
#define __PLAYER_H__	// @CarlesHoms

#include "Creature.h"

//Components
enum class player_state {	// @Carles, enum that groups all possible player states that will decide how the player behaves
	IDLE,
	CROUCHING,
	RUNNING,
	JUMPING,
	FALLING,
	SLIDING,
	HURT,
	ATTACKING,
	//HOOK
};

struct attack_data {	//IMPROVE: Make function that automatically "flips" the collider based on the animation sprite data (x,y,w,h)
	SDL_Rect offsetRight;
	SDL_Rect offsetLeft;
	ushort startAttackFrame;
	ushort finishAttackFrame;
};

class Player : public Creature
{
public:

	Player();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();
	bool LoadStart();

	// Called each loop iteration
	bool PreUpdate();
	//bool UpdateLogic(float dt);
	bool UpdateTick(float dt);
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Called when colliding
	collision_type OnCollision(Collider* c1, Collider* c2);	// @Carles
	collision_type WallCollision(Collider* c1, Collider* c2);

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	bool CameraFree() const {	//IMPROVE: MOVE TO CPP
		return freeCamera;
	}
	bool IsGod() const {
		return godMode;
	}
	uint LifeToStart() {
		life = startLife;
		return life;
	}
	iPoint GetActivationRadius() const {
		return activationRadius;
	}
	void ReturnToSpawn() {
		position = lastGroundPosition = spawnPosition;
	}
	void SetSpawn(fPoint spawn) {
		spawnPosition = spawn;
	}
	void DeadReset() {
		LifeToStart();
		deadSprite.anim.Reset();	//IMPROVE: Check if really needed here
		playedHurtSfx = false;
	}
	void AddScore(int points) {
		playerScore += points;
	}
	void ResetScore() {
		playerScore = 0;
	}

private:	// @Carles
	void ImportAttackData(const char* spriteName, attack_data* attack, pugi::xml_node& first_sprite);
	void ImportAllStates(pugi::xml_node&);													// Import all state data from config.xml
	void ImportAllSprites(pugi::xml_node&);													// Import all sprite data using the above function for each animation
	void AllocAllAnimations();																// Allocate all animations with previously recieved sprite data
	
	Collider* CreateAttackCollider(attack_data attack);
	SDL_Rect ReshapeAttackCollider(attack_data attack);

	// Player actions
	void Jump();			// Add Y speed and jump state when requested
	void Fall(float dt);	// Add acceleration to Y speed
	void LateralStop();		// Stop X speed
	void Land();			// Stop Y speed
	void StandUp();			// Return to normal acceleration and reset slide values
	void Hurt();			// Stop and move slightly up and opposite of current direction, player state changes to HURT
	void PlayerReset();
	void DeathByPit();
	//void Hook();

	// Debug update
	void DebugInput();

	// Player update
	void CheckInput();		// Check player input
	//void CheckMovement();	// Check player current movement
	void CheckState();		// Check player state
	void ApplyState();		// Add state effects like movement restrictions, animation and sounds
	void Move(float dt);	// Move player position and decide/calculate other movement related factors
	void UpdateHitbox();	// Transform player collider depending on new position and state

	// Check possible new states in each state and other changes in the player's status
	player_state IdleMoveCheck();
	player_state CrouchingMoveCheck();
	player_state RunningMoveCheck();
	player_state JumpingMoveCheck();
	player_state FallingMoveCheck();
	player_state SlidingMoveCheck();
	player_state HurtMoveCheck();
	player_state AttackMoveCheck();
	
	// Apply effects of each state
	void IdleEffects();
	void CrouchingEffects();
	void RunningEffects();
	void JumpingEffects();
	void FallingEffects();
	void SlidingEffects();
	void HurtEffects();
	void DeadEffects();
	void AttackEffects();

	// Movement and final changes
	fPoint GodModeMovement(float dt);
	fPoint NormalMovement(float dt);

public:
	int timesDead = 0;	//ALERT_CARLES: ADD TO SAVE LOAD

private:
	int playerScore = 0;
	uint startLife;
	player_state status;
	Collider* attackCollider = nullptr;
	SDL_Rect attackOffset;
	bool wantAttack = false;
	bool attackColliderCreated = false;
	bool weaponDrawn;	// CHANGE/FIX: Put to config & save_game when implemented

	fPoint lastGroundPosition;

	float normalAcceleration;
	float slideAcceleration;

	// Radius for player surround processing
	iPoint activationRadius;

	// Character status flags and directly related data
	bool airborne;			// Flag to mark if player is on air (not colliding with anything)
	bool somersaultUsed;	// Flag for somersault usage

	uint attackFinalFrame;	// To stop attack state and animation
	
	uint hurtTimer = 0;		//IMPROVE: Should be j1Timers instead of raw values
	uint hurtDelay;
	uint deadTimer = 0;		// Timer used for player death
	ushort deathDelay;		// Time delay between death and start FadeToBlack

	bool mustReset = false;		// Flag used to restart animations when needed (skipping workflow steps)	//CHANGE/FIX: This for more entities?
	bool godMode;				// Flag used to mark player invencibility
	bool freeCamera;			// Flag used to mark free camera movement

	//Character sprites	//IMPROVE: Make list?
	sprite_data idleSprite;

	sprite_data runSprite;
	sprite_data slideSprite;
	sprite_data crouchSprite;

	sprite_data jumpSprite;
	sprite_data somersaultSprite;
	sprite_data fallSprite;

	sprite_data hurtSprite;
	sprite_data deadSprite;

	sprite_data attack1Sprite;
	attack_data attack1Data;

	// Audio
	uint runSfxTimer = 0;	// Timer to mark time between run sounds
	int runSfxDelay;		// Time between run sounds
	bool playedSlideSfx;	// Flag to mark slide sfx played
	bool playedHurtSfx;		// Flag to mark hurt sfx played
};

#endif //__PLAYER_H__

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

	//Animation attack_1;	//FIRST ATTACK DONE
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