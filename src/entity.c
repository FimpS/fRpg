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
	Map* map = state->map;
	Tile tile = map_get_tile(state->map, Vector2V2(pos));
	if(tile.solid) return false;
	if(pos.x < 0 || pos.y < 0 || pos.x >= map->dim.x || pos.y >= map->dim.y) return false;

	return true;
}

void entity_handle_standard_pathing(Entity* self, GameState* state, bool condition)
{
	Map* map = state->map;
	WalkPath* path = &self->path;
	Vector2 target = path->pos[path->current];

	Vector2 dir = Vector2Subtract(target, self->pos);


	if(condition)
	{

		Vector2 end = map_get_mouse_cords(state->map);
	
		if(entity_path_end_valid(end, state))
		{
			if(path_line_of_sight(self, end, state))
			{
				self->path = path_get_line_path(self, end);
			}
			else
			{
				self->path = path_get_any_path(self, end, state);
				//ValidateAndPrintPath(self->path, map->content, 32, 32);
			}
		}
		return;
	}

	if (Vector2Length(dir) > 0.1f && path->count > path->current)
	{
		dir = Vector2Normalize(dir);
		self->pos = Vector2Add(self->pos, Vector2Scale(dir, 0.18));
	}
	else
	{
		path->current ++;
	}
}

void estate_player_tick(Entity* self, GameState* state)
{
	Map* map = state->map;
	WalkPath* path = &self->path;
	Vector2 target = path->pos[path->current];

	Vector2 dir = Vector2Subtract(target, self->pos);
	f32 speed = 0.05;
	if(IsKeyDown(KEY_SPACE)) speed = 0.15;
	if(IsKeyDown(KEY_D)) { self->theta = 0; self->speed = speed; };
	if(IsKeyDown(KEY_A)) { self->theta = PI; self->speed = speed; };
	if(IsKeyDown(KEY_S)) { self->theta = PI / 2; self->speed = speed; };
	if(IsKeyDown(KEY_W)) { self->theta = 3 * PI / 2; self->speed = speed; };

	entity_move(self, state);

	self->speed = 0.0;
	//entity_handle_standard_pathing(self, state, IsMouseButtonPressed(1));
#if 0
	if(IsMouseButtonDown(0))
	{
		Vector2 end2 = map_get_mouse_cords(state->map);
		for(i32 i = 0; i < 800; i++)
			self->path = path_get_line_path(self, end2, state);
	}
#endif

	if(IsKeyDown(KEY_W))
	{
		//self->pos.x += 1.0;
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

#define NEW_LINE() printf("\n");

void entity_move(Entity* self, GameState* state)
{
	Map* map = state->map;

	const f32 angle = self->theta;
	const Vector2 old_pos = self->pos;
	Vector2 new_pos = Vector2Add(old_pos, Vector2Scale( (Vector2) { cos(angle), sin(angle) }, self->speed));


		NEW_LINE();
	if( new_pos.x > old_pos.x )
	{

		const V2 tile_checks = { (i32) self->dim.x + 1, (i32) self->dim.y + 1};
		bool collided_vertically = false;
		for(i32 i = 0; i < tile_checks.y + 1; i++)
		{
			const f32 ti = tile_checks.y - (i + 1) >= 1.0 ? 1.0 : self->dim.y - (i32) self->dim.y;
				//printf("%d: %f32\n", i, ti);
			const Tile tile = map_get_tile(map, 
					(V2) { (i32) (new_pos.x + self->dim.x), 
					(i32) (old_pos.y + ti + i - 1) } );
			P_LOG("Entity:\tpos = (%f, %f) | dim = (%f, %f)\n", self->pos.x, self->pos.y, self->dim.x, self->dim.y);
			P_LOG("Tile Data:\tpos = (%d, %d) | dim = (%d, %d)\n", (i32) (new_pos.x + self->dim.x), (i32) (old_pos.y + ti), 1, 1);
			P_LOG("TI:\t%f\n", ti);
			if(tile.solid)
			{
				collided_vertically = true;
			}
		}
		NEW_LINE();

		if(collided_vertically) 
		{
			new_pos.x = old_pos.x ; //maybe this could be tile cordinates
		}
	}

#if 0
	else
	{
		const V2 tile_checks = { (i32) self->dim.x, (i32) self->dim.y };
		bool collided_vertically = false;
		for(i32 i = 0; i < tile_checks.y; i++)
		{
			const Tile tile = map_get_tile(map, (V2) {new_pos.x, old_pos.y - i} );
			if(tile.solid)
			{
				collided_vertically = true;
			}
		}
		if(collided_vertically) 
		{
			new_pos.x = (i32) new_pos.x + 1; 
		}
	}
#endif 

	if( new_pos.y > old_pos.y )
	{

	}

	self->pos = new_pos;

}

Entity* entity_player_init(GameState* state)
{
	Entity* player = entity_new(ENTITY_PLAYER, (Vector2) {14.0, 14.0} );
	player->speed = 0.15f;

	player->path = (WalkPath) {0};
	return player;
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
	const u32 frame_index = ( (animation->timer ++ ) / animation->stop_timer ) % animation->amount_frames;
	return animation->frames[ frame_index ];
}

void entity_render(Entity* self, GameState* state)
{
	MapCamera* cam = state->map->camera;
	const u32 rgb_values = 255 * self->light.self;
	const u32 rgb = 255;
	Color color = { rgb_values, rgb_values, rgb_values, rgb};

	Rectangle src_frame = entity_get_render_frame(self, state);
	DrawRectangle((self->pos.x - cam->offset.x) * cam->tile_len,
					(self->pos.y - cam->offset.y) * cam->tile_len,
					self->dim.x * cam->tile_len, 
					self->dim.y * cam->tile_len, (Color) {255, 255, 255, 255});
		
		
#if 0
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
#endif
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


