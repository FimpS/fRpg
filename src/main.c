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

#include <stdio.h>
#include <stdlib.h>

bool ValidateAndPrintPath(WalkPath path, Tile *tiles, int width, int height)
{
    if (path.count == 0)
    {
        printf("Path is EMPTY\n");
        return false;
    }

    printf("---- PATH DEBUG ----\n");

    for (int i = path.count - 1; i >= 0; i--)
    {
        int x = path.pos[i].x;
        int y = path.pos[i].y;

        // Check bounds
        if (x < 0 || y < 0 || x >= width || y >= height)
        {
            printf("❌ Out of bounds at (%d, %d)\n", x, y);
            return false;
        }

        // Check solid
        if (tiles[y * width + x].solid)
        {
            //printf("❌ Path goes through SOLID tile at (%d, %d)\n", x, y);
            //return false;
        }

        printf("Step %d -> (%d, %d)\n", path.count - 1 - i, x, y);

        // Check adjacency (skip first)
        if (i < path.count - 1)
        {
            int px = path.pos[i + 1].x;
            int py = path.pos[i + 1].y;

            int dx = abs(px - x);
            int dy = abs(py - y);

            if (dx + dy > 1)
            {
                printf("❌ Invalid jump from (%d,%d) to (%d,%d)\n", px, py, x, y);
                return false;
            }
        }
    }

    printf("✅ Path is VALID\n");
    printf("--------------------\n");

    return true;
}

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
	//state->player->path = entity_find_path(state->player, (Vector2) {16.0, 16.0}, state);

	ValidateAndPrintPath(state->player->path, state->map->content, 32, 32);

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


