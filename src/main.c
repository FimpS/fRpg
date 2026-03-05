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

#define EDITOR 1


#if !EDITOR
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
#endif

#if EDITOR

i32 main(i32 argc, u8* argv[])
{
	srand(time(NULL));
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "2D - TileMapEditor");
    SetTargetFPS(60);

	ToggleFullscreen();
	u8 filename[MAX_FILE_LEN];
	if(argc < 4)
	{
		printf("Too few arguments");
		CloseWindow();
	}
	strcpy(filename, argv[1]);
	editor_parse_file_input(filename);
	V2 map_dimension = {
		.x = atoi(argv[2]),
		.y = atoi(argv[3]),
	};
	Editor* editor = editor_new(filename, map_dimension);	
	CloseWindow();
	return 0;
	//dynList_push(editor->map->entities, entity_new(ENTITY_PLACEHOLDER, vector2(1.0, 1.0)));
	//dynList_push(editor->map->entities, entity_new(ENTITY_PLACEHOLDER2, vector2(4.0, 4.0)));

    while (!WindowShouldClose()) 
	{
		BeginDrawing();
		editor_tick(editor);
		editor_render(editor);
		DrawText(TextFormat("Height: %d ; Width: %d", GetScreenHeight(), GetScreenWidth()), 100, 100, 40, GREEN);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}

#endif

