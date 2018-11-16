#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1FadeScene.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Scene2.h"

#include "j1EntityManager.h"
#include "Bat.h"

Bat::Bat() : Enemy(enemy_type::BAT)
{
	name.create("bat");
}

Bat::~Bat()
{

}