#pragma once

#include "raylib.h"
#include "../lib/dynList.h"
#include "../lib/types.h"

#include "map.h"
#include "gfx.h"
#include "ui.h"
#include "entity.h"
#include "struct.h"

GameState* state_new();

void state_tick(GameState* state);
void state_render(GameState* state);

