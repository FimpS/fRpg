

#include "struct.h"
#include "state.h"
#include "map.h"
#include "entity.h"


void estate_placeholder_tick(Entity* self, GameState* state);
void estate_player_tick(Entity* self, GameState* state);
void estate_entity_move_attack(Entity* self, GameState* state);
void estate_entity_perform_melee(Entity* self, GameState* state);

const EntityState entity_state_table[];
