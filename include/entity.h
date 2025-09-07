#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "../lib/types.h"
#include "../include/state.h"

typedef enum EntityType
{
	ENTITY_PLACEHOLDER,
} EntityType;

typedef struct Entity
{
	Vector2 pos;
	EntityType type;
} Entity;

Entity* entity(EntityType type, Vector2 pos);

void entity_destroy(Entity* e);

void entities_tick(GameState* state);

#endif
