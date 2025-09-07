#include <stdlib.h>
#include <stdio.h>

#include "../include/entity.h"

/* PRIVATE */





/* PRIVATE */

/* PUBLIC */

void entities_render(GameState* state)
{
	for(i32 i = 0; i < dynList_len(state->entities); i++)
	{
		Entity* e = dynList_get(state->entities, i);
		//render_entity(e);
	}
}

void entities_tick(GameState* state)
{
	for(i32 i = 0; i < dynList_len(state->entities); i++)
	{
		Entity* e = dynList_get(state->entities, i);
		//e->tick();
		//printf("%d\n", e->type);

	}
}

Entity* entity(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));
	newe->type = type;
	newe->pos = pos;
}

void entity_destroy(Entity* e)
{
	free(e);
}

/* PUBLIC */
