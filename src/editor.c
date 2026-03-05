#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/editor.h"
#include "../include/global.h"
#include "raylib.h"
#include "raymath.h"

#define ENTITY_TYPE_LIST_LEN ENTITY_LAST


#define DEBUG_MODE 1
#define P_EDITORINFO(s, ...) if ( DEBUG_MODE )  { printf("EDITOR INFO: "); printf(s, __VA_ARGS__); }

void editor_parse_file_input(u8* file_buffer)
{
	u8 filepath[MAX_FILE_LEN];
	strcpy(filepath, "../maps/");
	strcat(filepath, file_buffer);
	strcat(filepath, ".tmp");
	strcpy(file_buffer, filepath);
}

bool editor_load_level(Map* map, const char* filepath)
{
	FILE* fp = NULL;

	fp = fopen(filepath, "rb");
	if(!fp)
	{
		P_ERROR("ERROR: Failed to open file\n");
		return false;
	}

	fread(&map->dim, sizeof(V2), 1, fp);

	u32 c = 0;
	if((c = fread(map->content, sizeof(Tile), map->dim.x * map->dim.y, fp)) != map->dim.x * map->dim.y)
	{
		P_ERROR("File failed to read appropriate bytes\n");
	}
	printf("bytes: %d %d %d\n", c, map->dim.x, map->dim.y);
	u32 entity_list_len = 0;
	fread(&entity_list_len, sizeof(unsigned), 1, fp);

	for (u32 i = 0; i < entity_list_len; i++)
	{
		Entity* allocated_entity = malloc(sizeof(Entity));
		fread(allocated_entity, sizeof(Entity), 1, fp);
		dynList_push(map->entities, allocated_entity);
	}
	fclose(fp);
	return true;
}

void editor_push_saved(Editor* editor)
{
	u8 text[MAX_FILE_LEN];
	strcpy(text, "Saved map to ");
	strcat(text, editor->filename + 8);
	strcat(text, "...");
	dynList_push(editor->temp_texts, temporary_text_new(
				text,
				(Vector2) {GetScreenWidth() - GetScreenHeight() / 2, GetScreenHeight() / 24},
				20 + (i32)((f32)(GetScreenWidth() + GetScreenHeight()) / 364),
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

	fwrite(&map->dim, sizeof(V2), 1, fp);

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

static const Rectangle entity_textures[] =
{
	{0, 0, 16, 16},
	{0, 0, 16, 16},
};

void editor_reset_brushmap(Editor* editor)
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

void editor_hotbar_init(Editor* editor)
{
	const Tile t = tile_sheet[0];
	for(i32 i = 0; i < HOTBAR_LEN; i++)
	{
		editor->hotbar[i] = t;
	}
	editor->hotbar[2] = tile_sheet[1];
	editor->hotbar[0] = tile_sheet[2];
	editor->hotbar[3] = tile_sheet[3];
}

void editor_entity_hotbar_init(Editor* editor)
{
	EntityType t = ENTITY_PLACEHOLDER;
	for(i32 i = 0; i < HOTBAR_LEN; i++)
	{
		editor->entity_hotbar[i] = t;
	}
}

void editor_init_empty_level(Editor* editor)
{
	Map* map = editor->map;
	for(int x = 0; x < map->dim.x; x++)
	{
		for(int y = 0; y < map->dim.y; y++)
		{
			map->content[y * map->dim.x + x] = (Tile) {
				.type = 0,
					.animated = false,
					.solid = false,	
			};
		}
	}
}

Editor* editor_new(const u8* map_filename, V2 map_dim)
{
	Editor* editor = malloc(sizeof(Editor));

	editor->temp_texts = dynList_new();
	editor->selected_hotbar = 0;
	editor_hotbar_init(editor);
	editor_entity_hotbar_init(editor);
	editor->lock_hotbar = false;

	editor->spline = (EditorMode) {
		.start_cord = (V2) {0, 0},
		.mode = false,
		.end_cord = (V2) {0, 0},
	};
	editor->quad = (EditorMode) {
		.start_cord = (V2) {0, 0},
		.mode = false,
		.end_cord = (V2) {0, 0},
	};
	editor->quad_buffer = NULL;
	editor->quad_dim = (V2) {0, 0};
	editor->quad_copied = false;

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
#if 0
	for(int x = 0; x < editor->map->dim.x; x++)
	{
		for(int y = 0; y < editor->map->dim.y; y++)
		{
			editor->map->content[y * editor->map->dim.x + x] = (Tile) {
				.type = 0,
					.animated = false,
			};
		}
	}
#endif
	strcpy(editor->filename, map_filename);
	bool file_exists = editor_load_level(editor->map, editor->filename);
	if(!file_exists)
	{
		editor->map->dim = map_dim;
		editor_init_empty_level(editor);
	}
	editor_reset_brushmap(editor);
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
	if(IsMouseButtonDown(0) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_C) && !IsKeyDown(KEY_LEFT_SHIFT))
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
	if(IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_CONTROL))
	{
		P_EDITORINFO("%s", "Saved Map\n");
		editor_save_level(editor, editor->filename);
	}
}

void editor_copy_tile(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(IsMouseButtonDown(0) && IsKeyDown(KEY_C))
	{
		V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		Tile selected = map_get_tile(editor->map, mp);
		if(!editor->lock_hotbar)
		{
			editor->hotbar[editor->selected_hotbar] = selected;
		}
		editor->brush = selected;
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
				if(!editor->lock_hotbar)
				{
					editor->entity_hotbar[editor->selected_hotbar] = editor->showing_entity->type;
				}
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
	"Ctrl + Alt + R: Reload last save",
	"+ : Increase brush size",
	"- : Decrease brush size",
	"M1 + C: Copy Tile/Entity",
	"M1 + D: Delete Tile/Entity",
	"M1 + Shift: Spline mode (M2 to cancel, M1 to draw)",
	"Shift + R: Rectangle mode",
	"Scroll: Zoom in/out",
	"L: Lock/Unlock hotbar",
	"P: Save Map",
	"F11: Toggle fullscreen",
};

void editor_print_tutorial()
{
	const u8 font_size = 30;
	const u32 x = 25;
	const u32 y = 25;
	const u32 y_offset = 35;
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
	const u8 font_size = (GetScreenWidth() + GetScreenWidth()) / 156;
	const u32 x = GetScreenWidth() - GetScreenWidth() / 8;
	const u32 y = GetScreenHeight() - GetScreenHeight() / 16;
	const u32 y_offset = 20;
	DrawText(TextFormat("Brush Size: %d", editor->brush_dim), x, y + y_offset * 0, font_size, DARKGREEN);
	if(editor->lock_hotbar)
	{
		DrawText("Hotbar Locked", x, y - y_offset * 1, font_size, RED);
	}
	else
	{
		DrawText("Hotbar UnLocked", x, y - y_offset * 1, font_size, DARKGREEN);
	}
}

void editor_toggle_hotbar_lock(Editor* editor)
{
	if(IsKeyReleased(KEY_L))
	{
		editor->lock_hotbar = !editor->lock_hotbar;
	}
}

void editor_reload_last_save(Editor* editor)
{
	if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_ALT) && IsKeyReleased(KEY_R))
	{
		const u32 len = dynList_len(editor->map->entities);
		for(i32 i = 0; i < len; i++)
		{
			Entity* e = dynList_get(editor->map->entities, i);
			dynList_pop(editor->map->entities);
			entity_destroy(e);
		}
		editor_load_level(editor->map, editor->filename);
	}
}

void editor_toggle_fullscreen()
{
	if(IsKeyReleased(KEY_F11))
	{
		ToggleFullscreen();
	}
}

void editor_spline(Map* map, V2 start, V2 end, Tile tile)
{
    i32 x0 = start.x;
    i32 y0 = start.y;
    i32 x1 = end.x;
    i32 y1 = end.y;

    i32 dx = abs(x1 - x0);
    i32 dy = abs(y1 - y0);

    i32 sx = (x0 < x1) ? 1 : -1;
    i32 sy = (y0 < y1) ? 1 : -1;

    i32 err = dx - dy;

    while (true)
    {
        map_set_tile(map, v2_new(x0, y0), tile);

        if (x0 == x1 && y0 == y1)
            break;

        i32 e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void editor_draw_line(MapCamera* cam, V2 v1, V2 v2, Color color)
{
	DrawLine((v1.x - cam->offset.x) * cam->tile_len, 
			 (v1.y - cam->offset.y) * cam->tile_len, 
			 (v2.x - cam->offset.x) * cam->tile_len, 
			 (v2.y - cam->offset.y) * cam->tile_len, 
			 color);
}

void editor_spline_mode(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(!editor->spline.mode && IsMouseButtonPressed(0) && IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_R))
	{
		editor->spline.mode = true;
		editor->spline.start_cord = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		return;
	}
	if(editor->spline.mode)
	{
		editor->spline.end_cord = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		V2 v1 = editor->spline.start_cord;
		V2 v2 = editor->spline.end_cord;
		editor_draw_line(cam, v1, v2, RED);
	}

	if(editor->spline.mode && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
	{
		editor->spline.mode = false;
	}

	if(editor->spline.mode && IsMouseButtonPressed(0))
	{
		editor->spline.end_cord = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		const V2 v1 = editor->spline.start_cord;
		const V2 v2 = editor->spline.end_cord;
		editor->spline.mode = false;
		editor_spline(editor->map, v1, v2, editor->brush);
	}

}

void editor_copy_selection(Editor* editor, const V2 v1, const V2 v2)
{

    const i32 x1 = v1.x < v2.x ? v1.x : v2.x;
    const i32 y1 = v1.y < v2.y ? v1.y : v2.y;
    const i32 x2 = v1.x > v2.x ? v1.x : v2.x;
    const i32 y2 = v1.y > v2.y ? v1.y : v2.y;

	editor->quad_dim = (V2) {
		.x = abs(x1 - x2) + 1,
		.y = abs(y1 - y2) + 1,
	};

	if(editor->quad_buffer != NULL) free(editor->quad_buffer);	
	
	const u32 buffer_size = editor->quad_dim.x * editor->quad_dim.y;
	if(buffer_size >= 1 << 16)
	{
		P_ERROR("BUFFER TOO LARGE");
		return;
	}
	editor->quad_buffer = malloc(sizeof(Tile) * buffer_size);



	for(i32 x = 0; x < editor->quad_dim.x; x++)
	{
		for(i32 y = 0; y < editor->quad_dim.y; y++)
		{
			const V2 c_pos = (V2) {x1 + x, y1 + y};
			editor->quad_buffer[y * editor->quad_dim.x + x] = map_get_tile(editor->map, c_pos);
		}
	}
	editor->quad_copied = true;
}

void editor_quad_copy_mode(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	if(!editor->quad.mode && IsMouseButtonPressed(0) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_R))
	{
		editor->quad.mode = true;
		editor->quad.start_cord = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		return;
	}

	if(editor->quad.mode)
	{
		editor->quad.end_cord = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		V2 a = editor->quad.start_cord;
		V2 b = editor->quad.end_cord;

		const i32 x1 = a.x < b.x ? a.x : b.x;
		const i32 y1 = a.y < b.y ? a.y : b.y;
		const i32 x2 = a.x > b.x ? a.x : b.x;
		const i32 y2 = a.y > b.y ? a.y : b.y;

		const V2 v1 = {x1, y1};
		const V2 v2 = {x2 + 1, y1};
		const V2 v3 = {x2 + 1, y2 + 1};
		const V2 v4 = {x1, y2 + 1};

		editor_draw_line(cam, v1, v2, RED);
		editor_draw_line(cam, v2, v3, RED);
		editor_draw_line(cam, v3, v4, RED);
		editor_draw_line(cam, v4, v1, RED);
	}

	if(editor->quad.mode && IsMouseButtonPressed(0))
	{
		editor->quad.end_cord = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		V2 v1 = editor->quad.start_cord;
		V2 v2 = editor->quad.end_cord;
		editor_copy_selection(editor, v1, v2);
		editor->quad.mode = false;
	}

	if(editor->quad_copied && IsKeyPressed(KEY_V) && IsKeyDown(KEY_LEFT_CONTROL))
	{
		V2 mp = v2_new(((f32)GetMouseX() / editor->map->camera->tile_len + cam->offset.x), ((f32)GetMouseY() / editor->map->camera->tile_len + cam->offset.y));
		for(i32 x = 0; x < editor->quad_dim.x; x ++)
		{
			for(i32 y = 0; y < editor->quad_dim.y; y++)
			{
				const Tile tile = editor->quad_buffer[y * editor->quad_dim.x + x];
				const V2 pos = (V2) {mp.x + x, mp.y + y};
				map_set_tile(editor->map, pos, tile);
			}
		}
	}
}

void editor_tick(Editor* editor)
{
	MapCamera* cam = editor->map->camera;
	editor_toggle_hotbar_lock(editor);
	editor_reload_last_save(editor);
	editor_toggle_fullscreen();
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

void editor_render_inventory(Editor* editor)
{
	Gfx* gfx = editor->gfx;

	i32 select = GetKeyPressed();
	if(select >= KEY_ONE && select <= KEY_NINE)
	{
		editor->selected_hotbar = select - KEY_ONE;
		editor->brush = editor->hotbar[editor->selected_hotbar];
		editor->selected_entity = editor->entity_hotbar[editor->selected_hotbar];

		editor->selected_entity = editor->entity_hotbar[editor->selected_hotbar];
		entity_destroy(editor->showing_entity);
		editor->showing_entity = entity_new_editor(editor->selected_entity, (Vector2) {0.0, 0.0} );

	}

	const Rectangle hotbar_src = {
		.x = 0,
		.y = 48,
		.width = 180,
		.height = 22,
	};
	const i32 width_offset = (i32)((f32)(64 * 4.5));
	const Rectangle hotbar_dst = {
		.x = GetScreenWidth() / 2 - width_offset,
		.y = GetScreenHeight() - GetScreenHeight() / 8,
		.width = 64 * 9,
		.height = 64,
	};
	DrawTexturePro(gfx->texs[TEXTURE_EDITOR_UI], hotbar_src, hotbar_dst, (Vector2) {0}, 0.0, WHITE);


	switch(editor->bstate)
	{
		case BRUSHSTATE_TILE:
			for(i32 i = 0; i < HOTBAR_LEN; i++)
			{
				const Rectangle hotbar_item_src = tilemap_textures[editor->hotbar[i].type];
				const i32 width_offset = (i32)((f32)(64 * 4.5));
				const Rectangle hotbar_item_dst = {
					.x = GetScreenWidth() / 2 - width_offset + i * 64 + 9,
					.y = GetScreenHeight() - GetScreenHeight() / 8 + 8,
					.width = 46,
					.height = 42,
				};
				DrawTexturePro(gfx->texs[TEXTURE_TILEMAP], hotbar_item_src, hotbar_item_dst, (Vector2) {0}, 0.0, WHITE);
			}
			break;
		case BRUSHSTATE_ENTITY:
			for(i32 i = 0; i < HOTBAR_LEN; i++)
			{
				const Rectangle hotbar_item_src = entity_textures[editor->entity_hotbar[i]];
				const i32 width_offset = (i32)((f32)(64 * 4.5));
				const Rectangle hotbar_item_dst = {
					.x = GetScreenWidth() / 2 - width_offset + i * 64 + 9,
					.y = GetScreenHeight() - GetScreenHeight() / 8 + 8,
					.width = 46,
					.height = 42,
				};
				DrawTexturePro(gfx->texs[TEXTURE_TILEMAP], hotbar_item_src, hotbar_item_dst, (Vector2) {0}, 0.0, WHITE);
			}
			break;
	}

	const Rectangle src = {
		.x = 0,
		.y = 0,
		.width = 15,
		.height = 16,
	};
	const Rectangle dst = {
		.x = GetScreenWidth() / 2 - width_offset + editor->selected_hotbar * 64 + 4,
		.y = GetScreenHeight() - GetScreenHeight() / 8 + 4,
		.width = 56,
		.height = 50,
	};
	DrawTexturePro(gfx->texs[TEXTURE_EDITOR_UI], src, dst, (Vector2) {0}, 0.0, WHITE);

}

void editor_render_ui(Editor* editor)
{
	temporary_text_render(editor->temp_texts);
	editor_render_inventory(editor);
}

void editor_render(Editor* editor)
{
    MapCamera* cam = editor->map->camera;
    i32 tile = cam->tile_len;

    i32 start_x = (i32)floorf(cam->offset.x);
    i32 start_y = (i32)floorf(cam->offset.y);

    Vector2 mouse = GetMousePosition();
    V2 mp = {
        (i32)floorf(mouse.x / tile + cam->offset.x),
        (i32)floorf(mouse.y / tile + cam->offset.y)
    };

    Texture2D tex = editor->gfx->texs[TEXTURE_TILEMAP];

    for (i32 x = 0; x < cam->visible_tiles.x + 1; x++)
    {
        for (i32 y = 0; y < cam->visible_tiles.y + 1; y++)
        {
            i32 tx = start_x + x;
            i32 ty = start_y + y;

            Rectangle dst = {
                (tx - cam->offset.x) * tile,
                (ty - cam->offset.y) * tile,
                tile,
                tile
            };

            if (tx < 0 || ty < 0 || tx >= editor->map->dim.x || ty >= editor->map->dim.y)
            {
                DrawRectangleRec(dst, BLACK);
                continue;
            }

            Tile tile_data = map_get_tile(editor->map, (V2){tx, ty});
            Rectangle src = tilemap_textures[tile_data.type];

            DrawTexturePro(tex, src, dst, (Vector2){0}, 0, WHITE);
        }
    }

    if(!IsKeyDown(KEY_D))
    {
        switch(editor->bstate)
        {
            case BRUSHSTATE_TILE:
            {
                for(i32 i = 0; i < editor->brush_dim; i++)
                {
                    for(i32 j = 0; j < editor->brush_dim; j++)
                    {
                        V2 pos = {mp.x + i, mp.y + j};

                        Rectangle dst = {
                            (pos.x - cam->offset.x) * tile,
                            (pos.y - cam->offset.y) * tile,
                            tile,
                            tile
                        };

                        DrawTexturePro(
                            tex,
                            tilemap_textures[editor->brush.type],
                            dst,
                            (Vector2){0},
                            0,
                            WHITE
                        );
                    }
                }
            } break;

            case BRUSHSTATE_ENTITY:
            {
                editor->showing_entity->pos = (Vector2){mp.x, mp.y};
                editor_entity_render(editor->showing_entity, editor);
            } break;
        }
    }

    if(editor->bstate == BRUSHSTATE_ENTITY) editor_entities_render(editor);

    editor_spline_mode(editor);
    editor_quad_copy_mode(editor);

    editor_print_tutorial();
    editor_print_info(editor);

    editor_render_ui(editor);
}


