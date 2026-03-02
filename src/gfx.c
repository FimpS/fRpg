
#include <stdlib.h>
#include <string.h>

#include "gfx.h"


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

static const u8* texture_filenames[] = 
{
	"../assets/TileMap.png",
	"../assets/EditorUI.png",
};

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
	};

	gfx_load_textures(gnew->texs);

	return gnew;
}
