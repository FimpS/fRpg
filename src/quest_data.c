#include "quest_data.h"


const QuestData quest_data_table[] = {
// 	QuestType							QuestClass					name						len		Objectives																						Experience		len			ItemRewards
	{QUEST_TYPE_PLACEHOLDER,			QUEST_CLASS_RED,			"Placeholder",				2,		{
																								{QUEST_OBJECTIVE_KILL,				10,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
																								{QUEST_OBJECTIVE_TALK,				1,				{ .talk = {NPC_TYPE_TOWN_MERCHANT}	} }, 
		}, 																																																{1501,			0,  		{ ITEM_CLASS_NONE } }, {0, 1, { QUEST_TYPE_PLACEHOLDER, } },
	},
	{QUEST_TYPE_KILL_5_IMPS,			QUEST_CLASS_YELLOW, 		"Clear the way!",			2,		{
																								{QUEST_OBJECTIVE_KILL,				5,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
																								{QUEST_OBJECTIVE_KILL,				10,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
		}, 																																																{150,			2,  		{ ITEM_TYPE_HELMET, ITEM_TYPE_PLACEHOLDER } }, {0, 0, { 0, } },
	},

};

const NPCQuestDataTypes npc_quest_data_types_table[] = {
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_KILL_5_IMPS, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_PLACEHOLDER, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 1, .types= { QUEST_TYPE_KILL_5_IMPS, } },
};
