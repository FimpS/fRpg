
#include "sound.h"

void play_sound_multi(SoundSnippet* sound)
{
	PlaySound(sound->sound[sound->index ++]);
	if(sound->index >= MAX_SNIPPET) sound->index = 0;
}

void play_sound(SoundSnippet* sound)
{
	PlaySound(sound->sound[0]);
}

MapSound* map_sound_new()
{
	MapSound* map_sound = malloc(sizeof(MapSound));
	map_sound->len = 1;
	map_sound->sounds[0].sound[0] = LoadSound("../soundassets/woosh.mp3");
	map_sound->sounds[0].index = 0;
	for(i32 i = 0; i < MAX_SNIPPET; i++)
	{
		map_sound->sounds[0].sound[i] = LoadSoundAlias(map_sound->sounds[0].sound[0]);
	}

	return map_sound;
}
