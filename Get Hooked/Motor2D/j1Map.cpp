#include "Brofiler/Brofiler.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "Enemy.h"
#include <cmath>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Map Draw", Profiler::Color::Magenta);

	if(map_loaded == false)
		return;

	ImageLayer*Image;

	for (int x = 0; x < data.imagelayers.count(); ++x) {

		Image = data.imagelayers.At(x)->data;

		App->render->Blit(data.imagelayers[x]->texture,
			Image->SpeedBack, data.imagelayers[x]->OffsetY,
			&data.imagelayers[x]->GetImageLayerRect(), SDL_FLIP_NONE, 0U, data.imagelayers[x]->speed);
	}

	p2List_item<TileSet*>* item; //Sprites_Layer
	item = data.tilesets.start;

	p2List_item<MapLayer*>* layer; //Map
	layer = data.layers.start;

	for (int y = 0; y < data.height; ++y) {
		for (int x = 0; x < data.width; ++x) {

			uint id = layer->data->Get(x, y);

			id = layer->data->data[id];

			if (id != 0) {
				SDL_Rect *rect = &item->data->GetTileRect(id);
				iPoint pos = MapToWorld(x, y);
				App->render->Blit(item->data->texture, pos.x, pos.y, rect);
			}
		}
	}
}


iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	ret.x = x * data.tile_width;
	ret.y = y * data.tile_height;

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

uint MapLayer::Get(int x, int y) const {
	return (y * width) + x;
}

SDL_Rect ImageLayer::GetImageLayerRect() const {

	SDL_Rect rec;

	rec.w = Width;
	rec.h = Height;
	rec.x = 0;
	rec.y = 0;

	return rec;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		if (item->data->texture != nullptr) {
			App->tex->UnLoad(item->data->texture);
			item->data->texture = nullptr;
		}

		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Remove all image layers
	p2List_item<ImageLayer*>* item3;
	item3 = data.imagelayers.start;

	while (item3 != NULL)
	{
		if (item3->data->texture != nullptr) {
			App->tex->UnLoad(item3->data->texture);
			item3->data->texture = nullptr;
		}

		RELEASE(item3->data);
		item3 = item3->next;
	}
	data.imagelayers.clear();

	// Remove all enemies
	p2List_item<Enemy*>* item4;
	item4 = data.enemies.start;

	while (item4 != NULL)
	{
		RELEASE(item4->data);
		item4 = item4->next;
	}
	data.enemies.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* set = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.add(set);
	}

	//Load ImageLayer Info-------------------------------------------
	pugi::xml_node Image_Layer;
	for (Image_Layer = map_file.child("map").child("imagelayer"); Image_Layer && ret; Image_Layer = Image_Layer.next_sibling("imagelayer"))
	{

		ImageLayer*ImagLay = new ImageLayer();

		ret = LoadImageLayer(Image_Layer, ImagLay);

		if (ret == true)
			data.imagelayers.add(ImagLay);

	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}
		
		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	App->collision->Load(map_file);

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");	

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));	//CHANGE/FIX: Texture is loaded into textureList and the content deleted(?) but the list node is never deleted, problem?
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	/*bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	pugi::xml_node layer_data = node.child("data");

	LoadProperties(node, layer->properties);

	layer->data = new uint[layer->height*layer->width];
	memset(layer->data, 0, sizeof(uint)*layer->width*layer->height);

	int i = 0;
	for (node = map_file.child("map").child("layer").child("data").child("tile"); node && ret; node = node.next_sibling("tile"))
	{
		layer->data[i++] = node.attribute("gid").as_uint();
	}

	return ret;*/

	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}

	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

bool j1Map::LoadImageLayer(pugi::xml_node& node, ImageLayer* Image) {

	bool ret = true;

	Image->name = node.attribute("name").as_string();
	Image->Width = node.child("image").attribute("width").as_int();
	Image->Height = node.child("image").attribute("height").as_int();
	Image->texture = App->tex->Load(PATH(folder.GetString(), node.child("image").attribute("source").as_string()));

	LoadProperties(node, Image->PropImg);

	p2List_item<Properties::Property*>* prop = Image->PropImg.Propertieslist.start;
	while (prop != NULL)
	{
		if (prop->data->name == "Speed")
			Image->speed = prop->data->value;

		prop = prop->next;
	}

	if (node.attribute("offsetx").as_int() != NULL)
		Image->OffsetX = node.attribute("offsetx").as_float();

	if (node.attribute("offsety").as_int() != NULL)
		Image->OffsetY = node.attribute("offsety").as_float();

	return ret;
}


bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;
	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* property_aux = new Properties::Property();

			property_aux->name = prop.attribute("name").as_string();
			property_aux->value = prop.attribute("value").as_float();

			properties.Propertieslist.add(property_aux);
		}
	}

	return ret;
}
float Properties::GetProperty(const char* value, float def_value) const
{
	p2List_item<Property*>* itemP = Propertieslist.start;

	while (itemP)
	{
		if (itemP->data->name == value)
			return itemP->data->value;
		itemP = itemP->next;
	}

	return def_value;
}

int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = Propertieslist.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}