#include <stdlib.h>
#include <stdio.h>

#include "../include/entity.h"

/* PRIVATE */

void estate_placeholder_tick(Entity* self)
{
	//self->pos.x += 0.05;
}

EntityState state_map[] =
{
	{ESTYPE_PLACEHOLDER, estate_placeholder_tick, {{0, 0, 16, 16}, }, },
	{ESTYPE_PLACEHOLDER, estate_placeholder_tick, {{0, 0, 16, 16}, }, },
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

void entities_tick(DynList* entities)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		e->state.tick(e);
	}
}

void entity_render(Entity* self)
{
	//DrawTexturePro(state->tex, rec_tex, rec_dst, (Vector2) {0}, 0.0, WHITE);
	//DrawTexturePro(state->gfx->texs[TEXTURE_TILEMAP], self->state.sprite.rec_bmap, (Rectangle) {(self->pos.x - camera->offset.x) * editor->map->camera->tile_len, (self->pos.y - camera->offset.y) * editor->map->camera->tile_len, self->dim.x * editor->map->camera->tile_len, self->dim.y * editor->map->camera->tile_len}, (Vector2) {0}, 0.0, RED);
}

void entities_render(DynList* entities)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
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
	newe->light = (Light) {
		.pos = newe->pos,
		.distance = 12.0,
		.tint = WHITE,
		.value = 0.8,
	};
	newe->dim = (Vector2) {2.0, 2.0};
	newe->id = 1;
}

void entity_destroy(Entity* e)
{
	free(e);
}

/* PUBLIC */
