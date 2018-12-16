#include "Brofiler/Brofiler.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "Item.h"

// Called before render is available
bool Item::Awake(pugi::xml_node& entities)
{
	bool ret = true;

	LOG("Loading Enemy Data");

	folder.create(entities.child("folder").child_value());

	// Character stats and flags
	ImportAllStates(entities.child("stats"));	// Import all state data from config.xml

	// Sprites
	textureName.create("%s%s", folder.GetString(), entities.child("sprites").child("spriteSheet").child_value());
	spriteSize = { entities.child("sprites").child("spriteSize").attribute("x").as_int(), entities.child("sprites").child("spriteSize").attribute("y").as_int() };

	pugi::xml_node first_sprite = entities.child("sprites").child("first_sprite");
	ImportAllSprites(first_sprite);	// Import all sprite data

	// Animations
	AllocAllAnimations();	// Allocate all animations with previously recieved sprite data

	return ret;
}

bool Item::Start()
{
	bool ret = true;

	graphics = App->tex->Load(textureName.GetString());

	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_ENEMY, this);
	hitboxOffset = hitbox->rect;

	return ret;
}

// Called each loop iteration
bool Item::PreUpdate()
{
	BROFILER_CATEGORY("Entity Item PreUpdate", Profiler::Color::IndianRed);

	bool ret = true;

	return ret;
}

// Called each frame (framerate dependant)
bool Item::UpdateTick(float dt)
{
	BROFILER_CATEGORY("Entity Item UpdateTick", Profiler::Color::Red);

	bool ret = true;

	animRect = animPtr->AdvanceAnimation(dt);

	return ret;
}

// Called each loop iteration
bool Item::Update()
{
	BROFILER_CATEGORY("Entity Item Update", Profiler::Color::DarkRed);

	bool ret = true;

	Draw(&animRect);

	return ret;
}

// Called each loop iteration
bool Item::PostUpdate()
{
	BROFILER_CATEGORY("Entity Item PostUpdate", Profiler::Color::OrangeRed);

	bool ret = true;

	return ret;
}

bool Item::CleanUp()
{
	bool ret = true;

	LOG("Unloading item");

	App->tex->UnLoad(graphics);
	graphics = nullptr;

	if (hitbox != nullptr) {
		hitbox->to_delete = true;	// @Carles, mark collider for destruction
		hitbox = nullptr;
	}

	return ret;
}

// Save and Load
bool Item::Load(pugi::xml_node &entities)
{
	//TODO: Load items data of current alive items (enemyType, state, position, speed)
	return true;
}

bool Item::Save(pugi::xml_node &entities) const
{
	//TODO: Save item data of current alive items (enemyType, state, position, speed)
	return true;
}

// Called when colliding
collision_type Item::OnCollision(Collider* c1, Collider* c2)
{
	collision_type ret = collision_type::NONE;

	if (c1->GetType() == collider_type::COLLIDER_ITEM && c2->GetType() == collider_type::COLLIDER_PLAYER) {
		pickedUp = true;
		active = false;
		turnedOn = false;

		if (hitbox != nullptr) {
			hitbox->to_delete = true;
			hitbox = nullptr;
		}

		App->audio->PlayFx(App->audio->pickItemSfx.id, 0);
	}

	return ret;
}

void Item::Spawn(int posX, int posY)
{
	position.x = spawnPosition.x = (float)posX;
	position.y = spawnPosition.y = (float)posY;
}

void Item::Respawn()
{
	position = spawnPosition;
	pickedUp = false;
}