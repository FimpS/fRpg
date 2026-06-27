
#include "entity_states.h"
#include "state.h"
#include "map.h"
#include "entity.h"
#include "sound.h"

void estate_placeholder_tick(Entity* self, GameState* state)
{
	if(IsKeyPressed(KEY_F1))
	{
		P_LOG("%d %d\n", self->type, self->state.type);
	}
}


void estate_player_tick(Entity* self, GameState* state)
{
	Map* map = state->map;
	WalkPath* path = &self->path;
	const Vector2 target = path->pos[path->current];

	if(IsKeyPressed(KEY_H)) play_global_sound(state, SOUND_GLOBAL_WOOSH2);


	entity_player_determine_movement_direction(self, state);
	entity_move(self, state);
}

void estate_entity_move_attack(Entity* self, GameState* state)
{
	if(self->state.timer >= self->state.stop_timer)
	{
		self->target = state->player->entity;
		self->path = entity_get_path(self, state, Vector2Midpoint(self->target->pos, self->target->data.dim));
		self->state = entity_state_table[ESTYPE_ENTITY_MOVE_ATTACK];
		return;
	}
	if(entity_in_range(entity_get_midpoint(self), entity_get_midpoint(self->target), 1.0))
	{
		self->state = entity_state_table[ESTYPE_ENTITY_PERFORM_MELEE];
		return;
	}
	entity_move_along_path(self, state);

}

void estate_entity_perform_melee(Entity* self, GameState* state)
{
	if(self->state.timer >= self->state.stop_timer)
	{
		//find new target
		if(entity_in_range(entity_get_midpoint(self), entity_get_midpoint(self->target), 2.0) )
		{
			play_global_sound(state, SOUND_GLOBAL_WOOSH2);
			//P_LOG("Damage Dealt\n");
		} else 1;//P_LOG("Missed Attack\n");
		self->path = entity_get_path(self, state, entity_get_midpoint(self->target));
		self->state = entity_state_table[ESTYPE_ENTITY_MOVE_ATTACK];
	}
}

//Separate state and animationState, so one entity can use move_attack but not use the same sprite?
// Maybe not needed? just copy the function pointer, no
const EntityState entity_state_table[] = {
//	Type									t 	st 				tick									anim		af		frames
	{ESTYPE_PLACEHOLDER,					0,	0,				estate_placeholder_tick,				{0, 60, 	2,		{ {0,0,16,16}, {0,16,16,16} } } },
	{ESTYPE_PLACEHOLDER2,					0,	0,				estate_placeholder_tick,				{0, 60, 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
	{ESTYPE_PLAYER_TICK,					0,	0,				estate_player_tick,						{0, 0,	 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
	{ESTYPE_ENTITY_MOVE_ATTACK,				0,	60,				estate_entity_move_attack,				{0, 60, 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
	{ESTYPE_ENTITY_PERFORM_MELEE,			0,	30,				estate_entity_perform_melee,			{0, 60, 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
};













