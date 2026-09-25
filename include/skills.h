#pragma once

#include "struct.h"
#include "enum.h"

extern const SkillData skill_data_table[];


void skills_use_skill(Skill skill, GameState* state, Entity* target);


