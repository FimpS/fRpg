#pragma once

#define CONTENT_SIZE 4096
#define TILE_LEN 40

#include <stdbool.h>

#include "../lib/types.h"
#include "../lib/v2.h"
#include "entity.h"
#include "struct.h"
#include "raylib.h"


Vector2 map_get_mouse_cords(Map* map);
Vector2 map_convert_screen_to_map(Vector2 v, Map* map);
Vector2 map_convert_map_to_screen(Vector2 v, Map* map);
Map* map_new(V2 dim);
void map_destroy(Map* map);

bool map_load_level(Map* map, const char* filepath);
Tile map_get_tile(Map* map, V2 pos);
void map_set_tile(Map* map, V2 pos, Tile tile);
void map_render(Map* map, Texture2D* texp);
void map_render_light(Map* map, GameState* state);
Tile map_get_tile(Map* map, V2 pos);

void cam_tick(Map* map, Vector2 source);
void cam_tick_editor(Map* map, Vector2 source);

#if 0
void map_reset_light(Map* map);
void map_populate_light(Map* map);
void propagate(Map* map, i32 x, i32 y, f32 value);
void map_add_entity_lights(Map* map);
#endif

