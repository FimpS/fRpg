#pragma once

#include "global.h"
#include "gfx.h"
#include "../lib/types.h"
#include "../lib/dynList.h"
#include "../lib/statList.h"

typedef struct Map Map;
typedef struct Skill Skill;
typedef struct Entity Entity;
typedef struct SkillTreeNode SkillTreeNode;
typedef struct SkillTree SkillTree;
typedef struct GameState GameState;
typedef struct Inventory Inventory;

/* ITEM */

#define MAX_NAME_LEN 32
#define MAX_DESC_LEN 128
typedef struct ItemInfo
{
	u8 name[MAX_NAME_LEN];
	u8 description[MAX_DESC_LEN];
	ItemClass class;
	bool stackable;
} ItemInfo;

typedef struct ItemEnchant
{
	i32 level; //maybe this is a ratio of the level of the enemy?
} ItemEnchant;

typedef struct Item
{
	ItemType type;
	ItemInfo data;
	ItemEnchant enchant;
} Item;

/* ITEM */

/* Equipment */

typedef struct UIEquipmentMenu
{
	bool active;
} UIEquipmentMenu;

#define MAX_SLOTS_IN_EQUIPMENT 8
typedef struct EntityEquipment
{
	Item items[MAX_SLOTS_IN_EQUIPMENT];
} EntityEquipment;

/* Equipment */

/* PATH */

#define MAX_WALK_PATH_LEN 64
typedef struct WalkPath
{
	Vector2 pos[MAX_WALK_PATH_LEN];
	u32 count;
	u32 current;
} WalkPath;

/* PATH */

/* ENTITY */

#define MAX_ENTITY_ANIMATION_FRAMES 8
typedef struct EntityAnimation
{
	u32 timer;
	u32 stop_timer;
	u32 amount_frames;
	Rectangle frames[MAX_ENTITY_ANIMATION_FRAMES];
} EntityAnimation;

typedef struct EntitySprite
{
	Rectangle rec_bmap;
} EntitySprite;

typedef struct EntityState
{
	EntityStateType type;
	i32 timer;
	i32 stop_timer;
	void (*tick)(Entity* self, GameState* state);
	EntityAnimation animation;
} EntityState;

typedef struct EntityGearData
{
	//Defense defense_flat;
	//Defense defense_mult;
	f32 speed_flat;
	f32 speed_mult;
} EntityGearData;

typedef struct EntityBuffs //calculate this before speed (very early in tick)
{
	f32 speed_flat;
	f32 speed_mult;
} EntityBuffs;


typedef struct LightPulse
{
	bool enable;
	f32 depth;
	f32 speed;
} LightPulse;
typedef struct LightFlicker 
{
	bool enable;
	f32 min;
	f32 max;
	i32 frequency;
} LightFlicker;
typedef struct EntityLightData
{
	f32 value;
	f32 distance;
	bool light_source;
	LightPulse pulse;
	LightFlicker flicker;
	Color tint;
} EntityLightData;

typedef struct EntityData
{
	EntityType type;
	const u8* name;
	EntityStateType start_state;
	TextureIndex spritesheet_index;

	Vector2 dim;
	f32 base_speed;
	EntityLightData light;
} EntityData;

typedef struct EntityStats
{
	i32 level;
	u64 experience;
} EntityStats;

//Decent idea to have separate every Data/changing so you have LighData and Light
typedef struct Entity
{
	Vector2 pos;
	Vector2 mid_pos;

	WalkPath path;

	EntityType type;
	EntityState state;

	EntityStats stats;
	EntityEquipment* equipment;

	f32 speed;
	f32 facing_angle;

	f32 aggro_range;
	Entity* target;

	i32 light_frequency;

	EntityData data;
	u32 id;

} Entity;


#define MAX_SHOP_ITEMS 64
typedef struct ShopData
{
	u32 total_item;
	ItemType shop_item_types[MAX_SHOP_ITEMS];
} ShopData;

typedef union QuestObjectiveData
{
	struct
	{
		EntityType kill_type;
	} kill;

	struct
	{
		NPCType talk_to_type;	
	} talk;
} QuestObjectiveData;

typedef struct QuestObjective
{
	QuestObjectiveType type;
	u32 target;

	QuestObjectiveData objective;

} QuestObjective;

#define MAX_ITEM_QUEST_REWARDS 16
typedef struct QuestReward
{
	u32 experience;
	u32 item_rewards_len;
	ItemType item_rewards[MAX_ITEM_QUEST_REWARDS];
} QuestReward;

#define MAX_PREREQ_QUESTS 8
DEFINE_STATIC_LIST(QuestType, QuestPreList, quest_pre_list, MAX_PREREQ_QUESTS);
typedef struct QuestPrerequisite
{
	i32 player_level;

	QuestPreList quests;
	//u32 quests_len;
	//QuestType quests[MAX_PREREQ_QUESTS];
} QuestPrerequisite;

#define MAX_QUEST_OBJECTIVES 8
typedef struct QuestData
{
	QuestType type;
	QuestClass class;

	const u8* name;

	u32 objectives_len;
	QuestObjective objectives[MAX_QUEST_OBJECTIVES];
} QuestData;

#define MAX_QUESTS_ALLOWED 16
typedef struct NPCQuestData //TODO why does the NPC have the actual data, NPCQuestDataTypes should be enough to push them onto the questmanager?
{
	u32 len;
	QuestData data[MAX_QUESTS_ALLOWED];	
	bool completed[MAX_QUESTS_ALLOWED];
} NPCQuestData;
typedef struct NPCQuestDataTypes
{
	u32 len;
	QuestType types[MAX_QUESTS_ALLOWED];	
} NPCQuestDataTypes;

#define MAX_NPC_TALK_OPTIONS 8
typedef struct NPCMenu
{
	bool active;
	bool talking;
	bool quest_menu_active;
	i8 choice;
	i8 quest_choice;

	u32 len;
	const u8* option_strings[MAX_NPC_TALK_OPTIONS];
} NPCMenu;

typedef struct NPCTextData 
{
	TextType generic;
	struct 
	{
		TextType information;
		TextType completion;
	} quests[MAX_QUESTS_ALLOWED];

} NPCTextData;

typedef struct NPCData 
{
	EntityType entity;
	InventoryType inventory;
} NPCData;

typedef struct NPC
{
	NPCType type;
	NPCData data;

	NPCMenu menu;
	Entity* entity;
	Inventory* inventory;
	NPCTextData text;
	NPCQuestData quests;
} NPC;

typedef struct Quest
{
	u32 quest_counters[MAX_QUEST_OBJECTIVES];
	QuestData data;

	NPCType quest_giver;
	QuestStatus status;
} Quest;

#define MAX_QUESTS_IN_MANAGER 16
DEFINE_STATIC_LIST(Quest, QuestManagerList, quest_manager_list, MAX_QUESTS_IN_MANAGER);
typedef struct QuestManager
{
	//u32 len;
	//Quest quests[MAX_QUESTS_IN_MANAGER];
	QuestManagerList quests;

	struct {
		QuestType type;
		bool completed;
	} completed[MAX_QUESTS_IN_GAME];
} QuestManager;

typedef struct CharacterClass
{
	CharacterClassType class_1;
	CharacterClassType class_2;
	CharacterClassType class_3;
} CharacterClass;

typedef struct Player
{
	Entity* entity;
	Inventory* inventory;
	Skill* skill;
	QuestManager* quest_manager;
	CharacterClass class;
	SkillTree* skill_tree;
} Player;



/* ENTITY */

/* TEXT */

typedef struct TextData
{
	const u8* text;
	TextType type;
	f32 speed;
} TextData;

/* TEXT */


/* MAP */

typedef struct MapCamera
{
	Vector2 pos;
	V2 visible_tiles;
	Vector2 offset;
	Vector2 tile_offset;
	u32 tile_len;
	f32 zoom;
	f32 speed;
} MapCamera;

typedef struct Tile
{
	i32 type;
	f32 light;
	f32 light_level;
	bool animated;
	bool solid;
} Tile;

typedef struct LightSettings
{
	f32 ambient_light;
	Color fade;
} LightSettings;

#define MAX_SOUNDS 192
#define MAX_SOUND_MULTIPLE 8

typedef struct SoundMultiple
{
	Sound sound[MAX_SOUND_MULTIPLE];
	u32 counter;
} SoundMultiple; 
typedef struct SoundManager
{
	SoundMultiple* sound_pool;
	u32 len;
} SoundManager;

typedef struct Map
{
	MapCamera* camera;
	Tile* content;
	DynList* entities;
	DynList* npcs;
	//SoundManager* sound;
	LightSettings light_settings;
	V2 dim;	
} Map;

/* MAP */

/* SKILLS */

typedef struct Skill Skill;



#define PLAYER_MAX_SKILL_TREES 3
typedef struct SkillTree
{
	bool active;
	CharacterClassType active_skill_tree;
	SkillTreeNode* holding_node;
	SkillTreeNode* root[PLAYER_MAX_SKILL_TREES];

	i32 skill_points;
} SkillTree;



#define MAX_SKILL_TREE_ADJACENT 4
typedef struct SkillTreeNode SkillTreeNode;
typedef struct SkillTreeNodeData
{
	SkillType type;
	i32 level;
	i32 skill_point_cost;
	Vector2 pos;
	SkillType neighbors[MAX_SKILL_TREE_ADJACENT];
} SkillTreeNodeData;

DEFINE_STATIC_LIST(SkillTreeNodeData, SkillTreeClassList, skill_tree_class_list, MAX_SKILLS_IN_SKILLTREE);
typedef struct SkillTreeNodeDataClass
{
	SkillTreeClassList class_list;
} SkillTreeNodeDataTable;

typedef struct SkillTreeNode
{
	SkillType type;
	i32 level;
	i32 skill_point_cost;
	Vector2 pos;
	SkillTreeNode* neighbors[MAX_SKILL_TREE_ADJACENT];
} SkillTreeNode;

typedef struct SkillData
{
	SkillType type;
	bool castable;
	u32 base_cooldown;
	void (*activate)(Skill* skill, GameState* state, Entity* target);
} SkillData;

typedef struct Skill
{
	SkillData data;
	u32 cooldown_timer;
	Entity* caster;
} Skill;

#define MAX_HOTBAR_SKILLS 8
typedef struct SkillHotBar
{
	//SkillType skills[MAX_HOTBAR_SKILLS];	//TODO this should be gone
	Skill skills[MAX_HOTBAR_SKILLS];
	Rectangle start_location;
} SkillHotBar;

/* SKILLS */

/* UI */

typedef struct UIElements
{
	SkillHotBar hotbar;
	UIEquipmentMenu equipment;
} UIElements;

typedef struct TextDisplay
{
	//const u8* text;
	TextData data;
	NPC* owner;
	
	u32 text_offset;
	u32 reveal_timer;
	u32 timer;
	u32 stop;
} TextDisplay;




#define TOTAL_INVENTORY_CELLS 60
#define TOTAL_INVENTORY_BUTTONS 2

typedef struct InventoryData 
{
	void (*tick)(Inventory* inventory, GameState* state);
	Rectangle hitbox;
	u32 rows;
	u32 cols;
	V2 cell_offset;
} InventoryData;

typedef struct InventoryCell
{
	Item item;
	Rectangle hitbox;
	i32 focused;
	u32 id;
	bool stackable; //maybe not usefull
	u32 amount;
} InventoryCell;


typedef struct InventoryButton
{
	Rectangle hitbox;
	Rectangle src_rec;
	Rectangle hover_src_rec;
	bool hovered;
	void (*on_click)(Inventory* inventory, GameState* state);
} InventoryButton;

typedef struct Inventory
{
	InventoryCell cells[TOTAL_INVENTORY_CELLS]; //TODO malloc these instead
	Rectangle hitbox;
	u32 cols;
	u32 rows;
	V2 cell_offset;


	i32 focus_id;
	i32 moved_id;
	InventoryMode mode;
	bool active;

	void (*tick)(Inventory* inventory, GameState* state);

	InventoryButton buttons[TOTAL_INVENTORY_BUTTONS]; //Same here
} Inventory;

#define MAX_TEXTS_IN_UI_QUEUE 16
DEFINE_STATIC_LIST(TextDisplay, UITextQueue, ui_text_queue, MAX_TEXTS_IN_UI_QUEUE);
typedef struct UIQueues
{
	UITextQueue text_queue;
} UIQueues;

/* UI */

/* State */

typedef struct GameState
{
	Map* map;
	SoundManager* global_sound_manager;
	UIQueues* ui;
	Gfx* gfx;

	UIElements* ui_elements;

	Player* player;

} GameState;

/* State */
