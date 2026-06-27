
#include <stdlib.h>
#include <stdio.h>
#include "../include/state.h"
#include "../include/player.h"
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

	if(IsMouseButtonPressed(0))
	{
		Vector2 t = map_get_mouse_cords(map);
		//P_LOG("\n MPTile = (%d, %d)\n", (i32) t.x, (i32) t.y);
	}

	npcs_tick(map->npcs, state);
	player_tick(state->player, state);
	entities_tick(map->entities, state);
	Entity* e = dynList_get(entities, 0);
	cam_tick(map, Vector2Midpoint(state->player->entity->pos, state->player->entity->data.dim));

	if(IsKeyDown(KEY_M)) state->map->light_settings.ambient_light += 0.01;

}

void state_render(GameState* state)
{
	Map* map = state->map;
	map_render(map, &state->gfx->texs[TEXTURE_TILEMAP]); 
	player_render(state->player, state);
	entities_render(state->map->entities, state);
	npcs_render(map->npcs, state);
	map_render_light(map, state);
	npcs_ui_render(map->npcs, state);
	ui_render(state);
}
