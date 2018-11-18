#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__	// @CarlesHoms	//IMPROVE: Make better class classification instead of having Entity be this generally big, comments tip possible classification.

#include "p2Animation.h"

enum class collision_type;
struct Collider;
struct SDL_Texture;

//Components
struct movement_flags {	//Dynamic Entity
	bool movingUp;
	bool movingRight;
	bool movingLeft;
	bool movingDown;
};

struct movement_input {	//Dynamic Entity
	bool wantMoveUp;
	bool wantMoveRight;
	bool wantMoveLeft;
	bool wantMoveDown;
};

struct sprite_data {	// @Carles, struct used to store xml data of the first sprite of an animation to then automatize the animation allocation process
	//p2SString id;	//IMPROVE: Make sprite_data lists and use this to mark them
	iPoint sheetPosition;
	SDL_Rect colliderOffset;
	uint numFrames;
	Animation anim;
};

enum class entity_type
{
	NONE = -1,
	NPC,
	PLAYER,
	PLAYER_ATTACK,
	ENEMY,
	ENEMY_ATTACK,
	ITEM,
	CHECKPOINT,

	MAX_TYPES
};

class Entity
{
public:
	//Constructor
	Entity(entity_type type) : type(type) {}

	//Destructor
	virtual ~Entity();

	// Called on entity creation
	virtual void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called between a certain number of frames or times per second
	virtual bool UpdateLogic(float dt) { return true; }

	// Called each frame (framerate dependant)
	virtual bool UpdateTick(float dt) { return true; }

	// Called each loop iteration (graphic)
	virtual bool Update() { return true; }

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp() { return true; }

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	//Entity
	virtual entity_type GetType() const;
	virtual fPoint GetPosition() const;
	virtual fPoint Entity::GetCenterPosition() const;

	//Dynamic Entity
	virtual fPoint GetSpeed() const;
	virtual fPoint GetAcceleration() const;
	//virtual state GetState() const;

	//Physical Entity
	virtual Collider* GetCollider();
	virtual collision_type OnCollision(Collider*, Collider*) { return (collision_type)-1; }

	//Living Entity
	virtual uint GetLife() const;
	virtual void Kill();
	virtual void Hurt();
	virtual bool IsDead() const;

protected:
	//Entity
	virtual void CheckInput() {};
	virtual void Draw(SDL_Rect* animRect) const;
	virtual void ImportSpriteData(const char* spriteName, sprite_data* sprite, pugi::xml_node&);	// Import sprite data from config.xml

	//Dynamic Entity
	virtual void CheckMovement();		// Check player current movement
	virtual void CheckState() {};		// Check player state
	virtual void ApplyState() {};		// Add state effects like movement restrictions, animation and sounds
	virtual void Move(float dt) {};		// Move player position and decide/calculate other movement related factors
	virtual void UpdateHitbox() {};		// Transform player collider depending on new position and state
	virtual bool CheckOrientation(bool orientation);
	virtual fPoint LimitSpeed();

	//Physical Entity
	SDL_Rect ReshapeCollider(sprite_data sprite);

public:
	p2SString name;
	p2SString folder;
	bool active;
	bool mustDestroy = false;

protected:
	//Entity
	fPoint position;
	fPoint centerPosition;
	movement_input input;
	movement_flags movement;

	p2SString textureName;
	SDL_Texture* graphics = nullptr;
	iPoint spriteSize;
	Animation* animPtr = nullptr;
	SDL_Rect animRect;
	//p2List<sprite_data*> spriteList;

	//Dynamic Entity
	fPoint speed;
	fPoint maxSpeed;
	fPoint acceleration;
	float gravity;
	//state status;
	bool lookingRight;	// Flag for blit flipping and hurt speed x direction

	//Physical Entity
	Collider* hitbox;
	SDL_Rect hitboxOffset;

	//Living Entity
	//p2List<Collider*> attackColliders;
	int life;
	uint maxLife;
	fPoint hurtSpeed;
	bool dead;

private:
	entity_type type;
};

#endif //__j1ENTITY_H__