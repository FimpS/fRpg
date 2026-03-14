#include <stdlib.h>
#include <stdio.h>

#include "../include/entity.h"
#include "../include/global.h"

/* PRIVATE */

void estate_placeholder_tick(Entity* self)
{
	//self->pos.x += 0.05;
}

EntityState state_map[] =
{
	{ESTYPE_PLACEHOLDER, estate_placeholder_tick, {{0, 0, 16, 16}, }, },
	{ESTYPE_PLACEHOLDER2, estate_placeholder_tick, {{0, 0, 16, 16}, }, },
};

/* PRIVATE */

/* PUBLIC */

bool AAB(Entity* e, Vector2 p)
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

void entities_tick(DynList* entities, GameState* state)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		Tile t = map_get_tile(state->map, Vector2V2(Vector2Midpoint(e->pos, e->dim)));
		e->light.self = ( t.light * 2.0 ) + 0.25 ;
		e->light.self = e->light.self >= 1.0 ? 1.0 : e->light.self;
		e->state.tick(e);
	}
}

void entity_render(Entity* self, GameState* state)
{
	MapCamera* cam = state->map->camera;
	const u32 rgb_values = 255 * self->light.self;
	const u32 rgb = 255;
	Color color = { rgb_values, rgb_values, rgb_values, rgb};
	DrawTexturePro(state->gfx->texs[TEXTURE_TILEMAP], 
			self->state.sprite.rec_bmap, 
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
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->state = state_map[newe->type];
	newe->pos = pos;
	newe->light = (Light) {
		.pos = newe->pos,
		.distance = 1.0,
		.tint = WHITE,
		.value = 1.0,
	};
	newe->dim = (Vector2) {2.0, 2.0};
	newe->id = 1;
}

Entity* entity_new(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->state = state_map[newe->type];
	newe->pos = pos;
	if(type == ESTYPE_PLACEHOLDER2)
	{
		newe->light = (Light) {
			.pos = newe->pos,
				.distance = 12.0,
				.self = 0.1,
				.tint = WHITE,
				.value = 0.8,
		};
	}
	else
	{
		newe->light = (Light) { 0 };
	}

	newe->dim = (Vector2) {2.0, 2.0};
	newe->id = 1;
}

void entity_destroy(Entity* e)
{
	free(e);
}

/* PUBLIC */
