#include <stdlib.h>
#include <stdio.h>

#include "../include/entity.h"
#include "../include/entity_info.h"
#include "../include/global.h"

/* PRIVATE */

void estate_placeholder_tick(Entity* self, GameState* state)
{
	if(IsKeyDown(KEY_I))
	{
	//self->light.distance = (i32) ( self->light.distance + 1.0 ) % 25;
	self->light.value = (f32) ( self->light.value + 0.1 );
	if(self->light.value >= 1.0) self->light.value = 0.0;

	// from 0.0 - 10.0 increase, 10.0 - 50.0 decrease and looks good
	//printf("Light: %f\n", self->light.distance);
	//self->pos.x += 0.25;
	//self->pos.y += 0.15;
	}
}


bool entity_path_end_valid(Vector2 pos, GameState* state)
{
	Tile tile = map_get_tile(state->map, Vector2V2(pos));
	if(tile.solid) return false;

	return true;
}

bool map_has_line_of_sight(Entity* self, Vector2 end, GameState* state)
{
	Map* map = state->map;
	V2 dim = map->dim;
	Vector2 start = self->pos;

	int x0 = (int)start.x;
	int y0 = (int)start.y;
	int x1 = (int)end.x;
	int y1 = (int)end.y;

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	int err = dx - dy;

	while (true)
	{
		// bounds check (optional but safe)
		if (x0 < 0 || y0 < 0 || x0 >= dim.x || y0 >= dim.y)
			return false;

		// check tile
		if (map->content[vector2_to_vector_index(x0, y0, dim.x)].solid)
			return false;

		// reached end
		if (x0 == x1 && y0 == y1)
			break;

		int e2 = 2 * err;

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}

	return true;
}

void estate_player_tick(Entity* self, GameState* state)
{
	WalkPath* path = &self->path;
	Vector2 target = path->pos[path->current];

	Vector2 dir = Vector2Subtract(target, self->pos);

	if(IsMouseButtonPressed(1))
	{
		Vector2 end = map_get_mouse_cords(state->map);
		//Like 64 per is ok
		if(entity_path_end_valid(end, state))
		{
			if(map_has_line_of_sight(self, end, state))
			{
				WalkPath path = {0};
				path.pos[0] = Vector2Midpoint(end, Vector2Scale(self->dim, -1.0));
				path.count = 1;
				path.current = 0;
				self->path = path;
			}
			else
			{
				//end = Vector2Scale(GetMousePosition(), (f32) 1 / state->map->camera->tile_len);
				P_LOG("Outside: %f %f\n", end.x, end.y);
				self->path = entity_find_path(self, end, state);
			}
		}
		return;
	}

	if (Vector2Length(dir) > 0.1f && path->count > path->current)
	{
		dir = Vector2Normalize(dir);
		self->pos = Vector2Add(self->pos, Vector2Scale(dir, 0.08));
	}
	else
	{
		path->current ++;
	}
	if(IsKeyDown(KEY_W))
	{
		self->pos.x += 1.0;
	}
}

const EntityState entity_state_table[] = {
	(EntityState) {
		.type = ESTYPE_PLACEHOLDER, 
		.tick = estate_placeholder_tick, 
		.animation = {
			.stop_timer = 8,
			.amount_frames = 2,
			.frames = { {0, 0, 16, 16}, {0, 16, 16, 16}, },
		},
	},
	(EntityState) {
		.type = ESTYPE_PLACEHOLDER2, 
		.tick = estate_placeholder_tick, 
		.animation = {
			.stop_timer = 8,
			.amount_frames = 1,
			.frames = { {0, 0, 16, 16}, {0, 16, 16, 16}, },
		},
	},
	(EntityState) {
		.type = ESTYPE_PLAYER_TICK,
		.tick = estate_player_tick,
		.animation = {
			.stop_timer = 1,
			.amount_frames = 1,
			.frames = { {0, 0, 16, 16}, {0, 16, 16, 16}, },
		},
	},
};

Entity* entity_player_init(GameState* state)
{
	Entity* player = entity_new(ENTITY_PLAYER, (Vector2) {14.0, 14.0} );

	return player;
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

	P_LOG("from: %d %d to %d %d\n", start_half.x, start_half.y, dim.x, dim.y);
	P_LOG("start: %d %d\n", (i32) start.x, (i32) start.y);
	for(i32 y = start_half.y, i = 0; y < dim.y; y++, i++)
	{
		for(i32 x = start_half.x, j = 0; x < dim.x; x++, j++)
		{
			nodes[vector2_to_vector_index(j, i, len.x)] = (AStarNode) {
				//offset this with camera if we doing the visible tiles version...
				.pos = (Vector2) { x, y },
					.global_goal = INF,
					.local_goal = INF,
					.parent = (Vector2) { -1.0, -1.0 },
					.walkable = map_get_tile(map, (V2) {x, y} ).solid,
					.visited = false,
			};
		}
	}
}

void a_star_sort_not_tested(AStarNode** nodes, i32 left, i32 right)
{
	if(left >= right) return;

	AStarNode* pivot = nodes[(left + right) / 2];
	f32 pivot_value = pivot->global_goal;

	i32 i = left;
	i32 j = right;

	while(i <= j)
	{
		while(nodes[i]->global_goal < pivot_value) i++;
		while(nodes[j]->global_goal > pivot_value) j--;

		if(i <= j)
		{
			AStarNode* tmp = nodes[i];
			nodes[i] = nodes[j];
			nodes[j] = tmp;
			i++;
			j--;
		}
	}

	if(left < j)  a_star_sort_not_tested(nodes, left, j);
	if(i < right) a_star_sort_not_tested(nodes, i, right);
}



#define DIMTMP 32
WalkPath entity_find_path(Entity* self, Vector2 end_pos, GameState* state)
{

	Map* map = state->map;
	MapCamera* cam = state->map->camera;
	V2 dim = { DIMTMP, DIMTMP };//cam->visible_tiles;
	//Vector2 start_pos = Vector2Midpoint(self->pos, self->dim);
	//Vector2 start_pos = start_pos;//Vector2V2(Vector2Subtract(start_pos, V2Vector2(v2_scale(dim, 0.5)) ) );
	Vector2 start_pos = { dim.x / 2.0, dim.y / 2.0 };
	//TODO fix so it the DMPTMP is relative to entity
	const V2 card_dirs[4] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	AStarNode nodes[dim.x * dim.y];
	a_star_init_nodes(nodes, map, start_pos, dim);

#if 0
	for(i32 i = 0; i < dim.y; i++)
	{
		for(i32 j = 0; j < dim.x; j++)
			printf("%d", nodes[vector2_to_vector_index(j, i, dim.x)].walkable);
		printf("\n");
	}
#endif

	AStarNode* current = &nodes[vector2_to_vector_index(start_pos.x, start_pos.y, dim.x)];	
	current->local_goal = 0.0;
	current->global_goal = a_star_distance(start_pos, end_pos);
	AStarNode* end = &nodes[vector2_to_vector_index(end_pos.x, end_pos.y, dim.x)];
	printf("StartNode - x: %f y: %f\n", current->pos.x, current->pos.y);
	printf("EndNode   - x: %f y: %f\n", end->pos.x, end->pos.y);

	AStarNode* not_tested_nodes[dim.x * dim.y];
	u32 not_tested_nodes_len = 0;
	not_tested_nodes[not_tested_nodes_len ++] = current;

	u32 panic = 0;
	while(not_tested_nodes_len != 0 && current != end)
	{
		a_star_sort_not_tested(not_tested_nodes, 0, not_tested_nodes_len - 1);

		//for(i32 i = 0; i < not_tested_nodes_len; i++) printf("%f, ", not_tested_nodes[i]->global_goal);
		//printf("\n");

		while(not_tested_nodes_len > 0 && not_tested_nodes[0]->visited)
		{
			for(i32 i = 0; i < not_tested_nodes_len - 1; i++)
				not_tested_nodes[i] = not_tested_nodes[i + 1];

			not_tested_nodes_len--;
		}

		if(not_tested_nodes_len == 0) 
		{
			P_ERROR("Path Impossible\n");
			break;
		}

		current = not_tested_nodes[0];
		current->visited = true;
		for(i32 i = 0; i < sizeof(card_dirs) / sizeof(V2); i++)
		{
			V2 neighbor_pos = card_dirs[i];	
			int nx = current->pos.x + neighbor_pos.x;
			int ny = current->pos.y + neighbor_pos.y;

			if (nx < 0 || ny < 0 || nx >= dim.x || ny >= dim.y)
			{
				continue;
			}
			AStarNode* neighbor = &nodes[vector2_to_vector_index(current->pos.x + neighbor_pos.x,
					current->pos.y + neighbor_pos.y,
					dim.x)];
			if(!neighbor->visited && neighbor->walkable == false)
			{
				not_tested_nodes[not_tested_nodes_len ++] = neighbor;
			}

			f32 lower_goal = current->local_goal + a_star_distance(current->pos, neighbor->pos);

			if(lower_goal < neighbor->local_goal)
			{
				neighbor->parent = current->pos;
				neighbor->local_goal = lower_goal;
				neighbor->global_goal = neighbor->local_goal + a_star_distance(neighbor->pos, end->pos);
			}

		}
	}
	WalkPath path = {0};
	//current = &nodes[vector2_to_vector_index(start_pos.x, start_pos.y, dim.x)];
#if 1
	while (current->parent.x != -1.0)
	{
		path.pos[path.count].x = current->pos.x;
		path.pos[path.count].y = current->pos.y;
		path.count++;

		current = &nodes[vector2_to_vector_index(current->parent.x, current->parent.y, dim.x)];
	}
	for(i32 i = 0; i < path.count / 2; i++)
	{
		Vector2 tmp = path.pos[i];
		path.pos[i] = path.pos[path.count - 1 - i];
		path.pos[path.count - 1 - i] = tmp;
	}
	path.current = 1;
#endif

	return path;
}

/* PRIVATE */

/* PUBLIC */

bool entity_AAB(Entity* e, Vector2 p)
{
	return p.x >= e->pos.x 
		&& p.x <= e->pos.x + e->dim.x 
		&& p.y >= e->pos.y
		&& p.y <= e->pos.y + e->dim.y;
}

bool AABB(Entity* s, Entity* t)
{
	return true;
}

Entity* entity_get_vector(GameState* state, Vector2 pos)
{
	DynList* entities = state->map->entities;
	const u32 len = dynList_len(entities);
	for(i32 i = 0; i < len; i++)
	{
		Entity* e = dynList_get(entities, i);
		//if(Vector2V2(e->pos) == Vector2Equals
	}
}

void entities_tick(DynList* entities, GameState* state)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		Tile t = map_get_tile(state->map, Vector2V2(Vector2Midpoint(e->pos, e->dim)));
		if(e->light.light_source)
		{
			e->light.self = ( t.light * 2.0 ) + 0.25 ;
			e->light.self = e->light.self >= 1.0 ? 1.0 : e->light.self;
		}
		if( e->state.tick != NULL ) e->state.tick(e, state);
	}
}

Rectangle entity_get_render_frame(Entity* self, GameState* state)
{
	EntityAnimation* animation = &self->state.animation;
	const u32 expression = ( (animation->timer ++ ) / animation->stop_timer ) % animation->amount_frames;
	return animation->frames[ expression ];
}

void entity_render(Entity* self, GameState* state)
{
	MapCamera* cam = state->map->camera;
	const u32 rgb_values = 255 * self->light.self;
	const u32 rgb = 255;
	Color color = { rgb_values, rgb_values, rgb_values, rgb};

	Rectangle src_frame = entity_get_render_frame(self, state);
	DrawTexturePro(state->gfx->texs[TEXTURE_TILEMAP], 
			src_frame, 
			(Rectangle) {
			(self->pos.x - cam->offset.x) * cam->tile_len, 
			(self->pos.y - cam->offset.y) * cam->tile_len, 
			self->dim.x * cam->tile_len, 
			self->dim.y * cam->tile_len
			}, 
			(Vector2) {0}, 
			0.0, 
			color);
}

void entities_render(DynList* entities, GameState* state)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		entity_render(e, state);
	}
}

Entity* entity_new_editor(EntityType type, Vector2 pos)
{
#if 0
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->state = entity_state_table[newe->type];
	newe->pos = pos;
	newe->light = (Light) {
		.pos = newe->pos,
			.
				.distance = 1.0,
			.tint = WHITE,
			.value = 1.0,
	};
	newe->dim = (Vector2) {2.0, 2.0};
	newe->id = 1;
#endif
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->state = entity_state_table[newe->type];
	newe->pos = pos;
	if(type == ESTYPE_PLACEHOLDER)
	{
		newe->light = (Light) {
			.pos = newe->pos,
				.distance = 9.0, //frand( (Vector2) {3.0, 4.0} ),
				.self = 0.1,
				.tint = WHITE,
				.value = 0.8, //frand( (Vector2) {0.5, 0.45} ),
				.flicker = (Vector2) {0.8, 0.1},
				.light_source = false,
		};
	}
	else
	{
		newe->light = (Light) { .self = 0.0, .light_source = true, };
	}

	if(newe->light.flicker.x + newe->light.flicker.y > 1.0) newe->light.flicker.y -= newe->light.flicker.x - 1.0;
	if(type == ESTYPE_PLACEHOLDER) newe->dim = (Vector2) {1.0, 1.0};
	else newe->dim = (Vector2) {0.5, 0.5};
	newe->id = 1;
}

Entity* entity_new(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));
	*newe = entity_type_table[type];
	newe->type = type;
	newe->pos = pos;
	newe->state = entity_state_table[newe->state.type];

	return newe;
}

void entity_destroy(Entity* e)
{
	free(e);
}

/* PUBLIC */
