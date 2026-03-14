#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "../lib/types.h"
#include "../include/gfx.h"
#include "../include/state.h"

typedef struct GameState GameState;
typedef struct Entity Entity;

typedef enum EntityType
{
	ENTITY_PLACEHOLDER,
	ENTITY_PLACEHOLDER2,
	ENTITY_LAST,
} EntityType;

typedef enum EntityStateType
{
	ESTYPE_PLACEHOLDER,
	ESTYPE_PLACEHOLDER2,
	ESTYPE_CLEAR,

} EntityStateType;

typedef struct EntitySprite
{
	Rectangle rec_bmap;
} EntitySprite;

typedef struct EntityState
{
	EntityStateType type;
	void (*tick)(Entity* self);
	EntitySprite sprite;
} EntityState;

typedef struct Entity
{
	Vector2 pos;
	Vector2 dim;
	Light light;
	EntityType type;
	EntityState state;
	u32 id;
} Entity;

Entity* entity_new(EntityType type, Vector2 pos);
Entity* entity_new_editor(EntityType type, Vector2 pos);

void entity_destroy(Entity* e);

void entities_tick(DynList* entities, GameState* state);
void entities_render(DynList* entities, GameState* state);

#endif
