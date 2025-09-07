
#include <stdlib.h>
#include "../include/state.h"

GameState* state_new()
{
	GameState* newstate = malloc(sizeof(GameState));

	newstate->entities = dynList_new();
	return newstate;
}
