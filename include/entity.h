#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "raymath.h"
#include "../lib/types.h"
#include "../include/gfx.h"
#include "../include/state.h"
#include "../include/map.h"

#define MAX_ENTITY_ANIMATION_FRAMES 8

typedef struct GameState GameState;
typedef struct Entity Entity;



typedef struct AStarNode
{
	Vector2 pos;

	f32 global_goal;
	f32 local_goal;

	bool walkable;
	bool solid;
	bool visited;

	Vector2 parent;
} AStarNode;
#define MAX_WALK_PATH_LEN 64
typedef struct WalkPath
{
	Vector2 pos[MAX_WALK_PATH_LEN];
	u32 count;
	u32 current;
} WalkPath;

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
	ESTYPE_CLEAR,

} EntityStateType;

typedef struct EntityAnimation
{
	Rectangle frames[MAX_ENTITY_ANIMATION_FRAMES];
	u32 amount_frames;
	u32 timer;
	u32 stop_timer;
} EntityAnimation;

typedef struct EntitySprite
{
	Rectangle rec_bmap;
} EntitySprite;

typedef struct EntityState
{
	EntityStateType type;
	void (*tick)(Entity* self, GameState* state);
	EntityAnimation animation;
	WalkPath path;
} EntityState;

typedef struct Entity
{
	Vector2 pos;
	Vector2 dim;

	WalkPath path;

	Light light;
	EntityType type;
	EntityState state;

	u32 id;

} Entity;

typedef struct PlayerEntity
{
	Entity entity;
	
} PlayerEntity;

bool entity_AAB(Entity* e, Vector2 p);

Entity* entity_new(EntityType type, Vector2 pos);
Entity* entity_new_editor(EntityType type, Vector2 pos);

void entity_destroy(Entity* e);

void entities_tick(DynList* entities, GameState* state);
void entities_render(DynList* entities, GameState* state);

Entity* entity_player_init(GameState* state);

WalkPath entity_find_path(Entity* self, Vector2 end_pos, GameState* state);
#endif
