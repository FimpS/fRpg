#ifndef MAP_H
#define MAP_H

#define CONTENT_SIZE 4096
#define TILE_LEN 40

#include <stdbool.h>

#include "../lib/types.h"
#include "../lib/v2.h"
#include "../include/entity.h"
#include "raylib.h"

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
	bool animated;
	bool solid;
} Tile;

typedef struct EntitySignature //should prob be in Map...
{
	EntityType type;
	V2 spawn_tile;
} EntitySignature;

typedef struct Map
{
	MapCamera* camera;
	Tile* content;
	EntitySignature* entsinfo;
	DynList* entities;
	V2 dim;	
} Map;


Vector2 map_get_mouse_cords(Map* map);
Map* map_new();
void map_destroy(Map* map);
Tile map_get_tile(Map* map, V2 pos);
void map_set_tile(Map* map, V2 pos, Tile tile);

void cam_tick(Map* map, Vector2 source);

#endif
