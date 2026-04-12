#ifndef GFX_H
#define GFX_H

#include "raylib.h"
#include "../lib/types.h"
#include "../lib/dynList.h"

#define TEXTURE_COUNT 3
#define MAX_SCREEN_STRING_LEN 48
#define DEFAULT_RES_X 1920
#define DEFAULT_RES_Y 1080

typedef struct TemporaryText
{
	u8 text[MAX_SCREEN_STRING_LEN];
	Vector2 pos;
	Color color;
	u32 dim;
	u32 time;
	u32 timer;
} TemporaryText;

typedef enum TextureIndex
{
	TEXTURE_TILEMAP,
	TEXTURE_EDITOR_UI,
	TEXTURE_GAME_UI,
} TextureIndex;


typedef struct Light
{
	Vector2 pos;
	f32 distance;
	bool light_source;
	f32 self;
	Vector2 flicker;
	Color tint;
	f32 value;
} Light;

typedef struct LightGfx
{
	RenderTexture2D map;
} LightGfx;

typedef struct Gfx
{
	Font font;
	LightGfx* light_map;
	Texture2D* texs;
} Gfx;

Gfx* gfx_new();

Vector2 gfx_to_monitor_vector(Vector2 pixels);
Rectangle gfx_to_monitor_rectangle(Rectangle pixels);
u32 gfx_to_monitor(u32 pixels);

TemporaryText* temporary_text_new(const u8* text,
								  const Vector2 pos,
								  const u32 dim,
								  const u32 timer,
								  const Color color);
void temporary_text_destroy(TemporaryText* t);
void temporary_text_render(DynList* ts);

#endif
