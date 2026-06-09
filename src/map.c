
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "../include/map.h"
#include "../include/global.h"

#define MAX_SCROLL_UP 128
#define MAX_SCROLL_DOWN 20


Vector2 map_get_mouse_cords(Map* map)
{
	return vector2(((f32)GetMouseX() / map->camera->tile_len + map->camera->offset.x), ((f32)GetMouseY() / map->camera->tile_len + map->camera->offset.y));
}

MapCamera* cam_new()
{
	MapCamera* mc_new = malloc(sizeof(MapCamera));
	*mc_new = (MapCamera) {
		.pos = (Vector2) {10.0, 10.0},
		.visible_tiles = (V2) {0.0, 0.0},
		.offset = (Vector2) {0.0, 0.0},
		.tile_offset = (Vector2) {0, 0},
		.tile_len = 40,
		.zoom = 0.0,
		.speed = 4.0,
	};
	return mc_new;
}

void cam_tick_editor(Map* map, Vector2 source)
{
	MapCamera* cam = map->camera;

	const f32 t = 128.0*(4096.0)/(GetScreenHeight() + GetScreenWidth());
	const f32 t2 = 128.0*(164.0)/(GetScreenHeight() + GetScreenWidth());
	cam->tile_len += GetMouseWheelMove() * 4;
	if(cam->tile_len >= (i32)t) { cam->tile_len = (i32)t; }
	else if(cam->tile_len <= t2) { cam->tile_len = t2; }


	Vector2 mouse_pos = GetMousePosition();
	cam->pos = vector2(source.x, source.y);
	cam->visible_tiles = v2_new(
			(i32) ceilf(GetScreenWidth() / cam->tile_len) + 1,
			(i32) ceilf(GetScreenHeight() / cam->tile_len) + 1);


	f32 half_w = cam->visible_tiles.x * 0.5;
	f32 half_h = cam->visible_tiles.y * 0.5;

#if 0
	if (cam->pos.x < half_w) cam->pos.x = half_w;
	if (cam->pos.y < half_h) cam->pos.y = half_h;
	if (cam->pos.x > map->dim.x - half_w) cam->pos.x = map->dim.x - half_w;
	if (cam->pos.y > map->dim.y - half_h) cam->pos.y = map->dim.y - half_h;
#endif

	cam->offset = (Vector2) { cam->pos.x - half_w, cam->pos.y - half_h };

	cam->tile_offset = (Vector2) { (cam->offset.x - ((i32) cam->offset.x)) * cam->tile_len, (cam->offset.y - ((i32) cam->offset.y)) * cam->tile_len };
}

void cam_tick(Map* map, Vector2 source)
{
	MapCamera* cam = map->camera;

	const f32 zoom_max = 128.0*(4096.0)/(GetScreenHeight() + GetScreenWidth());
	const f32 zoom_low = 128.0*(164.0)/(GetScreenHeight() + GetScreenWidth());
	cam->tile_len += GetMouseWheelMove() * 4;
	if(cam->tile_len >= (i32) zoom_max) { cam->tile_len = (i32) zoom_max; }
	else if(cam->tile_len <=  zoom_low) { cam->tile_len =  zoom_low; }

	const Vector2 target_pos = source;
	const f32 dt = GetFrameTime();

	const f32 t = 1.0 - expf(- cam->speed * dt);


	cam->pos.x += (target_pos.x - cam->pos.x) * t;
	cam->pos.y += (target_pos.y - cam->pos.y) * t;

	Vector2 mouse_pos = GetMousePosition();

	cam->visible_tiles = v2_new(
			(i32) ceilf(GetScreenWidth() / cam->tile_len) + 1,
			(i32) ceilf(GetScreenHeight() / cam->tile_len) + 1);


	f32 half_w = cam->visible_tiles.x * 0.5;
	f32 half_h = cam->visible_tiles.y * 0.5;

#if 1
	if (cam->pos.x < half_w) cam->pos.x = half_w;
	if (cam->pos.y < half_h) cam->pos.y = half_h;
	if (cam->pos.x > map->dim.x - half_w) cam->pos.x = map->dim.x - half_w;
	if (cam->pos.y > map->dim.y - half_h) cam->pos.y = map->dim.y - half_h;
#endif

	cam->offset = (Vector2) { cam->pos.x - half_w, cam->pos.y - half_h };

	cam->tile_offset = (Vector2) { (cam->offset.x - ((i32) cam->offset.x)) * cam->tile_len, (cam->offset.y - ((i32) cam->offset.y)) * cam->tile_len };
}

// Now we have reason to save map so loading in map_new is not bad option...
Map* map_new(V2 dim)
{
	Map* new_map = malloc(sizeof(Map));
	new_map->content = malloc(sizeof(Tile) * dim.x * dim.y);
	new_map->entities = dynList_new();
	new_map->dim = dim;
	//memset(new_map, 0, sizeof(new_map->content));	
	//new_map->dim = v2_new(0, 0);
	new_map->light_settings = (LightSettings) {
		.ambient_light = 0.15,
	};
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

bool map_load_level(Map* map, const char* filepath)
{
	FILE* fp = NULL;
	fp = fopen(filepath, "rb");
	if(!fp)
	{
		P_ERROR("ERROR: Failed to open file\n");
		return false;
	}

	fread(&map->dim, sizeof(V2), 1, fp);
	map->content = realloc(map->content, sizeof(Tile) * map->dim.x * map->dim.y);

	u32 c = 0;
	if((c = fread(map->content, sizeof(Tile), map->dim.x * map->dim.y, fp)) != map->dim.x * map->dim.y)
	{
		P_ERROR("File failed to read appropriate bytes\n");
	}
	u32 entity_list_len = 0;
	fread(&entity_list_len, sizeof(unsigned), 1, fp);

	for (u32 i = 0; i < entity_list_len; i++)
	{
		Entity* allocated_entity = malloc(sizeof(Entity));
		fread(allocated_entity, sizeof(Entity), 1, fp);
		dynList_push(map->entities, allocated_entity);
	}
	fclose(fp);
	return true;
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

static Rectangle tilemap_textures[] =
{
	[0] = {0, 0, 16, 16},
	[TILETYPE_TEST1] = {0, 16, 16, 16},
	[TILETYPE_TEST2] = {16, 0, 16, 16},
	[TILETYPE_TEST3] = {32, 0, 16, 16},
	[TILETYPE_TEST4] = {48, 0, 16, 16},
};

void map_reset_light(Map* map)
{
	const u32 len = map->dim.x * map->dim.y;
	for(i32 i = 0; i < len; i++)
	{
		map->content[i].light = map->light_settings.ambient_light;
		map->content[i].light_level = 0.0;
	}
}

static i32 tick = 0;

f32 map_light_flicker(Vector2 flicker)
{
	if(tick % (rand() % 4 + 4) == 0)
	{
		return frand(flicker);
	}
	else return flicker.x;
}


void map_add_entity_lights(Map *map)
{
	tick ++;
		printf("\n");
	for(i32 i = 0; i < dynList_len(map->entities); i++)
	{
		Entity* e = dynList_get(map->entities, i);
		Vector2 pos = Vector2Midpoint(e->pos, e->data.dim);

		Tile map_tile = map_get_tile(map, Vector2V2(pos));
		if(e->data.light.value <= map->light_settings.ambient_light || map_tile.solid) 
		{
			continue;
		}
		e->light.self = 1.0;//e->data.light.value * 2.0;

		V2 t = (V2) { (i32)floorf(pos.x), (i32)floorf(pos.y) };
		V2 v = (V2) { (i32)ceilf(pos.x), (i32)ceilf(pos.y) };

		if(t.x >= 0 && t.y >= 0 && t.x < map->dim.x && t.y < map->dim.y)
		{
			map->content[t.y * map->dim.x + t.x].light = e->data.light.value * map_light_flicker(e->data.light.flicker);
			map->content[t.y * map->dim.x + t.x].light_level = e->data.light.distance;
			P_LOG("%d %f\n", e->type, map->content[t.y * map->dim.x + t.x].light);
		}

	}
		printf("\n");
}

void map_populate_step(Map *map, Vector2 pos, f32 value, f32 v)
{
	Tile *t = &map->content[(i32)pos.y * map->dim.x + (i32)pos.x];

	if(t->solid) value *= 0.4;

	if(value > t->light)
	{
		t->light_level = v;
		t->light = value;
	}
}
void map_print_light(Map* map)
{
	for(i32 y = 1; y < map->dim.y-100; y++)
	{
		for(i32 x = 1; x < map->dim.x-100; x++)
		{
			Tile* t = &map->content[y * map->dim.x + x];
			const f32 light = t->light;
			const f32 energy = t->light_level;
			printf("%f ", light);
		}
		printf("\n");
	}
	printf("\n");
}


void map_populate_light(Map *map)
{
	const f32 light_decay = 0.12;
	const f32 energy_decay = 1.0;
	const f32 epsilon = 0.01;
	const u32 max_iterations = 12;
	for(i32 iter = 0; iter < max_iterations; iter++)
	{
		//TODO this is gigaslow
		for(i32 y = 1; y < map->dim.y-1; y++)
		{
			for(i32 x = 1; x < map->dim.x-1; x++)
			{
				Tile* t = &map->content[y * map->dim.x + x];
				const f32 light = t->light;
				const f32 energy = t->light_level;

				if(light <= epsilon) continue;
				const f32 spread = light - (energy) / 100.0;
				f32 loss = energy - energy_decay;

				if(energy <= 0) continue;
				//if(loss <= 0) continue;
				//if(spread <= 0) continue;

				map_populate_step(map, (Vector2) {x + 1, y}, spread, loss);
				map_populate_step(map, (Vector2) {x - 1, y}, spread, loss);
				map_populate_step(map, (Vector2) {x, y + 1}, spread, loss);
				map_populate_step(map, (Vector2) {x, y - 1}, spread, loss);
				const f32 diag = spread - loss * 1/sqrtf(2.0);
				loss *= sqrtf(2.0);
				if(diag > 0 && 1)
				{
					Tile *t_right = &map->content[y * map->dim.x + (x + 1)];
					Tile *t_left  = &map->content[y * map->dim.x + (x - 1)];
					Tile *t_up    = &map->content[(y - 1) * map->dim.x + x];
					Tile *t_down  = &map->content[(y + 1) * map->dim.x + x];

					if(!(t_right->solid && t_down->solid))
						map_populate_step(map, (Vector2) {x + 1, y + 1}, diag, loss);

					if(!(t_left->solid && t_up->solid))
						map_populate_step(map, (Vector2) {x - 1, y - 1}, diag, loss);

					if(!(t_right->solid && t_up->solid))
						map_populate_step(map, (Vector2) {x + 1, y - 1}, diag, loss);

					if(!(t_left->solid && t_down->solid))
						map_populate_step(map, (Vector2) {x - 1, y + 1}, diag, loss);
				}
			}
		}
	}
}

void map_render(Map* map, Texture2D* texp)
{
	MapCamera* cam = map->camera;
	Texture2D tex = *texp;
	i32 tile = cam->tile_len;

	i32 start_x = (i32)floorf(cam->offset.x);
	i32 start_y = (i32)floorf(cam->offset.y);

	Vector2 mouse = GetMousePosition();
	V2 mp = {
		(i32)floorf(mouse.x / tile + cam->offset.x),
		(i32)floorf(mouse.y / tile + cam->offset.y)
	};
	for (i32 x = 0; x < cam->visible_tiles.x + 1; x++)
	{
		for (i32 y = 0; y < cam->visible_tiles.y + 1; y++)
		{
			i32 tx = start_x + x;
			i32 ty = start_y + y;

			Rectangle dst = {
				(tx - cam->offset.x) * tile,
				(ty - cam->offset.y) * tile,
				tile,
				tile
			};

			Tile tile_data = map_get_tile(map, (V2){tx, ty});
			Rectangle src = tilemap_textures[tile_data.type];
			const f32 light_level =  (sqrtf(tile_data.light)) * 1;// sqrtf(tile_data.light);	
			Color diffuse = (Color) {255 * light_level, 255 * light_level, 255 * light_level, 255};

			DrawTexturePro(tex, src, dst, (Vector2) {0}, 0, diffuse);
		}
	}	
}


