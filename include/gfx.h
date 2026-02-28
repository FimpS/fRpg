#ifndef GFX_H
#define GFX_H

#include "raylib.h"

#define TEXTURE_COUNT 1


typedef enum TextureIndex
{
	TEXTURE_TILEMAP,
} TextureIndex;

typedef struct Gfx
{
	Texture2D* texs;
} Gfx;

Gfx* gfx_new();

#endif
