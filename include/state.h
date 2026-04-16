#ifndef STATE_H
#define STATE_H

#include "raylib.h"
#include "../lib/dynList.h"
#include "../lib/types.h"

#include "../include/map.h"
#include "../include/gfx.h"
#include "../include/ui.h"
#include "../include/entity.h"

//Vector2 v2(f32 x, f32 y) { return (Vector2) {x, y}; }

typedef struct Map Map;
typedef struct Gfx Gfx;
typedef struct Entity Entity;
typedef struct Inventory Inventory;

typedef struct GameState
{
	Map* map;
	Gfx* gfx;
	Inventory* inventory;

	Entity* player;
} GameState;

GameState* state_new();

void state_tick(GameState* state);
void state_render(GameState* state);

#endif
