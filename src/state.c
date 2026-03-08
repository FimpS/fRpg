
#include <stdlib.h>
#include "../include/state.h"

GameState* state_new()
{
	GameState* newstate = malloc(sizeof(GameState));

	*newstate = (GameState) {
		.map = map_new(),
		.gfx = gfx_new(),
	};
	map_load_level(newstate->map, "../maps/test.tmp");
	return newstate;
}

void state_tick(GameState* state)
{
	Map* map = state->map;	
	MapCamera* cam = map->camera;


	cam_tick(map, (Vector2) {10, 10} );
	map_render(map, &state->gfx->texs[TEXTURE_TILEMAP]); 
}
