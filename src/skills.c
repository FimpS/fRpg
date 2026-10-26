#include "skills.h"
#include "entity.h"
#include "struct.h"
#include "enum.h"



void skills_use_skill(Skill* skill, GameState* state, Entity* caster)
{
	if(skill->data.type == SKILL_TYPE_NONE) return;
	if(!skill->data.castable) return;
	if(!entity_consume_mana(caster, skill->data.base_cost, state)) return;


	skill->cooldown_timer = 0;
	skill->data.activate(skill, state, caster);

}

Skill skills_skill_init(SkillType type)
{
	return (Skill) { 
		.data = skill_data_table[type],
		.cooldown_timer = skill_data_table[type].base_cooldown + 4,
		.caster = NULL,
	};
}


void skills_placeholder_activate(Skill* skill, GameState* state, Entity* caster)
{
	if(caster != NULL) return;

	dynList_push(state->map->entities, 
			entity_new(ENTITY_PLACEHOLDER, skill->caster->mid_pos)
		);
}

void skills_fireball_active(Skill* skill, GameState* state, Entity* caster)
{
	const Vector2 mouse_pos = map_get_mouse_cords(state->map);

	DynList* ents = state->map->entities;
	const u32 len = ents->len;
	dynList_push(ents, entity_new(ENTITY_PLACEHOLDER2, state->player->entity->pos));

	Entity* target = entity_get_entity_AAB(state, mouse_pos);

	if(target != NULL) 
	{
		entity_do_damage(caster, target, skill->data.flat_damage, skill->data.mult_damage,state);
	}
}

void skills_skill_one_activate(Skill* skill, GameState* state, Entity* caster)
{
	Entity* player = state->player->entity;

	player->speed_multiplier -= 0.75;
	entity_skill_channels_push(&player->skill_channels, *skill);

}


void skills_skill_one_channel(Skill* skill, GameState* state)
{
	if(skill->channel_complete)
	{
		Entity* player = state->player->entity;
		player->speed_multiplier += 0.75;
		return;
	}
}

Rectangle skills_get_skill_src(SkillType type, CharacterClassType class)
{
	const u32 length = 16;
	return (Rectangle) { type * length, class * length, length, length };
}

static void skills_channel_exit(Skill* skill)
{
	if(skill->skill_timer ++ >= skill->data.limit)
	{
		skill->channel_complete = true;
	}
}

void skills_tick_all(Entity* entity, GameState* state)
{
	for(i32 i = 0; i < entity_skill_channels_len(&entity->skill_channels); i++)
	{
		Skill* skill = entity_skill_channels_get(&entity->skill_channels, i);
		skills_channel_exit(skill);
		if(skill != NULL) skill->data.channel(skill, state);
		if(skill->channel_complete) entity_skill_channels_pop_swap(&entity->skill_channels, i);
	}
}

const SkillData skill_data_table[] = {
	//Type 							castable	Mana				BaseCD					activate							//flat
	{SKILL_TYPE_NONE, 				false, 		100,				0000,					NULL,							0,	NULL,	{0, 100, 0, 0, 0, 0},		{1,1,1,1,1,1}},
	{SKILL_TYPE_PLACEHOLDER, 		true, 		100,				0010,					skills_fireball_active},
	{SKILL_TYPE_FIREBALL, 			true, 		10,					0060,					skills_fireball_active,			0,	NULL,	{0, 300, 0, 0, 0, 0},		{0,50,0,0,0,0}},
	{SKILL_TYPE_ONE, 				true, 		40,					1600,					skills_skill_one_activate,							300,			skills_skill_one_channel},
	{SKILL_TYPE_TWO, 				false, 		20,					1000,					NULL},
	{SKILL_TYPE_THREE, 				false, 		20,					1000,					NULL},
	{SKILL_TYPE_FOUR, 				false, 		10,					1000,					NULL},
};









