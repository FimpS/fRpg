#include "quest_data.h"


const QuestData quest_data_table[] = {
// 	QuestType							QuestClass					len		Objectives
	{QUEST_TYPE_PLACEHOLDER,			QUEST_CLASS_RED,			"Placeholder",				2,		{
																								{QUEST_OBJECTIVE_KILL,				10,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
																								{QUEST_OBJECTIVE_TALK,				1,				{ .talk = {ENTITY_SHOP_NPC}		} }, 
	} },
	{QUEST_TYPE_KILL_5_IMPS,			QUEST_CLASS_YELLOW, 		"Clear the way!"			,2,		{
																								{QUEST_OBJECTIVE_KILL,				5,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
																								{QUEST_OBJECTIVE_KILL,				10,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
	} },

};

const NPCQuestDataTypes npc_quest_data_types_table[] = {
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_PLACEHOLDER, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_PLACEHOLDER, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 1, .types= { QUEST_TYPE_KILL_5_IMPS, } },
};
