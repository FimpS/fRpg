
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "../include/map.h"
#include "../include/global.h"

#define MAX_SCROLL_UP 128
#define MAX_SCROLL_DOWN 28


Vector2 map_get_mouse_cords(Map* map)
{
	return vector2(((f32)GetMouseX() / map->camera->tile_len + map->camera->offset.x), ((f32)GetMouseY() / map->camera->tile_len + map->camera->offset.y));
}

MapCamera* cam_new()
{
	MapCamera* mc_new = malloc(sizeof(MapCamera));
	*mc_new = (MapCamera) {
		.pos = (Vector2) {0.0, 0.0},
		.visible_tiles = (V2) {0, 0},
		.offset = (Vector2) {0, 0},
		.tile_offset = (Vector2) {0, 0},
		.tile_len = 40,
		.zoom = 0.0,
	};
}

void cam_tick(Map* map, Vector2 source)
{
	MapCamera* cam = map->camera;

	cam->tile_len += GetMouseWheelMove() * 4;
	if(cam->tile_len >= MAX_SCROLL_UP) { cam->tile_len = MAX_SCROLL_UP; }
	else if(cam->tile_len <= MAX_SCROLL_DOWN) { cam->tile_len = MAX_SCROLL_DOWN; }


	Vector2 mouse_pos = GetMousePosition();
	cam->pos = vector2(source.x, source.y);
	cam->visible_tiles = v2_new(
			(i32) ceilf(GetScreenWidth() / cam->tile_len) + 1,
			(i32) ceilf(GetScreenHeight() / cam->tile_len) + 1);


	f32 half_w = cam->visible_tiles.x * 0.5;
	f32 half_h = cam->visible_tiles.y * 0.5;

	if (cam->pos.x < half_w) cam->pos.x = half_w;
	if (cam->pos.y < half_h) cam->pos.y = half_h;
	if (cam->pos.x > map->dim.x - half_w) cam->pos.x = map->dim.x - half_w;
	if (cam->pos.y > map->dim.y - half_h) cam->pos.y = map->dim.y - half_h;

	cam->offset = (Vector2) { cam->pos.x - half_w, cam->pos.y - half_h };

	cam->tile_offset = (Vector2) { (cam->offset.x - ((i32) cam->offset.x)) * cam->tile_len, (cam->offset.y - ((i32) cam->offset.y)) * cam->tile_len };
}

// Now we have reason to save map so loading in map_new is not bad option...
Map* map_new()
{
	Map* new_map = malloc(sizeof(Map));
	new_map->content = malloc(sizeof(Tile) * CONTENT_SIZE);
	new_map->entities = dynList_new();
	//memset(new_map, 0, sizeof(new_map->content));	
	new_map->dim = v2_new(0, 0);
	new_map->camera = cam_new();
	return new_map;
}

void map_destroy(Map* map)
{
	free(map->camera);
	free(map->content);
	free(map->camera);
	free(map);
}

Tile map_get_tile(Map* map, V2 pos)
{
	return pos.x >= 0 && pos.x < map->dim.x && pos.y >= 0 && pos.y < map->dim.y ? map->content[pos.y * map->dim.x + pos.x] : (Tile) {-1};
}

void map_set_tile(Map* map, V2 pos, Tile tile)
{
	if(pos.x >= 0 && pos.x < map->dim.x && pos.y >= 0 && pos.y < map->dim.y)
	{
		map->content[pos.y * map->dim.x + pos.x] = tile;
	}
}


