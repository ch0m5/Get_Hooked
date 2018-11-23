#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

class Enemy;

// ----------------------------------------------------
struct Objects {

	p2SString nameGroup;

	struct Object
	{
		p2SString name;
		float x, y, width, height;
		int id;

	};


	~Objects()
	{
		p2List_item<Object*>* itemP;
		itemP = Objectlist.start;

		while (itemP != NULL)
		{
			RELEASE(itemP->data);
			itemP = itemP->next;
		}

		Objectlist.clear();
	}




	p2List<Object*>	Objectlist;

};

struct Properties
{
	struct Property
	{
		p2SString name;
		float value;
	};


	~Properties()
	{
		p2List_item<Property*>* itemP;
		itemP = Propertieslist.start;

		while (itemP != NULL)
		{
			RELEASE(itemP->data);
			itemP = itemP->next;
		}

		Propertieslist.clear();
	}

	float GetProperty(const char* value, float def_value) const;

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	Propertieslist;
};

struct ImageLayer {

	SDL_Rect GetImageLayerRect() const;

	p2SString name;
	int Width, Height;
	float OffsetX, OffsetY = 0.0f;

	SDL_Texture*texture;

	float SpeedBack;

	Properties PropImg;

};

struct MapLayer {
	p2SString name;
	uint width;
	uint height;
	uint* data = nullptr;
	inline uint Get(int x, int y) const;
};

// ----------------------------------------------------

struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;
	
	p2List<ImageLayer*> imagelayers;
	p2List<iPoint>		checkpoints;	//CHANGE/FIX: This info should be included in the map xml using game objects or similar, first item is start, last is next lvl
	p2List<Enemy*>		enemies;	//CHANGE/FIX: This info should be included in the map xml using game objects or similar
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadImageLayer(pugi::xml_node& node, ImageLayer* Image);
	bool LoadProperties(pugi::xml_node& node, Properties& list);	

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__