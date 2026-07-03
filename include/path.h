#pragma once

#define ASTAR_MINHEAP_MAX_LEN 1024
#define ASTAR_MAP_CAPACITY 364
#define MAX_NODES 300

//#include "../include/state.h"
//#include "../include/entity.h"

#include "global.h"


typedef struct AStarNode
{
	Vector2 pos;
	Vector2 parent;

	f32 global_goal;
	f32 local_goal;
	bool closed;

	//struct AStarNode* next; may do it
} AStarNode;

typedef struct AStarMinHeap
{
	u32 max_len;
	u32 len;
	AStarNode* nodes[ASTAR_MINHEAP_MAX_LEN];
} AStarMinHeap;

typedef struct AStarNodePool
{
	AStarNode nodes[MAX_NODES];
	i32 len;
} AStarNodePool;

typedef struct AStarMapElement
{
	u32 generation;
	V2 key;
	AStarNode* value;
} AStarMapElement;

typedef struct AStarMap
{
	AStarMapElement entries[ASTAR_MAP_CAPACITY];
} AStarMap;

WalkPath path_get_any_path(Entity* self, Vector2 end_pos, GameState* state);
WalkPath path_get_line_path(Entity* self, Vector2 end);
bool path_validate(WalkPath path, Tile* tiles, int width, int height);
bool path_line_of_sight(Entity* self, Vector2 end, GameState* state);
