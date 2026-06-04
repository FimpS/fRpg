
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

	if(IsKeyDown(KEY_I))
	{
		ui_inventory_tick(state->inventory, state);
	}
	entities_tick(map->entities, state);
	Entity* e = dynList_get(entities, 0);
	cam_tick(map, Vector2Midpoint(state->player->pos, state->player->dim));

	map_reset_light(map);
	map_add_entity_lights(map);
	map_populate_light(map);

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

#if 0
	BeginTextureMode(state->gfx->light_map->map);
	//ClearBackground(BLACK);
	Color color = (Color) {0, 0, 100, 255};
	ClearBackground(color);

	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* self = dynList_get(entities, i);
		DrawCircleGradient(
				(self->pos.x - cam->offset.x) * cam->tile_len, 
				(self->pos.y - cam->offset.y) * cam->tile_len, 
				self->light.distance * cam->tile_len,
				WHITE,
				color );
	}
	EndTextureMode();

	BeginBlendMode(BLEND_MULTIPLIED);
	DrawTextureRec(
			state->gfx->light_map->map.texture,
			(Rectangle){0, 0, GetScreenWidth(), -GetScreenHeight()},
			(Vector2){0, 0},
			WHITE
			);

	EndBlendMode();
#endif
}

void state_render(GameState* state)
{
	ui_render(state);

}
