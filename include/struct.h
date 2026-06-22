#pragma once

#include "global.h"
#include "gfx.h"
#include "../lib/types.h"
#include "../lib/dynList.h"

typedef struct Map Map;
typedef struct Entity Entity;
typedef struct GameState GameState;

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

#define MAX_SOUNDS 124
#define MAX_SOUND_MULTIPLE 8

typedef struct SoundMultiple
{
	Sound sound[MAX_SOUND_MULTIPLE];
	u32 counter;
} SoundMultiple; 
typedef struct MapSound
{
	SoundMultiple sound_pool[MAX_SOUNDS];
	u32 len;
} MapSound;

typedef struct Map
{
	MapCamera* camera;
	Tile* content;
	DynList* entities;
	MapSound* sound;
	LightSettings light_settings;
	V2 dim;	
} Map;

/* MAP */

/* UI */

#define TOTAL_INVENTORY_CELLS 60
#define TOTAL_INVENTORY_BUTTONS 2
typedef struct InventoryCell
{
	Item item;
	Rectangle hitbox;
	i32 focused;
	u32 id;
	bool stackable; //maybe not usefull
	u32 amount;
} InventoryCell;

typedef struct Inventory Inventory;

typedef struct InventoryButton
{
	Rectangle hitbox;
	Rectangle src_rec;
	Rectangle hover_src_rec;
	bool hovered;
	void (*on_click)(Inventory* inventroy, GameState* state);
} InventoryButton;

typedef struct Inventory
{
	InventoryCell cells[TOTAL_INVENTORY_CELLS];
	Rectangle hitbox;
	u32 cols;
	u32 rows;

	i32 focus_id;
	i32 moved_id;
	InventoryMode mode;

	InventoryButton buttons[TOTAL_INVENTORY_BUTTONS];
} Inventory;

/* UI */

/* State */

typedef struct GameState
{
	Map* map;
	Gfx* gfx;
	Inventory* inventory;

	Entity* player;
} GameState;

/* State */
