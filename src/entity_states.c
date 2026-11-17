
#include "entity_states.h"
#include "state.h"
#include "item.h"
#include "path.h"
#include "map.h"
#include "entity.h"
#include "sound.h"

void estate_placeholder_tick(Entity* self, GameState* state)
{
	return;
}

void estate_item_drop_tick(Entity* self, GameState* state)
{
	if(self->target == NULL) return;
	Vector2 mouse_pos = map_get_mouse_cords(state->map);
	Player* player = state->player;
	const u32 pickup_range = 1;
	const u32 move_range = 4;
	const f32 range = Vector2Distance(self->mid_pos, self->target->mid_pos);
	if(entity_hitbox_vector_collision(self, mouse_pos))
	{
		self->data.show_name = true;
	} else self->data.show_name = false;


	if(player->item_collecting)
	{
		if(range <= move_range)
		{
			const Vector2 diff = Vector2Subtract(self->target->pos, self->pos);
			const f32 angle = atan2(diff.y, diff.x);
			const f32 speed = minf32(0.5, 1 / (range * range) * 0.5);
			self->pos = Vector2Add(self->pos, (Vector2) { cos(angle) * speed, sin(angle) * speed });
		}

		if(range <= pickup_range)
		{
			//TODO Handle inventory full
			if(ui_inventory_add_item(state->player->inventory, 
						item_init(*entity_drop_list_get(&self->data.drops, 0), item_get_drop_enchant(self, state) ))
			  )
			{
				//draw closer and not snap
				self->state.type = ESTYPE_DEAD;
			}
			else
			{
				// WARNING TEXT
			}
		}
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
	if(self->target == NULL) return;

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
			entity_do_damage(self, self->target, self->stats.attack, (ElementalPower) { 0 }, state);
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
	{ESTYPE_ITEM_DROP_TICK,					0,	0,				estate_item_drop_tick,					{0, 0,	 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
	{ESTYPE_ENTITY_MOVE_ATTACK,				0,	60,				estate_entity_move_attack,				{0, 60, 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
	{ESTYPE_ENTITY_PERFORM_MELEE,			0,	30,				estate_entity_perform_melee,			{0, 60, 	1,		{ {0,0,16,16}, {0,16,16,16} } } },
};













