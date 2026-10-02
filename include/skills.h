#pragma once

#include "struct.h"
#include "enum.h"

extern const SkillData skill_data_table[];


void skills_use_skill(Skill skill, GameState* state, Entity* target);
Rectangle skills_get_skill_src(SkillType type, CharacterClassType class);


