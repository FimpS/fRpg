
#include <stdlib.h>
#include <string.h>

#include "../include/map.h"

Map* map_new()
{
	Map* new_map = malloc(sizeof(Map));
	memset(new_map, 0, sizeof(new_map->content));	
	return new_map;
}
