#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "Item.h"
#include "Coin.h"

Coin::Coin() : Item(entity_type::COIN)
{
	name.create("coin");
}

bool Coin::Start()
{
	bool ret = true;

	graphics = App->tex->Load(textureName.GetString());

	hitbox = App->collision->AddCollider({ (int)position.x + hitboxOffset.x, (int)position.y + hitboxOffset.y, hitboxOffset.w, hitboxOffset.h }, COLLIDER_ITEM, this);
	hitboxOffset = hitbox->rect;

	return ret;
}

// Import all sprite data using the above function for each animation
void Coin::ImportAllSprites(pugi::xml_node& first_sprite)
{
	ImportSpriteData("idle", &idleSprite, first_sprite);
}

// Allocate all animations with previously recieved sprite data
void Coin::AllocAllAnimations()
{
	idleSprite.anim.AllocAnimation({ idleSprite.sheetPosition.x * spriteSize.x, idleSprite.sheetPosition.y * spriteSize.y }, spriteSize, idleSprite.numFrames);
}

void Coin::UpdateHitbox()
{
	animPtr = &idleSprite.anim;
	hitboxOffset = ReshapeCollider(idleSprite);
}