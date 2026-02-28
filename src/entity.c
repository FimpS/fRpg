#include <stdlib.h>
#include <stdio.h>

#include "../include/entity.h"

/* PRIVATE */

void estate_placeholder_tick(Entity* self)
{
	self->pos.x += 0.05;
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

void entities_tick(GameState* state)
{
	for(i32 i = 0; i < dynList_len(state->entities); i++)
	{
		Entity* e = dynList_get(state->entities, i);
		e->state.tick(e);
		//printf("%d\n", e->type);
	}
}

void entity_render(GameState* state)
{
	//DrawTexturePro(state->tex, rec_tex, rec_dst, (Vector2) {0}, 0.0, WHITE);
}

void entities_render(GameState* state)
{
	for(i32 i = 0; i < dynList_len(state->entities); i++)
	{
		Entity* e = dynList_get(state->entities, i);
		//render_entity(e);
	}
}

Entity* entity_new_editor(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->state = state_map[newe->type];
	newe->pos = pos;
	newe->dim = (Vector2) {2.0, 2.0};
	newe->id = 1;
}

Entity* entity_new(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->state = state_map[newe->type];
	newe->pos = pos;
	newe->dim = (Vector2) {2.0, 2.0};
	newe->id = 1;
}

void entity_destroy(Entity* e)
{
	free(e);
}

/* PUBLIC */
