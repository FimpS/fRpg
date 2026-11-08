
#include "../include/entity_data.h"

const EntityData entity_data_table[] = {
// 	TYPE 						NAME								StartState							Texture					Dim 			Bspeed			Bhealth					Lightdata			Pulse					Flicker
	{ENTITY_PLACEHOLDER,		"Placeholder",						ESTYPE_ENTITY_MOVE_ATTACK,			TEXTURE_TILEMAP, 		{1.0, 1.0},		0.01,			1000,					{0.9, 1.0, true,	{true, 1.0, 2.0},		{false,	0.0, 0.0, 0}, WHITE},		true,	false,	true,				{3, {ITEM_TYPE_KEK_BOOTS, ITEM_TYPE_NONE, ITEM_TYPE_SWORD}}, 			{1, {0.0,0.0,1.0,1.0} }},
	{ENTITY_PLACEHOLDER2,		"Placeholder2",						ESTYPE_PLACEHOLDER2,				TEXTURE_TILEMAP, 		{0.5, 0.5},		0.10,			1000,					{1.0, 3.0, true,	{true, 0.8, 0.5},		{true, 	0.95, 1.0, 4}, WHITE},		false,	true,	false,				{1, {ITEM_TYPE_KEK_BOOTS}},				{1, {0.0,0.0,0.5,0.5} }},
	{ENTITY_PLAYER,				"Player",							ESTYPE_PLAYER_TICK,					TEXTURE_TILEMAP,		{1.0, 1.0},		0.10,			1000,					{0.9, 4.0, true,	{true, 0.9, 0.2},		{false, 0.0, 0.0, 0}, WHITE},		false,	false,	true,				{1, {ITEM_TYPE_KEK_BOOTS}},				{1, {0.0,0.0,1.0,1.0} }},
	{ENTITY_ITEM_DROP,			"ITEM_DROP",						ESTYPE_ITEM_DROP_TICK,				TEXTURE_TILEMAP, 		{0.4, 0.4},		0.10,			1000,					{1.0, 1.0, true,	{false, 0.8, 0.5},		{false,}, 				WHITE},		false,	true,	true, 				{0, {0}},								{1, {0.0,0.0,0.4,0.4} }},
	{ENTITY_SHOP_NPC,			"Shop NPC",							ESTYPE_PLACEHOLDER2,				TEXTURE_TILEMAP,		{2.0, 2.0},		0.10,			1000,					{0.5, 5.0, true,	{false, 0.9, 0.2},		{false, 0.0, 0.0, 0}, RED},			false,	false,	true,				{1, {ITEM_TYPE_KEK_BOOTS}},				{1, {0.0,0.0,2.0,2.0} }},
};

