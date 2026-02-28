
#include <stdlib.h>

#include "gfx.h"

void gfx_load_textures(Texture2D* texs)
{
	const char* filenames[] = 
	{
		"../assets/TileMap.png",
	};
	for(int i = 0; i < TEXTURE_COUNT; i++)
	{
		texs[i] = LoadTexture(filenames[i]);	
	}
}

Gfx* gfx_new()
{
	Gfx* gnew = malloc(sizeof(Gfx));
	*gnew = (Gfx) {
		.texs = malloc(sizeof(Texture2D) * TEXTURE_COUNT),
	};

	gfx_load_textures(gnew->texs);

	return gnew;
}
