#ifndef STATE_H
#define STATE_H

#include "raylib.h"
#include "../lib/dynList.h"
#include "../lib/types.h"

//Vector2 v2(f32 x, f32 y) { return (Vector2) {x, y}; }

typedef struct GameState
{
	DynList* entities;
} GameState;

GameState* state_new();

#endif
