#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "Player.h"
#include "PhysicalElement.h"

j1Collision::j1Collision()
{
	name.create("collision");
}

j1Collision::~j1Collision()
{}

// Called before render is available
bool j1Collision::Awake(pugi::xml_node& config)
{
	bool ret = true;

	pugi::xml_node childList = config.child("matrix").first_child();
	pugi::xml_attribute attributeList = childList.first_attribute();

	for (int i = COLLIDER_WALL; i < COLLIDER_MAX; ++i) {	// @Carles, automatically allocate collider matrix using config.xml
		for (int j = COLLIDER_WALL; j < COLLIDER_MAX; ++j) {

			matrix[i][j] = attributeList.as_bool();
			attributeList = attributeList.next_attribute();
		}

		childList = childList.next_sibling();
		attributeList = childList.first_attribute();
	}

	mustDebugDraw = config.child("mustDebugDraw").attribute("value").as_bool();

	return ret;
}

// Called each loop iteration
bool j1Collision::PreUpdate()	// @Carles
{
	bool ret = true;
	p2List_item<Collider*>* tmpCollider;
	p2List_item<Collider*>* nextCollider;

	// Remove all colliders scheduled for deletion
	for (tmpCollider = colliders.start; tmpCollider != nullptr; tmpCollider = nextCollider) {
		nextCollider = tmpCollider->next;

		if (tmpCollider->data->to_delete == true) {
			colliders.del(tmpCollider);
		}
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (tmpCollider = colliders.start; tmpCollider != nullptr; tmpCollider = tmpCollider->next) {
		c1 = tmpCollider->data;

		if (c1->active) {
			for (nextCollider = tmpCollider->next; nextCollider != nullptr; nextCollider = nextCollider->next) {
				c2 = nextCollider->data;

				if (c2->active) {
					if (c1->CheckCollision(c2->rect) == true) {
						if (matrix[c1->type][c2->type] && c1->callback)
							c1->callback->OnCollision(c1, c2);

						if (matrix[c2->type][c1->type] && c2->callback)
							c2->callback->OnCollision(c2, c1);
					}
				}
			}
		}
	}

	return ret;
}

bool j1Collision::Update()
{
	bool ret = true;

	DebugDraw();

	return ret;
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	p2List_item<Collider*>* item;

	for (item = colliders.start; item != nullptr; item = item->next) {
		RELEASE(item->data);
	}
	colliders.clear();

	return true;
}

// Save and Load
bool j1Collision::Load(pugi::xml_document& map_file)
{
	LOG("Loading Colliders");
	bool ret = true;

	//Load all collider info
	for (pugi::xml_node collider = map_file.child("map").child("objectgroup"); collider && ret; collider = collider.next_sibling("objectgroup"))
	{
		for (pugi::xml_node object = collider.child("object"); object && ret; object = object.next_sibling("object"))
		{
			// @Carles
			SDL_Rect tmpRect = { object.attribute("x").as_int(), object.attribute("y").as_int(), object.attribute("width").as_int(), object.attribute("height").as_int() };
			AddCollider(tmpRect, (collider_type)object.attribute("name").as_uint(), nullptr);
		}
	}

	return ret;
}

//bool Save(pugi::xml_node&) const;

void j1Collision::DebugDraw()
{
	if (mustDebugDraw == false) {
		return;
	}

	Uint8 alpha = 80;
	Uint8 alphaHard = 130;

	p2List_item<Collider*>* tmpCollider;
	for (tmpCollider = colliders.start; tmpCollider != nullptr; tmpCollider = tmpCollider->next)
	{
		/* COLORS
		Red: 255, 0, 0
		Green: 0, 255, 0
		Blue: 0, 0, 255
		Yellow: 255, 255, 0
		Magenta: 255, 0, 255
		Cyan: 0, 255, 255
		White: 255, 255, 255
		Black: 0, 0, 0
		Brown: 160, 128, 96
		*/

		switch (tmpCollider->data->GetType())
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(tmpCollider->data->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL: // green
			App->render->DrawQuad(tmpCollider->data->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_PLATFORM: // magenta
			App->render->DrawQuad(tmpCollider->data->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_FALLING_PLATFORM: // brown
			App->render->DrawQuad(tmpCollider->data->rect, 160, 128, 96, alpha);
			break;
		case COLLIDER_PLAYER: // blue
			App->render->DrawQuad(tmpCollider->data->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER_ATTACK: // cyan
			App->render->DrawQuad(tmpCollider->data->rect, 0, 255, 255, alpha);
			break;
		case COLLIDER_ENEMY: // red
			App->render->DrawQuad(tmpCollider->data->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_ENEMY_ATTACK: // yellow
			App->render->DrawQuad(tmpCollider->data->rect, 255, 255, 0, alpha);
			break;
		}
	}
}

Collider* j1Collision::AddCollider(SDL_Rect rect, collider_type type, PhysicalElement* callback)	//@Carles
{
	Collider* tmpPtr = new Collider(rect, type, callback);
	colliders.add(tmpPtr);

	return tmpPtr;
}

void j1Collision::DestroyCollider(Collider* collider)
{
	int indexPos = colliders.find(collider);
	
	if (indexPos > -1) {
		p2List_item<Collider*>* item;
		item = colliders.At(indexPos);

		RELEASE(item->data);
		colliders.del(item);
	}
	else {
		LOG("Collider not found inside list!");
	}
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return !(rect.y + rect.h < r.y || rect.y > r.y + r.h || rect.x + rect.w < r.x || rect.x > r.x + r.w);
}

bool j1Collision::CheckGroundCollision(Collider* hitbox) const
{
	bool ret = false;

	Collider* tmpHitbox = hitbox;
	tmpHitbox->rect.y++;

	for (p2List_item<Collider*>* tmpCollider = colliders.start; tmpCollider != nullptr; tmpCollider = tmpCollider->next) {

		if (tmpHitbox->CheckCollision(tmpCollider->data->rect) == true && matrix[tmpHitbox->type][tmpCollider->data->type] && tmpHitbox->callback) {
				ret = true;
		}
	}

	return ret;
}