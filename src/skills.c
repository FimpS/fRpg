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





const SkillData skill_data_table[] = {
	{SKILL_TYPE_PLACEHOLDER, 		true, 		skills_placeholder_activate},
	{SKILL_TYPE_FIREBALL, 			false, 		NULL},
};
