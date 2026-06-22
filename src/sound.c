
#include "sound.h"
#include <dirent.h>
#include <string.h>

#define PATH_TO_SOUND_MAX_LEN 96

static u8 path_to_sound[64];

//TODO how to index these??? Is it just better to order them in folders then load all files in that folder 
static const SoundStrings sound_string_table[] = {
	{ {"../soundassets/woosh.mp3", "../soundassets/woosh.mp3"}, 2},
	{ {"../soundassets/woosh.mp3", "../soundassets/woosh.mp3", "../soundassets/woosh.mp3"}, 3},
};


void play_sound_multiple(SoundMultiple* sound)
{
	PlaySound(sound->sound[sound->counter ++]);
	if(sound->counter >= MAX_SOUND_MULTIPLE) sound->counter= 0;
}

void play_sound(SoundMultiple* sound)
{
	PlaySound(sound->sound[0]);
}

void sound_load_map_multiple(SoundMultiple* sound_multiple)
{
	for(i32 i = 0; i < MAX_SOUND_MULTIPLE; i++)
	{
		sound_multiple->sound[i] = LoadSoundAlias(sound_multiple->sound[0]);
	}
}

void sound_load_map_sound_pool(MapSound* map_sound, MapSoundIndex index)
{
	const u8* directory_path_name = "../soundassets/";
	DIR* directory = opendir(directory_path_name);

	struct dirent* entry;
	for(i32 i = 0; ( entry = readdir(directory)) != NULL; i++)
	{
		if(entry->d_name[0] == '.') { i--; continue; }

		strcpy(path_to_sound, directory_path_name);
		strcat(path_to_sound, entry->d_name);

		P_LOG("%s\n", path_to_sound);

		map_sound->sound_pool[i] = (SoundMultiple) {
			.sound = LoadSound(path_to_sound), //Load first sound out of the total 8 in the array
			.counter = 0,
		};
		map_sound->len ++;
		sound_load_map_multiple(&map_sound->sound_pool[i]);
	}
	closedir(directory);
}

MapSound* map_sound_new(MapSoundIndex index)
{
	MapSound* map_sound = malloc(sizeof(MapSound));
	SoundStrings strings = sound_string_table[index];

	sound_load_map_sound_pool(map_sound, index);


	return map_sound;
}


