
#include "../include/entity_info.h"

const EntityData entity_data_table[] = {
	//TYPE 						//StartState				//dim 			//bspeed	//Lightdata
	{ENTITY_PLACEHOLDER,		ESTYPE_PLACEHOLDER,			{1.0, 1.0},		0.10,		{0.9, 0.0, false, {0.8, 0.1}, WHITE},		},
	{ENTITY_PLACEHOLDER2,		ESTYPE_PLACEHOLDER2,		{0.5, 0.5},		0.10,		{0.0, 0.0, true, {1.0, 0.0}, WHITE},				},
	{ENTITY_PLAYER,				ESTYPE_PLAYER_TICK,			{1.5, 1.5},		0.10,		{0.7, 8.0, true, {1.0, 0.0}, WHITE},				},
};

#if 0

const Entity entity_type_table[] = {
	{
		.dim = { 1.0, 1.0 },
		.state = {
			.type = ESTYPE_PLACEHOLDER2,
		},
		.light = {
			.distance = 0.0,
			.self = 1.0,
			.tint = WHITE,
			.value = 0.0,
			.flicker = {0.8, 0.1},
			.light_source = false,
		},
	},
	(Entity) {
		.dim = { 0.5, 0.5 },
		.state = {
			.type = ESTYPE_PLACEHOLDER2,
		},
		.light = {
			.light_source = true,
		},
	},
	(Entity) {
		.dim = {1.5, 1.5},
		.state = {
			.type = ESTYPE_PLAYER_TICK,
		},
		.speed = { 0.0, 0.1 },
		.light = {
			.self = 1.0,
			.light_source = false,
			.distance = 0.0,
			.flicker = 0.0,
		},
	},
};
#endif
