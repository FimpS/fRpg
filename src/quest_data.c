#include "quest_data.h"


const QuestPrerequisite quest_prerequisite_table[] = {
// 	PlayerLevel		len				QuestTypes
	{0, 			1, 				{ 	QUEST_TYPE_PLACEHOLDER,	} },
	{0, 			0, 				{	0, },	},
};

const QuestReward quest_reward_table[] = {
// 	Experience						len				QuestRewardItems
	{1500,							0,				{	ITEM_TYPE_NONE,	},	},
	{150,							2,				{	ITEM_TYPE_HELMET,	ITEM_TYPE_PLACEHOLDER	},	},
};

const QuestData quest_data_table[] = {
// 	QuestType							QuestClass					name								len			QuestObjectiveTypes
	{QUEST_TYPE_PLACEHOLDER,			QUEST_CLASS_RED,			"Placeholder",						2,			{
																													{QUEST_OBJECTIVE_KILL,				10,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
																													{QUEST_OBJECTIVE_TALK,				1,				{ .talk = {NPC_TYPE_TOWN_MERCHANT}	} }, 
																													},
	},
	{QUEST_TYPE_KILL_5_IMPS,			QUEST_CLASS_YELLOW, 		"Clear the way!",					2,			{
																													{QUEST_OBJECTIVE_KILL,				5,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
																													{QUEST_OBJECTIVE_KILL,				10,				{ .kill = {ENTITY_PLACEHOLDER} 		} },
	}, 																												},
};

const NPCQuestDataTypes npc_quest_data_types_table[] = {
	(NPCQuestDataTypes) { 4, .types= { QUEST_TYPE_KILL_5_IMPS, QUEST_TYPE_KILL_5_IMPS, QUEST_TYPE_KILL_5_IMPS, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 2, .types= { QUEST_TYPE_PLACEHOLDER, QUEST_TYPE_PLACEHOLDER } },
	(NPCQuestDataTypes) { 1, .types= { QUEST_TYPE_KILL_5_IMPS, } },
};

// Above and below should match in size of the QuestType and TextType array 

const NPCTextData npc_text_data_table[] = {
// 	Generic							Quests				Information								Completion
	{TEXT_TYPE_PLACEHOLDER,			{	
										{				TEXT_TYPE_IMPS_RAVAGING, 				TEXT_TYPE_PLACEHOLDER},
										{				TEXT_TYPE_IMPS_RAVAGING, 				TEXT_TYPE_PLACEHOLDER},
										{				TEXT_TYPE_IMPS_RAVAGING, 				TEXT_TYPE_PLACEHOLDER},
										{				TEXT_TYPE_PLACEHOLDER,					TEXT_TYPE_PLACEHOLDER},
									}
	},
	{TEXT_TYPE_PLACEHOLDER,			{	
										{				TEXT_TYPE_IMPS_RAVAGING, 				TEXT_TYPE_PLACEHOLDER},
										{				TEXT_TYPE_PLACEHOLDER,					TEXT_TYPE_PLACEHOLDER},
									}
	},
	{TEXT_TYPE_PLACEHOLDER,			{	
										{				TEXT_TYPE_PLACEHOLDER2, 				TEXT_TYPE_PLACEHOLDER},
										{				TEXT_TYPE_PLACEHOLDER,					TEXT_TYPE_PLACEHOLDER},
									}
	},

};


