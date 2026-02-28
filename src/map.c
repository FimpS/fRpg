
#include <stdlib.h>
#include <string.h>

#include "../include/map.h"
#include "../include/global.h"

Vector2 map_get_mouse_cords(Map* map)
{
	return vector2(((f32)GetMouseX() / TILE_LEN + map->camera->offset.x), ((f32)GetMouseY() / TILE_LEN + map->camera->offset.y));
}

MapCamera* cam_new()
{
	MapCamera* mc_new = malloc(sizeof(MapCamera));
	*mc_new = (MapCamera) {
		.pos = (Vector2) {0.0, 0.0},
		.visible_tiles = (V2) {0, 0},
		.offset = (Vector2) {0, 0},
		.tile_offset = (Vector2) {0, 0},
		.zoom = 0.0,
	};
}

void cam_tick(Map* map, Vector2 source)
{
	MapCamera* cam = map->camera;
	Vector2 mouse_pos = GetMousePosition();
	cam->pos = vector2(source.x, source.y);
	cam->visible_tiles = v2_new(GetScreenWidth() / TILE_LEN, GetScreenHeight() / TILE_LEN);

	cam->offset = vector2(cam->pos.x - (f32)cam->visible_tiles.x / 2.0, cam->pos.y - (f32)cam->visible_tiles.y / 2.0);

	if(cam->offset.x < 0.0) cam->offset.x = 0.0;
	if(cam->offset.y < 0.0) cam->offset.y = 0.0;
	if(cam->offset.x > map->dim.x - cam->visible_tiles.x) cam->offset.x = map->dim.x - cam->visible_tiles.x;
	if(cam->offset.y > map->dim.y - cam->visible_tiles.y) cam->offset.y = map->dim.y - cam->visible_tiles.y;

	cam->tile_offset = vector2((cam->offset.x - ((i32) cam->offset.x)) * TILE_LEN, (cam->offset.y - ((i32) cam->offset.y)) * TILE_LEN);
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


