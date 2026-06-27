#pragma once

#include "raylib.h"
#include "raymath.h"
#include "types.h"
#include "gfx.h"
#include "state.h"
#include "map.h"
#include "path.h"
#include "struct.h"


extern const EntityState entity_state_table[];

bool entity_AAB(Entity* e, Vector2 p);

Entity* entity_new(EntityType type, Vector2 pos);
Entity* entity_new_editor(EntityType type, Vector2 pos);

void entity_destroy(Entity* e);

void entities_tick(DynList* entities, GameState* state);
void entity_render(Entity* self, GameState* state);
void entities_render(DynList* entities, GameState* state);
void entity_move(Entity* self, GameState* state);

WalkPath entity_get_path(Entity* self, GameState* state, const Vector2 end);
void entity_move_along_path(Entity* self, GameState* state);
void entity_handle_standard_pathing(Entity* self, GameState* state, bool condition);
f32 entity_calculate_speed(Entity* self, GameState* state);
void entity_player_determine_movement_direction(Entity* self, GameState* state);
Vector2 entity_get_midpoint(Entity* self);
bool entity_in_range(Vector2 p, Vector2 u, f32 range);
void entity_move(Entity* self, GameState* state);
bool entity_path_blocked(Vector2 pos, GameState* state);
f32 entity_calculate_speed(Entity* self, GameState* state);

Entity* entity_player_init(GameState* state);

