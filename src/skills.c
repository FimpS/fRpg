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


Rectangle skills_get_skill_src(SkillType type, CharacterClassType class)
{
	const u32 length = 16;
	return (Rectangle) { type * length, class * length, length, length };
}


const SkillData skill_data_table[] = {
	//Type 							castable	BaseCD				activate
	{SKILL_TYPE_NONE, 				false, 		0000,					NULL},
	{SKILL_TYPE_PLACEHOLDER, 		true, 		6000,					skills_placeholder_activate},
	{SKILL_TYPE_FIREBALL, 			false, 		0060,					NULL},
	{SKILL_TYPE_ONE, 				false, 		0200,					NULL},
	{SKILL_TYPE_TWO, 				false, 		1000,					NULL},
	{SKILL_TYPE_THREE, 				false, 		1000,					NULL},
	{SKILL_TYPE_FOUR, 				false, 		1000,					NULL},
};
