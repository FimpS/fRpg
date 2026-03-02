#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/editor.h"
#include "../include/global.h"
#include "raylib.h"
#include "raymath.h"

#define ENTITY_TYPE_LIST_LEN ENTITY_LAST

#define P_ERROR(...) printf("ERROR: %s", __VA_ARGS__);


#define DEBUG_MODE 1
#define P_EDITORINFO(s, ...) if ( DEBUG_MODE )  { printf("EDITOR INFO: "); printf(s, __VA_ARGS__); }

void editor_load_level(Map* map, const char* filepath)
{
	FILE* fp = NULL;

	fp = fopen(filepath, "rb");
	if(!fp)
	{
		P_ERROR("ERROR: Failed to open file\n");
	}

	if(fread(map->content, sizeof(Tile), map->dim.x * map->dim.y, fp) != map->dim.x * map->dim.y)
	{
		P_ERROR("ERROR: File failed to read appropriate bytes\n");
	}
	u32 entity_list_len = 0;
	fread(&entity_list_len, sizeof(unsigned), 1, fp);

	for (u32 i = 0; i < entity_list_len; i++)
	{
		Entity* allocated_entity = malloc(sizeof(Entity));
		fread(allocated_entity, sizeof(Entity), 1, fp);
		dynList_push(map->entities, allocated_entity);
	}
	fclose(fp);
}

void editor_push_saved(Editor* editor)
{
	dynList_push(editor->temp_texts, temporary_text_new(
				"Saved map to file...",
				(Vector2) {GetScreenWidth() - GetScreenHeight() / 3.5, GetScreenHeight() / 24},
				20,
				240,
				RED
				));
}

void editor_save_level(Editor* editor, const char* filepath)
{
	Map* map = editor->map;

	FILE* fp = NULL;

	fp = fopen(filepath, "wb");
	if(!fp)
	{
		P_ERROR("ERROR: Failed to open file\n");
	}

	if(fwrite(map->content, sizeof(Tile), map->dim.x * map->dim.y, fp) != map->dim.x * map->dim.y)
	{
		P_ERROR("ERROR: File failed to write appropriate bytes\n");
	}
	const u32 len = dynList_len(map->entities);
	fwrite(&len, sizeof(u32), 1, fp);

	for (u32 i = 0; i < len; i++)
	{
		Entity* e = dynList_get(map->entities, i);
		fwrite(e, sizeof(Entity), 1, fp);
	}	
	editor_push_saved(editor);
	fclose(fp);
}

static Rectangle tilemap_textures[] =
{
	[0] = {0, 0, 16, 16},
	[TILETYPE_TEST1] = {0, 16, 16, 16},
	[TILETYPE_TEST2] = {16, 0, 16, 16},
	[TILETYPE_TEST3] = {32, 0, 16, 16},
	[TILETYPE_TEST4] = {48, 0, 16, 16},
};

static Tile tile_sheet[] =
{
	{0, 0, 0},
	{1, 0, 0},
	{2, 0, 0},
	{3, 0, 0},
	{4, 0, 0},
};

void editor_reset_map(Editor* editor)
{
	Map* map = editor->brush_map;
	for(int x = 0; x < map->dim.x; x++)
	{
		for(int y = 0; y < map->dim.y; y++)
		{
			map->content[y * map->dim.x + x] = (Tile) {
				.type = 0,
					.animated = false,
					.solid = false,	
			};
			//printf("%d ", editor->map->content[y * editor->map->dim.x + x].type);
		}
		//printf("\n");
	}
	for(int i = 0; i < sizeof(tilemap_textures) / sizeof(Rectangle); i++)
	{
		map->content[i] = tile_sheet[i];
	}

	for(i32 i = 0; i < ENTITY_TYPE_LIST_LEN; i++)
	{
		dynList_push(map->entities, entity_new_editor(i, (Vector2) {4.0 + i * 4.0, 20.0} ));	
	}
}

Editor* editor_new()
{
	Editor* editor = malloc(sizeof(Editor));

	editor->temp_texts = dynList_new();

	editor->state = EDITORSTATE_MAINCANVAS;
	editor->pos = vector2(4.0, 4.0);
	editor->map = map_new();
	editor->brush_map = map_new();
	editor->map->dim = v2_new(64, 64);
	editor->brush_map->dim = v2_new(64, 64);
	editor->brush = (Tile) { 3, 0, ENTITYCLASS_NONE };
	editor->gfx = gfx_new();
	editor->bstate = BRUSHSTATE_TILE;
	editor->brush_dim = 1;
	editor->selected_entity = ENTITY_PLACEHOLDER;
	editor->showing_entity = entity_new_editor(editor->selected_entity, (Vector2) {0.0, 0.0} );
	for(int x = 0; x < editor->map->dim.x; x++)
	{
		for(int y = 0; y < editor->map->dim.y; y++)
		{
			editor->map->content[y * editor->map->dim.x + x] = (Tile) {
				.type = 0,
					.animated = false,
			};
			//printf("%d ", editor->map->content[y * editor->map->dim.x + x].type);
		}
		//printf("\n");
	}
	editor_load_level(editor->map, "../maps/test.tmp");
	editor_reset_map(editor);
	//editor_save_level(editor->brush_map, "../maps/BrushCanvas.tmp");
	//editor_load_level(editor->brush_map, "../maps/BrushCanvas.tmp");
	//printf("%d %d\n", GetScreenHeight(), GetScreenWidth());
	return editor;
}

void editor_destroy(Editor* editor)
{
	map_destroy(editor->map);
	free(editor);
}

void editor_select_tile(Editor* editor)
{
	//if(IsMouseButtonReleased(0))
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonDown(0) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_C))
	{
		V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		Tile selected = map_get_tile(editor->map, mp);
		for(i32 i = 0; i < editor->brush_dim; i++)
		{
			for(i32 j = 0; j < editor->brush_dim; j++)
			{
				map_set_tile(editor->map, v2_add(mp, (V2) {i, j} ), editor->brush);
			}
			//map_set_tile(editor->map, mp, editor->brush);
		}
		//printf("INFO: Solid: %d Type: %d Anim: %d\n", editor->brush.solid, editor->brush.type, editor->brush.animated);
		//printf("%d\n", selected.type);
	}
}

void editor_push_selected_entity(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonPressed(0) && !IsKeyDown(KEY_C) && !IsKeyDown(KEY_D))
	{
		V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		Tile selected = map_get_tile(editor->map, mp);
		dynList_push(editor->map->entities, entity_new_editor(editor->selected_entity, vector2(mp.x, mp.y)));
		//editor_entity_add
	}
}

void editor_switch_state(Editor* editor)
{
	if(IsKeyReleased(KEY_F2))
	{
		editor->bstate = BRUSHSTATE_TILE;
	}
	if(IsKeyReleased(KEY_F3))
	{
		editor->bstate = BRUSHSTATE_ENTITY;
	}
	if(IsKeyReleased(KEY_F1))
	{
		editor->state = (editor->state + 1) % EDITORSTATEMAXSIZE;
		Map* tmp = editor->map;
		editor->map = editor->brush_map;
		editor->brush_map = tmp;
	}
}

void editor_save(Editor* editor)
{
	if(IsKeyReleased(KEY_P))
	{
		P_EDITORINFO("%s", "Saved Map\n");
		editor_save_level(editor, "../maps/test.tmp");
	}
}

void editor_copy_tile(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonDown(0) && IsKeyDown(KEY_C))
	{
		V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		Tile selected = map_get_tile(editor->map, mp);
		editor->brush = selected;
		// editor_set_brush(editor);
	}
}

void editor_delete_tile(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonDown(0) && IsKeyDown(KEY_D))
	{
		V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		Tile selected = map_get_tile(editor->map, mp);
		map_set_tile(editor->map, mp, tile_sheet[0] );
	}
}

void editor_delete_entity(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonPressed(0) && IsKeyDown(KEY_D))
	{
		Vector2 mp = map_get_mouse_cords(editor->map);
		for(i32 i = 0; i < dynList_len(editor->map->entities); i++)
		{
			Entity* ecurr = dynList_get(editor->map->entities, i);
			if(AAB(ecurr, (Vector2) {mp.x, mp.y} ))
			{
				ecurr->state.type = ESTYPE_CLEAR;
				break;
			}
		}
	}
}

void editor_copy_entity(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonPressed(0) && IsKeyDown(KEY_C))
	{
		//V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		Vector2 mp = map_get_mouse_cords(editor->map);
		for(i32 i = 0; i < dynList_len(editor->map->entities); i++)
		{
			Entity* ecurr = dynList_get(editor->map->entities, i);
			if(AAB(ecurr, (Vector2) {mp.x, mp.y} ))
			{
				editor->selected_entity = ecurr->type;
				entity_destroy(editor->showing_entity);
				editor->showing_entity = entity_new_editor(editor->selected_entity, (Vector2) {0.0, 0.0} );
			}
		}
	}
}

void editor_change_brush_dim(Editor* editor)
{
	if(IsKeyReleased(KEY_EQUAL))
	{
		editor->brush_dim += editor->brush_dim >= 4 ? 0 : 1;
	}
	if(IsKeyReleased(KEY_MINUS))
	{
		editor->brush_dim -= editor->brush_dim <= 1 ? 0 : 1;
	}
}

void editor_move_camera(Editor* editor, MapCamera* cam)
{
	if(IsKeyDown(KEY_SPACE))
	{
		Vector2 mp = Vector2Subtract(Vector2Scale(GetMousePosition(), (f32) 1 / cam->tile_len), cam->offset);
		Vector2 mp_raw = Vector2Scale(GetMousePosition(), (f32) 1 / cam->tile_len);
		Vector2 mid = Vector2Scale(vector2(GetScreenWidth() / cam->tile_len, GetScreenHeight() / cam->tile_len), 0.5);

		Vector2 sub = Vector2Subtract(mp_raw, mid);
		const f32 theta = atan2(sub.y, sub.x);
		const f32 dist = Vector2Distance(mp_raw, mid);
		const f32 speed = 0.35; //0.00015 * (dist * dist * dist);

		cam->pos = Vector2Add(cam->pos, vector2(speed * cos(theta), speed * sin(theta)));
	}
}

const u8* key_tutorial_text[] = 
{
	"F1: Color map",
	"F2: Tile view",
	"F3: Entity view",
	"P: Save Map",
	"M1 + C: Copy Tile/Entity",
	"M1 + D: Delete Tile/Entity",
	"Scroll: Zoom in/out",
};

void editor_print_tutorial()
{
	const u8 font_size = 25;
	const u32 x = 25;
	const u32 y = 25;
	const u32 y_offset = 30;
	if(IsKeyDown(KEY_TAB))
	{
		for(i32 i = 0; i < sizeof(key_tutorial_text) / sizeof(key_tutorial_text[0]); i++)
		{
			DrawText(key_tutorial_text[i], x, y + y_offset * i, font_size, DARKGREEN);
		}
	}
}

void editor_print_info(Editor* editor)
{
	const u8 font_size = 15;
	const u32 x = GetScreenWidth() - GetScreenWidth() / 8;
	const u32 y = GetScreenHeight() - GetScreenHeight() / 16;
	const u32 y_offset = 30;
	DrawText(TextFormat("Brush Size: %d", editor->brush_dim), x, y + y_offset * 0, font_size, DARKGREEN);
}

void editor_tick(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	switch(editor->state)
	{
		case EDITORSTATE_MAINCANVAS:
			cam_tick(editor->map, cam->pos);
			editor_switch_state(editor);
			editor_move_camera(editor, editor->map->camera);
			editor_save(editor);
			switch(editor->bstate)
			{
				case BRUSHSTATE_TILE:
					editor_copy_tile(editor);
					editor_change_brush_dim(editor);
					editor_delete_tile(editor);
					editor_select_tile(editor);
					break;
				case BRUSHSTATE_ENTITY:
					editor_copy_entity(editor);
					editor_push_selected_entity(editor);
					editor_delete_entity(editor); //TODO dsnt wrk
					break;
				default: break;
			}
			break;
		case EDITORSTATE_BRUSHCANVAS:
			cam_tick(editor->map, editor->map->camera->pos);
			editor_switch_state(editor);
			editor_move_camera(editor, editor->map->camera);
			switch(editor->bstate)
			{
				case BRUSHSTATE_TILE:
					editor_copy_tile(editor);
					break;
				case BRUSHSTATE_ENTITY:
					editor_copy_entity(editor);
					break;
			}
			break;
	}
}

void editor_entity_render(Entity* self, Editor* editor)
{
	MapCamera* camera = editor->map->camera;
	if(self->type == 0)
	{
		DrawTexturePro(editor->gfx->texs[TEXTURE_TILEMAP], self->state.sprite.rec_bmap, (Rectangle) {(self->pos.x - camera->offset.x) * editor->map->camera->tile_len, (self->pos.y - camera->offset.y) * editor->map->camera->tile_len, self->dim.x * editor->map->camera->tile_len, self->dim.y * editor->map->camera->tile_len}, (Vector2) {0}, 0.0, RED);
	}
	else
	{
		DrawTexturePro(editor->gfx->texs[TEXTURE_TILEMAP], self->state.sprite.rec_bmap, (Rectangle) {(self->pos.x - camera->offset.x) * editor->map->camera->tile_len, (self->pos.y - camera->offset.y) * editor->map->camera->tile_len, self->dim.x * editor->map->camera->tile_len, self->dim.y * editor->map->camera->tile_len}, (Vector2) {0}, 0.0, GREEN);
	}
}

void editor_check_dead_entity(DynList* entities, Entity* e, i32* i)
{
	if(e->state.type == ESTYPE_CLEAR)
	{
		dynList_del(entities, (*i) --);
		entity_destroy(e);
	}
}

void editor_entities_render(Editor* editor)
{
	for(i32 i = 0; i < dynList_len(editor->map->entities); i++)
	{
		Entity* e = dynList_get(editor->map->entities, i);
		editor_entity_render(e, editor);
		editor_check_dead_entity(editor->map->entities, e, &i);
	}
}

void editor_render_ui(Editor* editor)
{
	temporary_text_render(editor->temp_texts);
}

void editor_render(Editor* editor)
{

	V2 length = v2_new(editor->map->camera->visible_tiles.x, editor->map->camera->visible_tiles.y);
	MapCamera* cam = editor->map->camera;
	V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + editor->map->camera->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + editor->map->camera->offset.y));
	i32 start_x = (i32)floorf(cam->offset.x);
	i32 start_y = (i32)floorf(cam->offset.y);

	for (i32 x = 0; x <= cam->visible_tiles.x + 0; x++)
	{
		for (i32 y = 0; y <= cam->visible_tiles.y + 0; y++)
		{
			i32 tx = start_x + x;
			i32 ty = start_y + y;

			if (tx < 0 || ty < 0 || tx >= editor->map->dim.x || ty >= editor->map->dim.y)
			{
				continue;
			}
			Tile tile = map_get_tile(editor->map,
					v2_new(start_x + x, start_y + y));

			Rectangle rec_dst = {
				x * cam->tile_len - cam->tile_offset.x,
				y * cam->tile_len - cam->tile_offset.y,
				cam->tile_len,
				cam->tile_len
			};
			Rectangle rec_tex = tilemap_textures[tile.type];
			Texture2D tex = editor->gfx->texs[TEXTURE_TILEMAP];

			DrawTexturePro(tex, rec_tex, rec_dst, (Vector2){0}, 0.0f, WHITE);
		}
	}

	if(!IsKeyDown(KEY_D))
	{
		switch(editor->bstate)
		{
			case BRUSHSTATE_TILE:
				for(i32 i = 0; i < editor->brush_dim; i++)
				{
					for(i32 j = 0; j < editor->brush_dim; j++)
					{
						V2 mp_full = v2_add(mp, (V2) {i, j});
						DrawTexturePro(editor->gfx->texs[TEXTURE_TILEMAP], tilemap_textures[editor->brush.type], (Rectangle) {(mp_full.x - editor->map->camera->offset.x) * editor->map->camera->tile_len, (mp_full.y - editor->map->camera->offset.y) * editor->map->camera->tile_len, editor->map->camera->tile_len, editor->map->camera->tile_len}, (Vector2) {0}, 0.0, WHITE);
					}
				}
				break;
			case BRUSHSTATE_ENTITY:
				//editor->showing_entity->pos = Vector2Subtract(map_get_mouse_cords(editor->map), Vector2Scale(editor->showing_entity->dim, 0.5));
				editor->showing_entity->pos = (Vector2) {mp.x, mp.y};//(map_get_mouse_cords(editor->map));
				editor_entity_render(editor->showing_entity, editor);
				//DrawTexturePro(editor->gfx->texs[TEXTURE_TILEMAP], tilemap_textures[editor->brush.type], (Rectangle) {(mp.x - editor->map->camera->offset.x) * editor->map->camera->tile_len, (mp.y - editor->map->camera->offset.y) * editor->map->camera->tile_len, editor->map->camera->tile_len, TILE_LEN}, (Vector2) {0}, 0.0, WHITE);
				break;

		}
	}
	//DrawRectangle((cam->pos.x - editor->map->camera->offset.x) * editor->map->camera->tile_len, (cam->pos.y - editor->map->camera->offset.y) * editor->map->camera->tile_len, editor->map->camera->tile_len, TILE_LEN, DARKBLUE);
	if(editor->bstate == BRUSHSTATE_ENTITY) editor_entities_render(editor);
	editor_print_tutorial();
	editor_print_info(editor);
	//DrawRectangle((mp.x - editor->map->camera->offset.x) * editor->map->camera->tile_len, (mp.y - editor->map->camera->offset.y) * editor->map->camera->tile_len, editor->map->camera->tile_len, TILE_LEN, DARKBLUE);
	editor_render_ui(editor);
}



