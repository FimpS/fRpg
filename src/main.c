#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/types.h"
#include "../include/entity.h"
#include "../include/state.h"
#include "../include/global.h"
#include "../include/editor.h"
#include "../include/gfx.h"
#include "../lib/hashmap.h"
#include "raylib.h"



int main(void) {

	HashMap* map = hmap_new(32);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "2D - TileMapEditor");
    SetTargetFPS(60);

    Vector2 rectPos = { 400, 300 };
    float speed = 4.0f;
	GameState* state = state_new();
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER, (Vector2) {1.0, 1.0} ));
	dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER, (Vector2) {5.0, 5.0} ));

	//dynList_push(state->map->entities, entity_new(ENTITY_PLACEHOLDER, (Vector2) {5.0, 5.0} ));

	ToggleFullscreen();
    while (!WindowShouldClose()) 
	{
		BeginDrawing();
		state_tick(state);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}


