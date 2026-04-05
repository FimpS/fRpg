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

i32 main(i32 argc, u8* argv[])
{
	srand(time(NULL));
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "2D - TileMapEditor");
    SetTargetFPS(60);

	ToggleFullscreen();
	u8 filename[MAX_FILE_LEN];
	if(argc < 2)
	{
		CloseWindow();
		return 0;
	}
	strcpy(filename, argv[1]);
	editor_parse_file_input(filename);
	V2 map_dimension;
	if(argc >= 4)
	{
		map_dimension = (V2) {
			.x = atoi(argv[2]),
				.y = atoi(argv[3]),
		};
	}
	else
	{
		map_dimension = (V2) {
			.x = 64,
				.y = 64,
		};
	}
	Editor* editor = editor_new(filename, map_dimension);	

	while (!WindowShouldClose()) 
	{
		BeginDrawing();
		editor_tick(editor);
		editor_render(editor);
		//DrawText(TextFormat("Height: %d ; Width: %d", GetScreenHeight(), GetScreenWidth()), 100, 100, 40, GREEN);
		DrawText(TextFormat("Hello this is text"), 10, 10, 40, GREEN);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
