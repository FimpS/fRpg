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
    InitWindow(1200, 900, "raylib - input example");
    SetTargetFPS(60);
	//ToggleFullscreen();

    Vector2 rectPos = { 400, 300 };
    float speed = 4.0f;
	GameState* state = state_new();

	dynList_push(state->entities, entity_new(
				ENTITY_PLACEHOLDER, 
				vector2( -1.0, 2.0)
				));


    while (!WindowShouldClose()) {
		BeginDrawing();
		
		EndDrawing();
	}

	CloseWindow();
	return 0;
}


