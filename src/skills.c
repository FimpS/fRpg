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


void skills_placeholder_activate(Skill* skill, GameState* state, Entity* target)
{
	if(target != NULL) return;

	dynList_push(state->map->entities, 
			entity_new(ENTITY_PLACEHOLDER2, skill->caster->mid_pos)
		);
}


Rectangle skills_get_skill_src(SkillType type, CharacterClassType class)
{
	const u32 length = 16;
	return (Rectangle) { type * length, class * length, length, length };
}


const SkillData skill_data_table[] = { //TODO this needs to be indexed by class or do (classmath indexing this sounds better if it can work)
	//Type 							castable	activate
	//Warlock	
	{SKILL_TYPE_NONE, 				false, 		NULL},
	{SKILL_TYPE_PLACEHOLDER, 		true, 		skills_placeholder_activate},
	{SKILL_TYPE_FIREBALL, 			false, 		NULL},
	{SKILL_TYPE_ONE, 				false, 		NULL},
	{SKILL_TYPE_TWO, 				false, 		NULL},
	{SKILL_TYPE_THREE, 				false, 		NULL},
	{SKILL_TYPE_FOUR, 				false, 		NULL},
	//Warlock2
};
