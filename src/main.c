#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/types.h"
#include "../include/entity.h"
#include "../include/state.h"
#include "../include/global.h"
#include "../include/editor.h"
#include "../include/ui.h"
#include "../include/gfx.h"
#include "../include/npc.h"
#include "../include/player.h"
#include "../lib/hashmap.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>


int main(void) {
	srand(time(NULL));
	SetTraceLogLevel(LOG_ERROR);
	//HashMap* map = hmap_new(32);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "2D - TileMapEditor");
	InitAudioDevice();
	SetTargetFPS(60);

	HideCursor();
    Vector2 rectPos = { 400, 300 };
    float speed = 4.0f;
	GameState* state = state_new();


	state->player = player_new();
	//dynList_push(state->map->entities, state->player->entity); //This won't work later
#if 1
	dynList_push(state->map->npcs, npc_new(NPC_TYPE_TOWN_MERCHANT, (Vector2) { 17.0, 4.0 } ));
	for(i32 i = 0; i < 0; i++)
	{
	Entity* enemy = entity_new(ENTITY_PLACEHOLDER, (Vector2) {4.0, 4.0 + i} );
	enemy->target = state->player->entity;
	dynList_push(state->map->entities, enemy);
	}
#endif
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {5.0, 5.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {6.0, 5.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {10.0, 5.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {13.0, 5.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {5.0, 16.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {10.0, 22.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {22.0, 3.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {23.0, 5.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {33.0, 10.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {17.0, 12.5} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {17.0, 39.0} ));

	//state->player->path = entity_find_path(state->player, (Vector2) {16.0, 16.0}, state);

	//ValidateAndPrintPath(state->player->path, state->map->content, 32, 32);


	//dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER, (Vector2) {5.0, 5.0} ));

	ToggleFullscreen();
	int i = 0;
	float ft = 0.0;
    while (!WindowShouldClose()) 
	{
		state_tick(state);
		BeginDrawing();
		state_render(state);
		if(i++ % 60 == 0)
		{
			ft = GetFrameTime();
		}
		tick_tick();
		DrawText(TextFormat("%d", GetFPS()), 10, 10, 5, GREEN);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}


