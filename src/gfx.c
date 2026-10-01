
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/gfx.h"
#include "../include/global.h"
#include "raylib.h"
#include "raymath.h"


#define IDK 1.35
u32 gfx_to_monitor2(u32 pixels)
{
	Vector2 monitor = { MONITOR_WIDTH, MONITOR_HEIGHT };
	return ceilf(monitor.y / ( DEFAULT_RES_Y / pixels) ) * IDK;
}

u32 gfx_to_monitor(u32 pixels)
{
    float scale = fminf(
        (float)MONITOR_WIDTH / DEFAULT_RES_X,
        (float)MONITOR_HEIGHT / DEFAULT_RES_Y
    );

    return (u32)ceilf(pixels * scale);
}

Vector2 gfx_to_monitor_vector2(Vector2 pixels)
{
	Vector2 monitor = { MONITOR_WIDTH, MONITOR_HEIGHT };
	
	return (Vector2) { ceilf(monitor.x / ( DEFAULT_RES_X / pixels.x) * IDK), ceilf(monitor.y / ( DEFAULT_RES_Y / pixels.y )) * IDK};
}
Vector2 gfx_to_monitor_vector(Vector2 pixels)
{
    float scaleX = (float)MONITOR_WIDTH / DEFAULT_RES_X;
    float scaleY = (float)MONITOR_HEIGHT / DEFAULT_RES_Y;

    return (Vector2){
        pixels.x * scaleX,
        pixels.y * scaleY
    };
}
Rectangle gfx_to_monitor_rectangle(Rectangle r)
{
    float scaleX = (float)MONITOR_WIDTH / DEFAULT_RES_X;
    float scaleY = (float)MONITOR_HEIGHT / DEFAULT_RES_Y;

    return (Rectangle){
        r.x * scaleX,
        r.y * scaleY,
        r.width * scaleX,
        r.height * scaleY
    };
}
Rectangle gfx_to_monitor_rectangle2(Rectangle pixels)
{
	Rectangle monitor = { MONITOR_WIDTH, MONITOR_HEIGHT };
	
	return (Rectangle) { ceilf(monitor.x / ( DEFAULT_RES_X / pixels.x) * IDK), 
		ceilf(monitor.y / ( DEFAULT_RES_Y / pixels.y) * IDK),
		ceilf(monitor.x / ( DEFAULT_RES_X / pixels.width ) * IDK),
		ceilf(monitor.y / ( DEFAULT_RES_Y / pixels.height ) * IDK) 
	};
}

TemporaryText* temporary_text_new(const u8* text, 
		const Vector2 pos, 
		const u32 dim, 
		const u32 timer, 
		const Color color)
{
	TemporaryText* tmp_text_new = malloc(sizeof(TemporaryText));	
	strcpy(tmp_text_new->text, text);
	tmp_text_new->pos = pos;
	tmp_text_new->color = color;
	tmp_text_new->dim = dim;
	tmp_text_new->timer = timer;
	tmp_text_new->time = 0;

	return tmp_text_new;
}

void temporary_text_destroy(TemporaryText* t)
{
	free(t);
}

void temporary_text_render(DynList* ts)
{
	for(i32 i = 0; i < dynList_len(ts); i++)
	{
		TemporaryText* temp_text = dynList_get(ts, i);
		DrawText(temp_text->text, temp_text->pos.x, temp_text->pos.y, temp_text->dim, temp_text->color);
		if(temp_text->time >= temp_text->timer - 60)
		{
			if(temp_text->color.a >= 16)
			{
				temp_text->color.a -= 4;
			}
		}
		if(temp_text->time ++ >= temp_text->timer)
		{
			dynList_del(ts, i --);
			temporary_text_destroy(temp_text);
		}
	}
}


static const Rectangle mouse_render_table[][2] = {
	{ {16, 64, 16, 16}, {0, 0, 24, 24} },
	{ {0, 64, 16, 16}, {0, 0, 48, 48} },
	{ {0, 16, 16, 16}, {0, 0, 32, 32} },
};

void gfx_render_mouse(Gfx* gfx)
{
	Rectangle dst = mouse_render_table[gfx->mouse.type][1];
	const Vector2 mouse_cords = Vector2SubtractValue(GetMousePosition(), dst.height / 2); 
	Color color = IsMouseButtonDown(0) ? (Color) LIGHTGRAY : WHITE;
	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			mouse_render_table[gfx->mouse.type][0],
			(Rectangle) {mouse_cords.x, mouse_cords.y, dst.width, dst.height},
			(Vector2) {0},
			0.0,
			color);
}

static const u8* texture_filenames[] = 
{
	"../assets/TileMap.png",
	"../assets/EditorUI.png",
	"../assets/UI.png",
	"../assets/SkillDisplay.png",
};

LightGfx* lightgfx_new()
{
	LightGfx* lightgfx = malloc(sizeof(LightGfx));

	*lightgfx = (LightGfx) {
		.map = LoadRenderTexture(GetScreenWidth(), GetScreenHeight()),
	};

	return lightgfx;

}

void gfx_load_textures(Texture2D* texs)
{
	const u32 len = sizeof(texture_filenames) / sizeof(texture_filenames[0]);
	for(int i = 0; i < len; i++)
	{
		texs[i] = LoadTexture(texture_filenames[i]);	
	}
}

Gfx* gfx_new()
{
	Gfx* gnew = malloc(sizeof(Gfx));
	*gnew = (Gfx) {
		.texs = malloc(sizeof(Texture2D) * TEXTURE_COUNT),
			.font = LoadFontEx("../fonts/PixelSans.ttf", 64, NULL, 0),
			.ui_font = LoadFontEx("../fonts/Inter-VariableFont_opsz,wght.ttf", 64, NULL, 0),
			.light_map = lightgfx_new(),
			.mouse = (MouseGfx) {
				.type = MOUSE_TYPE_STANDARD,
			},
	};
	SetTextureFilter(gnew->font.texture, TEXTURE_FILTER_BILINEAR);
	SetTextureFilter(gnew->ui_font.texture, TEXTURE_FILTER_BILINEAR);
	gfx_load_textures(gnew->texs);

	return gnew;
}






