
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "../include/map.h"
#include "../include/global.h"
#include "../include/sound.h"

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
	new_map->sound = map_sound_new();
	new_map->entities = dynList_new();
	new_map->dim = dim;
	//memset(new_map, 0, sizeof(new_map->content));	
	//new_map->dim = v2_new(0, 0);
	new_map->light_settings = (LightSettings) {
		.ambient_light = 0.15,
		.fade = WHITE,
	};
	new_map->camera = cam_new();
	return new_map;
}

void map_destroy(Map* map)
{
	free(map->camera);
	free(map->sound);
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

void map_render_ambient_light(Map* map)
{
	const f32 ambience = map->light_settings.ambient_light;
	Color fade = map->light_settings.fade;
	const Color ambience_color = (Color) { 
		ambience * fade.r, 
		ambience * fade.g, 
		ambience * fade.b, 
		255
	};
	ClearBackground(ambience_color);
}

f32 map_entity_light_pulse(Entity* self)
{
	LightPulse* pulse = &self->data.light.pulse;
	if(!pulse->enable) return 1.0;
	const f32 pulse_depth = self->data.light.distance * (1.0 - pulse->depth);
	const f32 pulse_speed = 100.0 * pulse->speed;
	return self->data.light.distance - fabsf(sin( (f32) self->light_frequency / pulse_speed ) * pulse_depth);
}

f32 map_entity_light_flicker(Entity* self)
{
	if(!self->data.light.flicker.enable || self->data.light.flicker.frequency == 0) return 1.0;
	return self->light_frequency % self->data.light.flicker.frequency == rand() % 4 ? 
		frand(self->data.light.flicker.min, self->data.light.flicker.max) :
		self->data.light.flicker.max;
}

void map_render_entity_light(Entity* self, Map* map)
{
	MapCamera* cam = map->camera;
	const Vector2 midpoint = Vector2Midpoint(self->pos, self->data.dim);

	if(!self->data.light.light_source) return;

	const f32 light_intensity_flicker = map_entity_light_flicker(self);
	const f32 frame_light = self->data.light.value * light_intensity_flicker;
	const Color inner_color = { 
		frame_light * self->data.light.tint.r,
		frame_light * self->data.light.tint.g,
		frame_light * self->data.light.tint.b,
		255
	};
	const Color outer_color = {
		0,
		0,
		0,
		255	 //Diffuse can be changed for interesting effects, It effects how fast the light level decreases, Can maybe be a parameter
	};

	const f32 light_radius = map_entity_light_pulse(self);
	DrawCircleGradient(
			(midpoint.x - cam->offset.x) * cam->tile_len, 
			(midpoint.y - cam->offset.y) * cam->tile_len, 
			(light_radius * cam->tile_len) * light_intensity_flicker,
			inner_color,
			outer_color);

	self->light_frequency ++;

}

void map_render_add_entity_lights(Map* map)
{
	MapCamera* cam = map->camera;
	DynList* entities = map->entities;
	BeginBlendMode(BLEND_ADDITIVE);
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* self = dynList_get(entities, i);
		map_render_entity_light(self, map);
	}
	EndBlendMode();

}

void map_render_apply_light_map(Map* map, GameState* state)
{
	DrawTextureRec(
			state->gfx->light_map->map.texture,
			(Rectangle){0, 0, GetScreenWidth(), -GetScreenHeight()},
			(Vector2){0, 0},
			WHITE
			);

}

void map_render_light(Map* map, GameState* state)
{
	MapCamera* cam = map->camera;
	DynList* entities = map->entities;

	BeginTextureMode(state->gfx->light_map->map);
	map_render_ambient_light(map);
	map_render_add_entity_lights(map);
	EndTextureMode();

	BeginBlendMode(BLEND_MULTIPLIED);
	map_render_apply_light_map(map, state);
	EndBlendMode();
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
			const f32 light_level =   1.0;// sqrtf(tile_data.light);	
			Color diffuse = (Color) {255 * light_level, 255 * light_level, 255 * light_level, 255};

			DrawTexturePro(tex, src, dst, (Vector2) {0}, 0, diffuse);
		}
	}	
}


