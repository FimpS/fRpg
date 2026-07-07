#pragma once

typedef enum MapSoundIndex
{
	MAPSOUND_DEFAULT,
	MAPSOUND_LAVA,
} MapSoundIndex;

typedef enum TextureIndex
{
	TEXTURE_TILEMAP,
	TEXTURE_EDITOR_UI,
	TEXTURE_GAME_UI,
} TextureIndex;

typedef enum EntityType
{
	ENTITY_PLACEHOLDER,
	ENTITY_PLACEHOLDER2,
	ENTITY_PLAYER,
	ENTITY_SHOP_NPC,
	ENTITY_LAST,
} EntityType;

typedef enum NPCType
{
	NPC_TYPE_TOWN_MERCHANT,
	NPC_TYPE_CAVE_MERCHANT,
	NPC_TYPE_TOWN_SMITH,
} NPCType;

typedef enum NPCShopType
{
	NPC_SHOP_TOWN,
	NPC_SHOP_CAVE,
} NPCShopType;

typedef enum NPCQuestType
{
	NPC_QUEST_TOWN_QUESTER,
	NPC_QUEST_CAVE_QUESTER,
} NPCQuestType;

typedef enum QuestType
{
	QUEST_TYPE_PLACEHOLDER,
	QUEST_TYPE_KILL_5_IMPS,
} QuestType;

typedef enum QuestClass
{
	QUEST_CLASS_RED,
	QUEST_CLASS_YELLOW,
	QUEST_CLASS_GREEN,
	QUEST_CLASS_BLUE,
} QuestClass;

typedef enum QuestObjectiveType
{	
	QUEST_OBJECTIVE_KILL,
	QUEST_OBJECTIVE_TALK,
} QuestObjectiveType;

typedef enum QuestStatus
{
	QUEST_STATUS_ACCEPTED,
	QUEST_STATUS_COMPLETE,
	QUEST_STATUS_FINISHED,
} QuestStatus;

typedef enum EntityStateType
{
	ESTYPE_PLACEHOLDER,
	ESTYPE_PLACEHOLDER2,
	ESTYPE_PLAYER_TICK,
	ESTYPE_ENTITY_MOVE_ATTACK,
	ESTYPE_ENTITY_PERFORM_MELEE,
	ESTYPE_DEAD,
	ESTYPE_CLEAR,

} EntityStateType;

typedef enum 
{
	ITEM_TYPE_NONE,
	ITEM_TYPE_PLACEHOLDER,
	ITEM_TYPE_HELMET,
} ItemType;

typedef enum
{
	ITEM_CLASS_NONE,
	ITEM_CLASS_RING,
	ITEM_CLASS_HELMET,
} ItemClass;

typedef enum TileType //Not needed probably, But some kind of list of what A tile should look like idk last part to think about...
{
	TILETYPE_TEST1 = 1,
	TILETYPE_TEST2,
	TILETYPE_TEST3,
	TILETYPE_TEST4,
} TileType;

typedef enum EntityClass
{
	ENTITYCLASS_NONE,
	ENTITYCLASS_PLACEHOLDER,
} EntityClass;

typedef enum 
{
	INVENTORY_TYPE_PLAYER,
	INVENTORY_TYPE_SHOP,
	INVENTORY_TYPE_SMITH,
} InventoryType;

typedef enum
{
	INVENTORY_MODE_NONE,
	INVENTORY_MODE_MOVE,
	INVENTORY_MODE_DELETE,
} InventoryMode;

typedef enum SoundTypeGlobal
{
	SOUND_GLOBAL_WOOSH,
	SOUND_GLOBAL_WOOSH2,
} SoundTypeGlobal;

