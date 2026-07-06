#include "quest_data.h"


const QuestData quest_data_table[] = {
// 	QuestType							QuestClass					len		Objectives
	{QUEST_TYPE_PLACEHOLDER,			QUEST_CLASS_RED,			2,		{
																			{QUEST_OBJECTIVE_KILL,				{ .kill = {10,ENTITY_PLACEHOLDER} 		} },
																			{QUEST_OBJECTIVE_TALK,				{ .talk = {false, ENTITY_SHOP_NPC}		} }, 
	} },
	{QUEST_TYPE_KILL_5_IMPS,			QUEST_CLASS_YELLOW, 		1,		{
																			{QUEST_OBJECTIVE_KILL,				{ .kill = {5,ENTITY_PLACEHOLDER2} 		} },
	} },

};

const NPCQuestDataTypes npc_quest_data_types_table[] = {
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_PLACEHOLDER, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_PLACEHOLDER, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 1, .types= { QUEST_TYPE_KILL_5_IMPS, } },
};
