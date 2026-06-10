
#include <stdlib.h>
#include <stdio.h>
#include "../include/state.h"
#include "../include/ui.h"
#include "../include/global.h"

GameState* state_new()
{
	GameState* newstate = malloc(sizeof(GameState));

	*newstate = (GameState) {
		.map = map_new(v2_new(0, 0)),
		.gfx = gfx_new(),
		.inventory = ui_inventory_new(),
	};
	map_load_level(newstate->map, "../maps/test.tmp");
	return newstate;
}

void state_tick(GameState* state)
{
	Map* map = state->map;	
	MapCamera* cam = map->camera;
	DynList* entities = map->entities;

	if(IsKeyDown(KEY_K))
	{
		ui_inventory_tick(state->inventory, state);
	}
	if(IsMouseButtonPressed(0))
	{
		Vector2 t = map_get_mouse_cords(map);
		P_LOG("\n MPTile = (%d, %d)\n", (i32) t.x, (i32) t.y);
	}

	entities_tick(map->entities, state);
	Entity* e = dynList_get(entities, 0);
	cam_tick(map, Vector2Midpoint(state->player->pos, state->player->data.dim));

	//map_reset_light(map);
	//map_add_entity_lights(map);
	//map_populate_light(map);

	map_render(map, &state->gfx->texs[TEXTURE_TILEMAP]); 


	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* self = dynList_get(entities, i);
		if( i == 0 )
		{
			const f32 f = 0.1;
#if 0
			if(IsKeyDown(KEY_D)) { self->pos.x += f; };
			if(IsKeyDown(KEY_A)) { self->pos.x -= f; };
			if(IsKeyDown(KEY_W)) { self->pos.y -= f; };
			if(IsKeyDown(KEY_S)) { self->pos.y += f; };
#endif
		}
#if 0
		DrawTexturePro(state->gfx->texs[TEXTURE_TILEMAP], self->state.sprite.rec_bmap, 
				(Rectangle) {
				(self->pos.x - cam->offset.x) * cam->tile_len, 
				(self->pos.y - cam->offset.y) * cam->tile_len, 
				self->dim.x * cam->tile_len, 
				self->dim.y * cam->tile_len}, 
				(Vector2) {0}, 0.0, RED);
#endif
	}
	entities_render(state->map->entities, state);

	map_render_light(map, state);

#if 1
	BeginTextureMode(state->gfx->light_map->map);
	//ClearBackground(BLACK);
	const u32 l1 = 255 * map->light_settings.ambient_light;
	Color color = (Color) {l1, l1, l1, 255};
	ClearBackground(color);

	BeginBlendMode(BLEND_ADDITIVE);
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* self = dynList_get(entities, i);
		Vector2 midpoint = Vector2Midpoint(self->pos, self->data.dim);
		if(!self->data.light.light_source) break;

#if 0
		Color grad_color = {
			self->data.light.tint.r * frand(self->data.light.flicker),
			self->data.light.tint.g * frand(self->data.light.flicker),
			self->data.light.tint.b * frand(self->data.light.flicker),
			self->data.light.tint.a,
		};
#endif
		Color grad_color = { self->data.light.value * self->data.light.tint.r,
							 self->data.light.value * self->data.light.tint.g,
							 self->data.light.value * self->data.light.tint.b,
							 255
		};
		for(i32 i = 0; i < 1; i++)
		{
			DrawCircleGradient(
					(midpoint.x - cam->offset.x) * cam->tile_len, 
					(midpoint.y - cam->offset.y) * cam->tile_len, 
					(self->data.light.distance * cam->tile_len / 2.0) * frand( (Vector2) {0.95, 0.05}),
					grad_color,
					BLACK );
		}
		const f32 tileSize = TILE_LEN;
		Vector2 lightPos = Vector2Scale(Vector2Subtract(self->pos, cam->offset), tileSize);
	}
	EndBlendMode();

	EndTextureMode();

#if 1
	BeginBlendMode(BLEND_MULTIPLIED);
	DrawTextureRec(
			state->gfx->light_map->map.texture,
			(Rectangle){0, 0, GetScreenWidth(), -GetScreenHeight()},
			(Vector2){0, 0},
			WHITE
			);

	EndBlendMode();
#endif
#endif
}

void state_render(GameState* state)
{
	ui_render(state);

}
