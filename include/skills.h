#pragma once

#include "struct.h"
#include "enum.h"

extern const SkillData skill_data_table[];

Skill skills_skill_init(SkillType type);
void skills_tick_all(Entity* entity, GameState* state);
/* Activates skill, fails if either: not castable, not enough mana, type is none */
void skills_use_skill(Skill* skill, GameState* state, Entity* target);
Rectangle skills_get_skill_src(SkillType type, CharacterClassType class);


