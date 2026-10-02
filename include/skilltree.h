#pragma once

#include "struct.h"
#include "enum.h"


//extern const SkillTreeNodeData warlock_skill_tree[];

void skill_tree_render(SkillTreeNode* root, GameState* state);

SkillTree* skill_tree_new();
SkillTreeNode* skill_tree_warlock_tree();
SkillTreeNode* skill_tree_get_class_tree();
