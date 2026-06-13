
#include "../include/entity_data.h"

const EntityData entity_data_table[] = {
// 	TYPE 						StartState							Texture					Dim 			Bspeed		Lightdata			Pulse					Flicker
	{ENTITY_PLACEHOLDER,		ESTYPE_ENTITY_MOVE_ATTACK,			TEXTURE_TILEMAP, 		{1.0, 1.0},		0.05,		{0.9, 1.0, true,	{true, 1.0, 2.0},		{false,	0.0, 0.0, 0}, WHITE},		},
	{ENTITY_PLACEHOLDER2,		ESTYPE_PLACEHOLDER2,				TEXTURE_TILEMAP, 		{0.5, 0.5},		0.10,		{1.0, 3.0, true,	{true, 0.8, 0.5},		{true, 	0.95, 1.0, 4}, WHITE},		},
	{ENTITY_PLAYER,				ESTYPE_PLAYER_TICK,					TEXTURE_TILEMAP,		{1.0, 1.0},		0.10,		{0.9, 4.0, true,	{true, 0.9, 0.2},		{false, 0.0, 0.0, 0}, WHITE},		},
};

