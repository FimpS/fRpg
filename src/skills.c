#include "skills.h"
#include "entity.h"
#include "struct.h"
#include "enum.h"



void skills_use_skill(Skill skill, GameState* state, Entity* target)
{
	if(skill.data.type == SKILL_TYPE_NONE) return;
	if(!skill.data.castable) return;

	skill.data.activate(&skill, state, target);

}

Skill skills_skill_init(SkillType type)
{
	return (Skill) { 
		.data = skill_data_table[type],
		.cooldown_timer = skill_data_table[type].base_cooldown + 4,
		.caster = NULL,
	};
}


void skills_placeholder_activate(Skill* skill, GameState* state, Entity* target)
{
	if(target != NULL) return;

	dynList_push(state->map->entities, 
			entity_new(ENTITY_PLACEHOLDER, skill->caster->mid_pos)
		);
}

void skills_fireball_active(Skill* skill, GameState* state, Entity* target)
{
	Vector2 mouse_pos = map_get_mouse_cords(state->map);

	DynList* ents = state->map->entities;
	const u32 len = ents->len;
	for(i32 i = 0; i < len; i++)
	{
		Entity* ent = dynList_get(ents, i);
		const Rectangle r = { ent->pos.x, ent->pos.y, ent->data.dim.x, ent->data.dim.y };
		if(AAB(r, mouse_pos))
		{
			entity_do_damage(state->player->entity, ent, skill->data.flat_damage, skill->data.mult_damage,state);
		}
	}
}

Rectangle skills_get_skill_src(SkillType type, CharacterClassType class)
{
	const u32 length = 16;
	return (Rectangle) { type * length, class * length, length, length };
}


const SkillData skill_data_table[] = {
	//Type 							castable	BaseCD				activate							//flat
	{SKILL_TYPE_NONE, 				false, 		0000,					NULL,							{0, 100, 0, 0, 0, 0},		{1,1,1,1,1,1}},
	{SKILL_TYPE_PLACEHOLDER, 		true, 		0010,					skills_fireball_active},
	{SKILL_TYPE_FIREBALL, 			true, 		0060,					skills_fireball_active,			{0, 300, 0, 0, 0, 0},		{0,50,0,0,0,0}},
	{SKILL_TYPE_ONE, 				false, 		0200,					NULL},
	{SKILL_TYPE_TWO, 				false, 		1000,					NULL},
	{SKILL_TYPE_THREE, 				false, 		1000,					NULL},
	{SKILL_TYPE_FOUR, 				false, 		1000,					NULL},
};









