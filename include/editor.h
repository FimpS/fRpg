#ifndef EDITOR_H
#define EDITOR_H

#include "../include/map.h"
#include "../include/gfx.h"
#include "../lib/dynList.h"

#define EDITORSTATEMAXSIZE 2

typedef enum TileType //Not needed probably, But some kind of list of what A tile should look like idk last part to think about...
					  //
{
	TILETYPE_TEST1 = 1,
	TILETYPE_TEST2,
	TILETYPE_TEST3,
	TILETYPE_TEST4,
} TileType;


typedef enum EditorState
{
	EDITORSTATE_MAINCANVAS,
	EDITORSTATE_BRUSHCANVAS,
} EditorState;

typedef enum BrushState
{
	BRUSHSTATE_TILE,
	BRUSHSTATE_ENTITY,
} BrushState;

typedef struct Editor
{
	Gfx* gfx;
	DynList* temp_texts;

	EditorState state;

	Map* map;
	Map* brush_map;

	Vector2 pos;
	Tile selected_tile;

	Entity* showing_entity;

	BrushState bstate;
	u32 brush_dim;
	Tile brush;
	EntityType selected_entity;
} Editor;

bool AAB(Entity* e, Vector2 p);
Editor* editor_new();
void editor_tick(Editor* editor);
void editor_render(Editor* editor);
// TODO
// Argc, and Argv
// selected Tile at bottom left
// also add Mobjects for example if (button) is pressed you can now copy Entities
#endif
