
#include "../include/path.h"
#include "../include/map.h"
#include <assert.h>



bool path_validate(WalkPath path, Tile *tiles, int width, int height)
{
    if (path.count == 0)
    {
        printf("Path is EMPTY\n");
        return false;
    }


    for (int i = path.count - 1; i >= 0; i--)
    {
        int x = path.pos[i].x;
        int y = path.pos[i].y;

       printf("Step %d -> (%d, %d)\n", path.count - 1 - i, x, y);
    }

    return true;
}

bool path_line_of_sight(Entity* self, Vector2 end, GameState* state)
{
	Map* map = state->map;
	V2 dim = map->dim;
	V2 start_pos = Vector2V2(self->pos);
	V2 end_pos = Vector2V2(end);

	V2 pos_diff = (V2) { abs(end_pos.x - start_pos.x), abs(end_pos.y - start_pos.y) };

	V2 dir = (V2) { (start_pos.x < end_pos.x) ? 1 : -1, (start_pos.y < end_pos.y) ? 1 : -1 };

	i32 error = pos_diff.x - pos_diff.y;

	if( map->content[vector2_to_vector_index(end_pos.x, end_pos.y, dim.x)].solid )
	{
		return false;
	}

	while (true)
	{
		if (start_pos.x < 0 || start_pos.y < 0 || start_pos.x >= dim.x || start_pos.y >= dim.y)
		{
			return false;
		}

		if (map->content[vector2_to_vector_index(start_pos.x, start_pos.y, dim.x)].solid)
		{
			return false;
		}

		if (v2_eq(start_pos, end_pos))
		{
			break;
		}

		const i32 two_error = 2 * error;

		if (two_error > - pos_diff.y)
		{
			error -= pos_diff.y;
			start_pos.x += dir.x;
		}

		if (two_error < pos_diff.x)
		{
			error += pos_diff.x;
			start_pos.y += dir.y;
		}
	}

	return true;
}

WalkPath path_get_line_path(Entity* self, Vector2 end)
{
	return (WalkPath) {
		.pos[0] = Vector2Midpoint(end, Vector2Scale(self->data.dim, -1.0)),
		.count = 1,
		.current = 0,
	};
}



void a_star_min_heap_swap(AStarNode** n1, AStarNode** n2)
{
	AStarNode* tmp = *n1;
	*n1 = *n2;
	*n2 = tmp;
}

void a_star_min_heap_push(AStarMinHeap* heap, AStarNode* node)
{
	heap->nodes[heap->len++] = node;

	i32 index = heap->len - 1;

	while(index > 0)
	{
		i32 parent = (index - 1) >> 1;

		if(heap->nodes[parent]->global_goal <= heap->nodes[index]->global_goal)
		{
			break;
		}

		a_star_min_heap_swap(&heap->nodes[index], &heap->nodes[parent]);
		index = parent;
	}
}

#define ASTAR_MAP_CAPACITY 512

typedef struct
{
    bool occupied;
    V2 key;
    AStarNode* value;
} AStarMapEntry;

typedef struct
{
    AStarMapEntry entries[ASTAR_MAP_CAPACITY];
} AStarMap;

u32 a_star_hash(V2 p)
{
    return
        (u32)(p.x * 73856093u) ^
        (u32)(p.y * 19349663u);
}

void a_star_map_insert(
    AStarMap* map,
    V2 key,
    AStarNode* value)
{
    u32 index =
        a_star_hash(key) %
        ASTAR_MAP_CAPACITY;

    while (map->entries[index].occupied)
    {
        index =
            (index + 1) %
            ASTAR_MAP_CAPACITY;
    }

    map->entries[index].occupied = true;
    map->entries[index].key = key;
    map->entries[index].value = value;
}

AStarNode* a_star_map_get(
    AStarMap* map,
    V2 key)
{
    u32 index =
        a_star_hash(key) %
        ASTAR_MAP_CAPACITY;

    while (map->entries[index].occupied)
    {
        if (map->entries[index].key.x == key.x &&
            map->entries[index].key.y == key.y)
        {
            return map->entries[index].value;
        }

        index =
            (index + 1) %
            ASTAR_MAP_CAPACITY;
    }

    return NULL;
}

AStarNode* a_star_min_heap_pop(AStarMinHeap* heap)
{
	if(heap->len == 0) return NULL;

	AStarNode* min_node = heap->nodes[0];

	heap->nodes[0] = heap->nodes[heap->len - 1];
	heap->len --;

	i32 index = 0;

	while(true)
	{
		i32 left_index = 2 * index + 1;
		i32 right_index = 2 * index + 2;
		i32 smallest_index = index;


		if(left_index < heap->len && heap->nodes[left_index]->global_goal < heap->nodes[smallest_index]->global_goal)
			smallest_index = left_index;
		if(right_index < heap->len && heap->nodes[right_index]->global_goal < heap->nodes[smallest_index]->global_goal)
			smallest_index = right_index;
		if(smallest_index <= index) break;
		a_star_min_heap_swap(&heap->nodes[index], &heap->nodes[smallest_index]);
		index = smallest_index;
	}

	return min_node;
}

u32 a_star_distance(Vector2 startf, Vector2 endf)
{
	V2 start = Vector2V2(startf);
	V2 end = Vector2V2(endf);
	
	return (u32) (abs(start.x - end.x) + abs(start.y - end.y));
}

#define MAX_NODES 300
AStarNode* a_star_node_new(AStarNode* nodes, i32* len)
{
	if( (*len) >= MAX_NODES )
	{
		return NULL;
	}

	return &nodes[ (*len) ++ ];
}

AStarNode* a_star_find_node(AStarNode* open, i32 open_len, V2 key)
{
	for(i32 i = 0; i < open_len; i++)
	{
		AStarNode* node = &open[i];
		V2 pos = Vector2V2(node->pos);
		if(pos.x == key.x && pos.y == key.y)
		{
			return node;
		}
	}
	return NULL;
}

WalkPath path_get_any_path(Entity* self, Vector2 end_pos, GameState* state)
{
	Map* map = state->map;
	MapCamera* cam = state->map->camera;

	const V2 card_dirs[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	const Vector2 start_pos = self->pos;
	const Vector2 entity_midpoint = Vector2Midpoint(self->pos, self->data.dim);

	AStarNode node_pool[MAX_NODES];
	AStarMap node_map;
	i32 pool_len = 0;

	AStarNode* current = &node_pool[pool_len ++];
	*current = (AStarNode) {
		.pos = start_pos,
		.local_goal = 0.0f,
		.global_goal = a_star_distance(start_pos, end_pos),
		.parent = { -1.0, -1.0 },
		.closed = false,
	};

	a_star_map_insert(&node_map, Vector2V2(current->pos), current);

	AStarMinHeap open = {
		.max_len = ASTAR_MINHEAP_MAX_LEN,
		.len = 0,
	};
	a_star_min_heap_push(&open, current);
	//P_LOG("Start: (%d,%d), End: (%d,%d)\n", (i32) start_pos.x, (i32) start_pos.y, (i32) end_pos.x, (i32) end_pos.y);

	while((i32) current->pos.x != (i32) end_pos.x || (i32) current->pos.y != (i32) end_pos.y)
	{
		if(open.len == 0) 
		{
			P_LOG("Path Impossible1\n");
			return (WalkPath) { 0 };
		}

		current = a_star_min_heap_pop(&open);
#if 0
		P_LOG(
				"POP (%d,%d) g=%f h=%d f=%f\n",
				(int)current->pos.x,
				(int)current->pos.y,
				current->local_goal,
				a_star_distance(current->pos, end_pos),
				current->global_goal
			 );
#endif

		if(current == NULL)
		{
			P_ERROR("Path Impossible2\n");
			return (WalkPath) { 0 };
		}
		if(current->closed) continue;
		current->closed = true;

		for(i32 i = 0; i < sizeof(card_dirs) / sizeof(V2); i++)
		{
			V2 dir = card_dirs[i];
			V2 neighbor_pos = v2_add(Vector2V2(current->pos), dir);

			Tile tile = map_get_tile(map, neighbor_pos);
			if(tile.solid) continue;

			AStarNode* neighbor = a_star_map_get(&node_map, neighbor_pos);
			//if(neighbor != NULL) P_LOG("%f %f\n", neighbor->pos.x, neighbor->pos.y);
#if 0
			if(neighbor == NULL)
			{
				neighbor = a_star_node_new(node_pool, &pool_len);
				if(neighbor == NULL) 
				{
					//P_LOG("Out of space\n");
					return (WalkPath) { 0 };
				}
#if 0
				P_LOG("len: %d, x: %f, y: %f, end: (x,y) = (%d,%d)\n", pool_len, 
						node_pool[pool_len].pos.x, 
						node_pool[pool_len].pos.y,
						(i32) end_pos.x, (i32) end_pos.y);
#endif

				*neighbor = (AStarNode) {
					.global_goal = INF,
						.local_goal = INF,
						.pos = V2Vector2(neighbor_pos),
						.closed = false,
						.parent = (Vector2) {-1.0, -1.0},
				};
			}
#endif

			if (neighbor == NULL)
			{
				neighbor =
					a_star_node_new(
							node_pool,
							&pool_len);

				if (!neighbor)
					return (WalkPath){0};

				*neighbor = (AStarNode){
					.global_goal = INF,
						.local_goal = INF,
						.pos = V2Vector2(neighbor_pos),
						.closed = false,
						.parent = {-1,-1},
				};

				a_star_map_insert(
						&node_map,
						neighbor_pos,
						neighbor);
			}

			if(neighbor->closed) continue;

			const f32 lower_goal = current->local_goal + 1.01 * a_star_distance(current->pos, neighbor->pos);

			if(lower_goal < neighbor->local_goal)
			{
				//P_LOG("%f\n", neighbor->pos.x);
				neighbor->parent = current->pos;
				neighbor->local_goal = lower_goal;
				neighbor->global_goal = neighbor->local_goal + a_star_distance(neighbor->pos, end_pos);
				neighbor->closed = false;
#if 0
				P_LOG("%d |Node: (x,y) : (%f,%f) | start: (%d,%d) | end: (%d,%d)\n", pool_len, neighbor->pos.x, neighbor->pos.y,
						(i32) start_pos.x, (i32) start_pos.y, (i32) end_pos.x, (i32) end_pos.y);
#endif
				a_star_min_heap_push(&open, neighbor);
			}
		}	

	}

	//P_LOG("Finished Algorithm\n");

	WalkPath path = {0};

	while (current->parent.x != -1.0)
	{
		path.pos[path.count].x = current->pos.x;
		path.pos[path.count].y = current->pos.y;
		Vector2 p = current->parent;
		path.count++;
		current = a_star_map_get(&node_map, Vector2V2(p));
	}
	for(i32 i = 0; i < path.count / 2; i++)
	{
		Vector2 tmp = path.pos[i];
		path.pos[i] = path.pos[path.count - 1 - i];
		path.pos[path.count - 1 - i] = tmp;
	}
	path.current = 1;

	return path;



	return (WalkPath) { 0 };
}

#if 0

void a_star_init_nodes(AStarNode* nodes, Map* map, Vector2 start, V2 len)
{
	//V2 dim = map->camera->visible_tiles;
	V2 start_half = Vector2V2(Vector2Subtract(start, V2Vector2(v2_scale(len, 0.5)) ) );
	V2 dim = v2_add(len, start_half);

	for(i32 y = start_half.y, i = 0; y < dim.y; y++, i++)
	{
		for(i32 x = start_half.x, j = 0; x < dim.x; x++, j++)
		{
			nodes[vector2_to_vector_index(j, i, len.x)] = (AStarNode) {
				.pos = (Vector2) { x, y },
					.open = 0,
					.global_goal = INF,
					.local_goal = INF,
					.parent = (Vector2) { -1.0, -1.0 },
					.solid = map_get_tile(map, (V2) {x, y} ).solid,
					.visited = false,
			};
			AStarNode t = nodes[vector2_to_vector_index(j, i, len.x)];
		}
	}
}


//TODO Redo this to the Diablo 1 algorithm with nodes instead of grid
WalkPath path_get_any_path(Entity* self, Vector2 end_pos, GameState* state)
{

	Map* map = state->map;
	MapCamera* cam = state->map->camera;

	const V2 dim = { 28, 28 };
	const V2 card_dirs[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	const Vector2 entity_midpoint = Vector2Midpoint(self->pos, self->data.dim);

	const V2 array_offset = Vector2V2(Vector2Subtract(entity_midpoint, V2Vector2(v2_scale(dim, 0.5)) ) );

	const Vector2 array_start_pos = V2Vector2(v2_scale(dim, 0.5));

	const Vector2 array_end_pos = Vector2Subtract(end_pos, V2Vector2(array_offset));

	const V2 diff = v2_sub(Vector2V2(self->pos), Vector2V2(end_pos));
	if( abs(diff.x) >= dim.x / 2 || abs(diff.y) >= dim.y / 2 ) 
	{
		return (WalkPath) { 0 };
	}


	AStarNode nodes[dim.x * dim.y];
	a_star_init_nodes(nodes, map, entity_midpoint, dim);

	AStarNode* current = &nodes[vector2_to_vector_index(array_start_pos.x, array_start_pos.y, dim.x)];	
	current->local_goal = 0.0;
	current->global_goal = a_star_distance(self->pos, end_pos);
	AStarNode* end = &nodes[vector2_to_vector_index((i32) array_end_pos.x, (i32) array_end_pos.y, dim.x)];

	AStarMinHeap open = {
		.max_len = ASTAR_MINHEAP_MAX_LEN,
		.len = 0
	};

	a_star_min_heap_push(&open, current);
	current->open = 1;


	bool impossible = false;
	while(current != end)
	{

		if(open.len == 0) return (WalkPath) { 0 };

		if(open.len == 0) 
		{
			P_ERROR("Path Impossible\n");
			impossible = true;
			break;
		}
		current = a_star_min_heap_pop(&open);
		if(current == NULL) 
		{
			P_ERROR("Path Impossible\n");
			break;
		}

		for(i32 i = 0; i < sizeof(card_dirs) / sizeof(V2); i++)
		{
			V2 dir = card_dirs[i];	
			V2 neighbor_pos_array = v2_add(v2_sub(Vector2V2(current->pos), array_offset), dir);

			if (neighbor_pos_array.x < 0 || neighbor_pos_array.y < 0 || neighbor_pos_array.x >= dim.x || neighbor_pos_array.y >= dim.y)
			{
				continue;
			}

			AStarNode* neighbor = &nodes[vector2_to_vector_index(neighbor_pos_array.x, neighbor_pos_array.y, dim.x)];

			if(neighbor->pos.x < 0 || neighbor->pos.y < 0 || neighbor->pos.x >= map->dim.x || neighbor->pos.y >= map->dim.y)
			{
				continue;
			}
			if(neighbor->open == 2) continue;
			neighbor->open = 2;

			if(neighbor->solid == true)
			{
				neighbor->open = 2;
				continue;
			}

			assert(open.len < open.max_len);

			const f32 lower_goal = current->local_goal + 1.5 * a_star_distance(current->pos, neighbor->pos);

			if(lower_goal < neighbor->local_goal)
			{
				neighbor->parent = current->pos;
				neighbor->local_goal = lower_goal;
				neighbor->global_goal = neighbor->local_goal + a_star_distance(neighbor->pos, end->pos);
				neighbor->open = 1;
				a_star_min_heap_push(&open, neighbor);
			}

		}
	}


	WalkPath path = {0};
	if(impossible) return path;

	while (current->parent.x != -1.0)
	{
		path.pos[path.count].x = current->pos.x;
		path.pos[path.count].y = current->pos.y;
		Vector2 p = Vector2Subtract(current->parent, V2Vector2(array_offset));
		path.count++;
		current = &nodes[vector2_to_vector_index(p.x, p.y, dim.x)];
	}
	for(i32 i = 0; i < path.count / 2; i++)
	{
		Vector2 tmp = path.pos[i];
		path.pos[i] = path.pos[path.count - 1 - i];
		path.pos[path.count - 1 - i] = tmp;
	}
	path.current = 1;

	return path;
}

#endif
