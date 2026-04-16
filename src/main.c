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
#include "../lib/hashmap.h"
#include "raylib.h"



int main(void) {
	srand(time(NULL));
	SetTraceLogLevel(LOG_ERROR);
	//HashMap* map = hmap_new(32);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "2D - TileMapEditor");
    SetTargetFPS(60);

	HideCursor();
    Vector2 rectPos = { 400, 300 };
    float speed = 4.0f;
	GameState* state = state_new();


	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER, (Vector2) {1.0, 1.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {5.0, 5.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {17.0, 12.5} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER2, (Vector2) {17.0, 39.0} ));

	state->player = entity_player_init(state);
	dynList_push(state->map->entities, state->player);

	//dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER, (Vector2) {5.0, 5.0} ));

	ToggleFullscreen();
    while (!WindowShouldClose()) 
	{
		BeginDrawing();
		state_tick(state);
		state_render(state);
		DrawText(TextFormat("Sword of the Fallen\nNeutral"), 10, 10, 5, GREEN);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}


