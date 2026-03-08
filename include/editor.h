#ifndef EDITOR_H
#define EDITOR_H

#include <string.h>

#include "../include/map.h"
#include "../include/gfx.h"
#include "../lib/dynList.h"

#define EDITORSTATEMAXSIZE 2
#define HOTBAR_LEN 9
#define MAX_FILE_LEN 64


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


typedef struct EditorMode
{
	V2 start_cord;
	bool mode;
	V2 end_cord;
} EditorMode;

typedef struct Editor
{
	Gfx* gfx;
	DynList* temp_texts;

	EditorState state;

	u8 filename[MAX_FILE_LEN];
	Map* map;
	Map* brush_map;

	Vector2 pos;
	Tile selected_tile;

	Entity* showing_entity;

	BrushState bstate;
	u32 brush_dim;
	Tile brush;
	EntityType selected_entity;

	i32 selected_hotbar;
	Tile hotbar[HOTBAR_LEN];
	EntityType entity_hotbar[HOTBAR_LEN];
	bool lock_hotbar;

	EditorMode spline;

	EditorMode quad;
	Tile* quad_buffer;
	V2 quad_dim;
	bool quad_copied;
} Editor;

bool AAB(Entity* e, Vector2 p);
Editor* editor_new();
void editor_tick(Editor* editor);
void editor_render(Editor* editor);
void editor_parse_file_input(u8* file_buffer);
// TODO
// Argc, and Argv
// selected Tile at bottom left
// also add Mobjects for example if (button) is pressed you can now copy Entities
#endif
