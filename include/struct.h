#pragma once

#include "global.h"
#include "gfx.h"
#include "../lib/types.h"
#include "../lib/dynList.h"

typedef struct Map Map;
typedef struct Entity Entity;
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
	ItemInfo info;
	ItemEnchant enchant;
} Item;

/* ITEM */

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
	EntityStateType start_state;
	TextureIndex spritesheet_index;

	Vector2 dim;
	f32 base_speed;
	EntityLightData light;
} EntityData;

//Decent idea to have separate every Data/changing so you have LighData and Light
typedef struct Entity
{
	Vector2 pos;
	Vector2 mid_pos;

	WalkPath path;

	EntityType type;
	EntityState state;
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

typedef struct QuestObjective
{
	QuestObjectiveType type;
	union
	{
		struct
		{
			u32 kill_count;
			EntityType kill_type;
		} kill;

		struct
		{
			bool talk_to;
			NPCType talk_to_type;	
		} talk;
	} objective;
} QuestObjective;

#define MAX_QUEST_OBJECTIVES 8
typedef struct QuestData
{
	QuestType type;
	QuestClass class;

	//TODO figure out a way for prerequesites
	u32 objectives_len;
	QuestObjective objectives[MAX_QUEST_OBJECTIVES];
} QuestData;

#define MAX_QUESTS_ALLOWED 16
typedef struct NPCQuestData
{
	u32 len;
	QuestData data[MAX_QUESTS_ALLOWED];	
} NPCQuestData;
typedef struct NPCQuestDataTypes
{
	u32 len;
	QuestType types[MAX_QUESTS_ALLOWED];	
} NPCQuestDataTypes;

typedef struct NPC
{
	NPCType type;
	Entity* entity;
	Inventory* inventory;
	NPCQuestData quests;
} NPC;

typedef struct Quest
{
	QuestData data;
	QuestStatus status;
} Quest;

#define MAX_QUESTS_IN_MANAGER 16
typedef struct QuestManager
{
	u32 len;
	Quest quests[MAX_QUESTS_IN_MANAGER];
} QuestManager;

typedef struct Player
{
	Entity* entity;
	Inventory* inventory;
	QuestManager* quest_manager;
} Player;



/* ENTITY */

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

/* UI */

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

/* UI */

/* State */

typedef struct GameState
{
	Map* map;
	SoundManager* global_sound_manager;
	Gfx* gfx;


	Player* player;
} GameState;

/* State */
