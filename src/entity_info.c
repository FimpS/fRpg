
#include "../include/entity_info.h"

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
