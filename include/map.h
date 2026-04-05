#ifndef MAP_H
#define MAP_H

#define CONTENT_SIZE 4096
#define TILE_LEN 40

#include <stdbool.h>

#include "../lib/types.h"
#include "../lib/v2.h"
#include "../include/entity.h"
#include "raylib.h"

typedef enum TileType //Not needed probably, But some kind of list of what A tile should look like idk last part to think about...
					  //
{
	TILETYPE_TEST1 = 1,
	TILETYPE_TEST2,
	TILETYPE_TEST3,
	TILETYPE_TEST4,
} TileType;

typedef enum EntityClass
{
	ENTITYCLASS_NONE,
	ENTITYCLASS_PLACEHOLDER,
} EntityClass;

typedef struct MapCamera
{
	Vector2 pos;
	V2 visible_tiles;
	Vector2 offset;
	Vector2 tile_offset;
	u32 tile_len;
	f32 zoom;
} MapCamera;

typedef struct Tile
{
	i32 type;
	f32 light;
	f32 light_level;
	bool animated;
	bool solid;
} Tile;

typedef struct LightSettings
{
	f32 ambient_light;
} LightSettings;

typedef struct Map
{
	MapCamera* camera;
	Tile* content;
	DynList* entities;
	LightSettings light_settings;
	V2 dim;	
} Map;


Vector2 map_get_mouse_cords(Map* map);
Map* map_new(V2 dim);
void map_destroy(Map* map);

bool map_load_level(Map* map, const char* filepath);
Tile map_get_tile(Map* map, V2 pos);
void map_set_tile(Map* map, V2 pos, Tile tile);
void map_render(Map* map, Texture2D* texp);

void cam_tick(Map* map, Vector2 source);
void cam_tick_editor(Map* map, Vector2 source);
void map_reset_light(Map* map);
void map_populate_light(Map* map);
void propagate(Map* map, i32 x, i32 y, f32 value);
void map_add_entity_lights(Map* map);

#endif
