
#include "sound.h"
#include "fstring.h"
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

static String sound_get_path_to_sound_file(String file_name, const char* directory_path_name)
{
	String path = string_new(directory_path_name);
	strcat(path.body, file_name.body);
	strcat(path.body, ".mp3");

	return path;
}

static void sound_cut_file_extension(String* str)
{
	string_cut_right(str, 3);
}

static void sound_load_sorted_strings(SoundManager* map_sound, String* path_strings, const u8* directory_path_name)
{
	for(i32 i = 0; i < map_sound->len; i++)
	{
		String full_path = sound_get_path_to_sound_file(
				path_strings[i], 
				directory_path_name);
		//P_LOG("Order: %s\n", full_path.body);
		map_sound->sound_pool[i] = (SoundMultiple) {
			.sound = LoadSound(full_path.body), //Load first sound out of the total 8 in the array
			.counter = 0,

		};
		sound_load_map_multiple(&map_sound->sound_pool[i]);
	}
}

void sound_load_global_sound_pool(SoundManager* map_sound)
{
	const u8* directory_path_name = "../soundassets/";
	DIR* directory = opendir(directory_path_name);
	String* path_strings = malloc(sizeof(String) * MAX_SOUNDS);

	struct dirent* entry;
	for(i32 i = 0; ( entry = readdir(directory)) != NULL; i++)
	{
		if(entry->d_name[0] == '.') { i--; continue; }

		strcpy(path_to_sound, entry->d_name);
		path_strings[i] = string_new(path_to_sound);
		sound_cut_file_extension(&path_strings[i]);
		map_sound->len ++;
	}

	string_sort(path_strings, map_sound->len);
	sound_load_sorted_strings(map_sound, path_strings, directory_path_name);

	free(path_strings);
	closedir(directory);
}

void sound_manager_global_init(SoundManager* sound_manager)
{
	sound_load_global_sound_pool(sound_manager);
}

SoundManager* sound_manager_new()
{
	SoundManager* sound_manager = malloc(sizeof(SoundManager));
	sound_manager->sound_pool = malloc(sizeof(SoundManager) * MAX_SOUNDS);

	return sound_manager;
}

void sound_manager_destroy(SoundManager* sound_manager)
{
	free(sound_manager->sound_pool);
	free(sound_manager);
}

