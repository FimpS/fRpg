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
	ENTITY_LAST,
} EntityType;

typedef enum EntityStateType
{
	ESTYPE_PLACEHOLDER,
	ESTYPE_PLACEHOLDER2,
	ESTYPE_PLAYER_TICK,
	ESTYPE_ENTITY_MOVE_ATTACK,
	ESTYPE_ENTITY_PERFORM_MELEE,
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
	INVENTORY_MODE_NONE,
	INVENTORY_MODE_MOVE,
	INVENTORY_MODE_DELETE,
} InventoryMode;

typedef enum SoundTypeGlobal
{
	SOUND_GLOBAL_WOOSH,
	SOUND_GLOBAL_WOOSH2,
} SoundTypeGlobal;

