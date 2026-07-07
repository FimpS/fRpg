#pragma once

#include "raylib.h"
#include "enum.h"
#include "../lib/types.h"
#include "../lib/dynList.h"

#define TEXTURE_COUNT 3
#define MAX_SCREEN_STRING_LEN 48
#define DEFAULT_RES_X 1920
#define DEFAULT_RES_Y 1080

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080

#define MAX_ANIMATION_FRAMES 8

typedef struct TemporaryText
{
	u8 text[MAX_SCREEN_STRING_LEN];
	Vector2 pos;
	Color color;
	u32 dim;
	u32 time;
	u32 timer;
} TemporaryText;

typedef struct Animation
{
	Rectangle frames[MAX_ANIMATION_FRAMES];
	u32 amount_frames;
	u32 timer;
	u32 stop_timer;
} Animation;

typedef struct LightGfx
{
	RenderTexture2D map;
} LightGfx;

typedef enum MouseType
{
	MOUSE_TYPE_STANDARD,
	MOUSE_TYPE_HAMMER,
	MOUSE_TYPE_BURN,
} MouseType;

typedef struct MouseGfx
{
	MouseType type;
} MouseGfx;

typedef struct Gfx
{
	Font font;
	LightGfx* light_map;
	Texture2D* texs;

	MouseGfx mouse;
} Gfx;

Gfx* gfx_new();

Vector2 gfx_to_monitor_vector(Vector2 pixels);
Rectangle gfx_to_monitor_rectangle(Rectangle pixels);
u32 gfx_to_monitor(u32 pixels);

void gfx_render_mouse(Gfx* gfx);

TemporaryText* temporary_text_new(const u8* text,
		const Vector2 pos,
		const u32 dim,
		const u32 timer,
		const Color color);
void temporary_text_destroy(TemporaryText* t);
void temporary_text_render(DynList* ts);

