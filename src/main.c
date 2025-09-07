#include <stdio.h>

#include "../lib/types.h"
#include "../include/entity.h"
#include "../include/state.h"
#include "../include/global.h"
#include "../lib/hashmap.h"
#include "raylib.h"

int main(void) {

	HashMap* map = hmap_new(32);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 900, "raylib - input example");
	//ToggleFullscreen();

    Vector2 rectPos = { 400, 300 };
    float speed = 4.0f;
	GameState* state = state_new();

	dynList_push(state->entities, entity(
				ENTITY_PLACEHOLDER, 
				vector2( -1.0, 2.0)
				));

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
#if 0
        if (IsKeyDown(KEY_RIGHT)) rectPos.x += speed;
        if (IsKeyDown(KEY_LEFT))  rectPos.x -= speed;
        if (IsKeyDown(KEY_DOWN))  rectPos.y += speed;
        if (IsKeyDown(KEY_UP))    rectPos.y -= speed;

        // Move rectangle to mouse when left click
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            rectPos = GetMousePosition();
        }

		entities_tick(state);
		// ---- DRAW ----
		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawRectangle(rectPos.x - 50, rectPos.y - 25, 100, 50, SKYBLUE);
		DrawText("Use arrow keys to move", 10, 10, 20, DARKGRAY);
		DrawText("Click mouse to teleport", 10, 40, 20, DARKGRAY);
		EndDrawing();
#endif
		BeginDrawing();
		
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

