
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
		.pos[0] = Vector2Midpoint(end, Vector2Scale(self->dim, -1.0)),
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

u32 a_star_distance(Vector2 start, Vector2 end)
{
	return (u32) (abs(start.x - end.x) + abs(start.y - end.y));
}

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


WalkPath path_get_any_path(Entity* self, Vector2 end_pos, GameState* state)
{

	Map* map = state->map;
	MapCamera* cam = state->map->camera;

	const V2 dim = { 28, 28 };
	const V2 card_dirs[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	const Vector2 entity_midpoint = Vector2Midpoint(self->pos, self->dim);

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
