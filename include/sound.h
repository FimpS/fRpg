#pragma once

#include "struct.h"

#define MAX_SOUND_STRING_LEN 32
#define MAX_MAP_SOUNDS 64

typedef struct SoundStrings
{
	char body[MAX_SOUND_STRING_LEN][MAX_MAP_SOUNDS];
	u32 len;
} SoundStrings;

void play_sound_multiple(SoundMultiple* sound);
void play_sound(SoundMultiple* sound);
MapSound* map_sound_new();

