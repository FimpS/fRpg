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

f32 entity_calculate_speed(Entity* self, GameState* state)
{
	f32 speed = self->speed.base;
	f32 speed_multiplier = 1.0;
	if(IsKeyDown(KEY_SPACE)) return self->speed.base * 1.5;
	/*	A solution but I dont like that enemies cannot do anything wise with speed.base, only affect the multiplier variable
		speed += self->gear.speed_flat; 
		speed += self->buffs.speed_flat; 
		speed_multiplier += self->buffs.speed_mult;
		speed_multiplier += self->gear.speed_mult;
		speed *= speed_multiplier

	*/
	return self->speed.base - 0.05;
}


//Speed multiplier for states which multiplied with self->speed.base?
//
//Better do maybe have a few fields that are multipliers which are addative
//Say you have speed_mult = 1.0; then you just add to it speed_mult += amazing_gear.speed;
//Then Just self->speed *= speed_mult; So never multiply something temporary
//Perhaps only affect base_speed with lvls and keep a constant if you want to revert
void estate_player_determine_movement_direction(Entity* self, GameState* state)
{
	f32 speed = entity_calculate_speed(self, state);
	Vector2 dir = {
		0.0,
		0.0
	};

	if(IsKeyDown(KEY_D)) { dir.x += 1.0; };
	if(IsKeyDown(KEY_A)) { dir.x -= 1.0; };
	if(IsKeyDown(KEY_S)) { dir.y += 1.0; };
	if(IsKeyDown(KEY_W)) { dir.y -= 1.0; };
	dir = Vector2Normalize(dir);

	if(dir.x != 0.0 || dir.y != 0.0) self->speed.frame = speed; else self->speed.frame = 0.0;

	self->facing_angle = atan2(dir.y, dir.x);
}

void estate_player_tick(Entity* self, GameState* state)
{
	Map* map = state->map;
	WalkPath* path = &self->path;
	Vector2 target = path->pos[path->current];

	f32 speed = 0.05;
	
	estate_player_determine_movement_direction(self, state);
	entity_move(self, state);



#if 0
	if(IsMouseButtonDown(0))
	{
		Vector2 end2 = map_get_mouse_cords(state->map);
		for(i32 i = 0; i < 800; i++)
			self->path = path_get_line_path(self, end2, state);
	}
#endif

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

f32 entity_check_tile_bounds_horizontal(Vector2 new_pos, const Vector2 old_pos, Entity* self, GameState* state)
{
	Map* map = state->map;
	const i32 first_tile = (i32) (new_pos.y);
	const i32 last_tile = (i32) (new_pos.y + self->dim.y - 0.001f);
	const i32 width = new_pos.x > old_pos.x ? (i32) (new_pos.x + self->dim.x) : new_pos.x;
	bool collided = false;
	for (i32 i = first_tile; i <= last_tile; i++)
	{
		Tile tile = map_get_tile(map, (V2) { width, i } );

		if (tile.solid)
		{
			collided = true; break;
		}
	}

	if (collided)
	{
		return new_pos.x > old_pos.x ? width - self->dim.x : width + 1.0;
	}
	return new_pos.x;

}

f32 entity_check_tile_bounds_vertical(Vector2 new_pos, const Vector2 old_pos, Entity* self, GameState* state)
{
	Map* map = state->map;
	const i32 first_tile = (i32) (new_pos.x);
	const i32 last_tile = (i32) (new_pos.x + self->dim.x - 0.001f);
	const i32 height = new_pos.y > old_pos.y ? (i32) (new_pos.y + self->dim.y) : new_pos.y;
	bool collided = false;
	for (i32 i = first_tile; i <= last_tile; i++)
	{
		Tile tile = map_get_tile(map, (V2) { i, height } );

		if (tile.solid)
		{
			collided = true; break;
		}
	}

	if (collided)
	{
		return new_pos.y > old_pos.y ? height - self->dim.y : height + 1.0;
	}
	return new_pos.y;

}

void entity_move(Entity* self, GameState* state)
{
	Map* map = state->map;

	const f32 angle = self->facing_angle;
	const Vector2 old_pos = self->pos;
	const const Vector2 velocity = Vector2Scale( (Vector2) { cos(angle), sin(angle) }, self->speed.frame);

	const Vector2 candidate_horizontal = Vector2Add(old_pos, (Vector2) { velocity.x, 0.0 } );
	const Vector2 candidate_vertical = Vector2Add(old_pos, (Vector2) { 0.0, velocity.y } );

	Vector2 new_pos = { 
		new_pos.x = entity_check_tile_bounds_horizontal(candidate_horizontal, old_pos, self, state),
		new_pos.y = entity_check_tile_bounds_vertical(candidate_vertical, old_pos, self, state)
	};

	self->pos = new_pos;

}

Entity* entity_player_init(GameState* state)
{
	Entity* player = entity_new(ENTITY_PLAYER, (Vector2) {14.0, 14.0} );
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


