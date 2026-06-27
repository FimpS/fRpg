#pragma once

#include "raylib.h"
#include "../lib/dynList.h"
#include "../lib/types.h"

#include "map.h"
#include "npc.h"
#include "gfx.h"
#include "ui.h"
#include "entity.h"
#include "struct.h"

GameState* state_new();

//SoundMultiple* state_get_global_sound(GameState* state, SoundTypeGlobal index);
void play_global_sound(GameState* state, SoundTypeGlobal index);
void state_tick(GameState* state);
void state_render(GameState* state);

