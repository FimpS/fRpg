#pragma once

#define MAX_WALK_PATH_LEN 64
#define ASTAR_MINHEAP_MAX_LEN 1024

//#include "../include/state.h"
//#include "../include/entity.h"
#include "../include/global.h"

typedef struct GameState GameState;
typedef struct Entity Entity;
typedef struct Tile Tile;

typedef struct AStarNode
{
	Vector2 pos;

	f32 global_goal;
	f32 local_goal;

	bool solid;
	bool visited;

	u8 open;

	Vector2 parent;
} AStarNode;

typedef struct AStarMinHeap
{
	u32 max_len;
	u32 len;
	AStarNode* nodes[ASTAR_MINHEAP_MAX_LEN];
} AStarMinHeap;

typedef struct WalkPath
{
	Vector2 pos[MAX_WALK_PATH_LEN];
	u32 count;
	u32 current;
} WalkPath;

WalkPath path_get_any_path(Entity* self, Vector2 end_pos, GameState* state);
WalkPath path_get_line_path(Entity* self, Vector2 end);
bool path_validate(WalkPath path, Tile* tiles, int width, int height);
bool path_line_of_sight(Entity* self, Vector2 end, GameState* state);
