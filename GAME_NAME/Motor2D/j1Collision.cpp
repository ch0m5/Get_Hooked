#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"

j1Collision::j1Collision()
{
	name.create("collision");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER_ATTACK] = false;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY_ATTACK] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER_ATTACK] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_ATTACK] = true;

	matrix[COLLIDER_PLAYER_ATTACK][COLLIDER_WALL] = false;
	matrix[COLLIDER_PLAYER_ATTACK][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER_ATTACK][COLLIDER_PLAYER_ATTACK] = false;
	matrix[COLLIDER_PLAYER_ATTACK][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER_ATTACK][COLLIDER_ENEMY_ATTACK] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER_ATTACK] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY_ATTACK] = false;

	matrix[COLLIDER_ENEMY_ATTACK][COLLIDER_WALL] = false;
	matrix[COLLIDER_ENEMY_ATTACK][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY_ATTACK][COLLIDER_PLAYER_ATTACK] = false;
	matrix[COLLIDER_ENEMY_ATTACK][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY_ATTACK][COLLIDER_ENEMY_ATTACK] = false;

	//screen parameter to render bellow and avoid green background @Andres
	//screen = { 0, 0, SCREEN_WIDTH * SCREEN_SIZE, SCREEN_HEIGHT * SCREEN_SIZE };	//SamAlert: This is code from Project1, I can't remember what it's for, decide if it's relevant to you
}

j1Collision::~j1Collision()
{}

// Called before render is available
bool j1Collision::Awake(pugi::xml_node&)
{
	bool ret = true;

	return ret;
}

// Called each loop iteration
bool j1Collision::PreUpdate()
{
	bool ret = true;

	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return ret;
}
bool j1Collision::Update(float dt)
{
	bool ret = true;

	DebugDraw();

	return ret;
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

// Save and Load
bool j1Collision::Load(pugi::xml_document& map_file)
{
	LOG("Loading Colliders");
	bool ret = true;
	//Load all collider info

	pugi::xml_node collider;

	for (collider = map_file.child("map").child("objects"); collider && ret; collider.next_sibling("objects"))
	{
		for (pugi::xml_node object = collider.child("object"); object && ret; object = collider.next_sibling("object"))
		{
			Collider* col = new Collider();

			col->rect.x = object.attribute("x").as_uint();
			col->rect.y = object.attribute("y").as_uint();
			col->rect.w = object.attribute("y").as_uint();
			col->rect.h = object.attribute("y").as_uint();
			col->type = (COLLIDER_TYPE)object.attribute("name").as_uint();

			App->collision->AddCollider(col->rect, col->type, col->callback);
		}
	}
	return ret;
	
}
//bool Save(pugi::xml_node&) const;

void j1Collision::DebugDraw()
{
	/*if (App->input->colliderView == false || App->input->debugMode == false) {	// SamAlert: This code relates to the debug function of collider viewing
		return;
	}*/

	Uint8 alpha = 80;
	Uint8 alphaHard = 130;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

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

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_PLAYER: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER_ATTACK: // cyan
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
		case COLLIDER_ENEMY: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_ENEMY_ATTACK: // yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		}
	}
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return !(rect.y + rect.h < r.y || rect.y > r.y + r.h || rect.x + rect.w < r.x || rect.x > r.x + r.w);
}