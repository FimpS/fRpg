
#include <stdlib.h>
#include <stdio.h>
#include "../include/state.h"
#include "../include/sound.h"
#include "../include/ui.h"
#include "../include/global.h"

static SoundMultiple* state_get_global_sound(GameState* state, SoundTypeGlobal type)
{
	return &state->global_sound_manager->sound_pool[type];
}

void play_global_sound(GameState* state, SoundTypeGlobal type)
{
	play_sound_multiple(state_get_global_sound(state, type));
}

GameState* state_new()
{
	GameState* newstate = malloc(sizeof(GameState));

	*newstate = (GameState) {
		.map = map_new(v2_new(0, 0)),
		.gfx = gfx_new(),
		.inventory = ui_inventory_new(),
		.global_sound_manager = sound_manager_new(),
	};

	sound_manager_global_init(newstate->global_sound_manager);
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
		//P_LOG("\n MPTile = (%d, %d)\n", (i32) t.x, (i32) t.y);
	}

	entities_tick(map->entities, state);
	Entity* e = dynList_get(entities, 0);
	cam_tick(map, Vector2Midpoint(state->player->pos, state->player->data.dim));

	if(IsKeyDown(KEY_M)) state->map->light_settings.ambient_light += 0.01;

}

void state_render(GameState* state)
{
	Map* map = state->map;
	map_render(map, &state->gfx->texs[TEXTURE_TILEMAP]); 
	entities_render(state->map->entities, state);
	map_render_light(map, state);
	ui_render(state);
}
