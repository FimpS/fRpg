
#include "player.h"
#include "entity.h"

Player* player_new()
{
	Player* player = malloc(sizeof(Player));
	player->entity = entity_new(ENTITY_PLAYER, (Vector2) { 10.0, 10.0 } );
	player->inventory = ui_inventory_new(INVENTORY_TYPE_PLAYER);	

	return player;
}

void player_tick(Player* player, GameState* state)
{
	Entity* player_entity = player->entity;

	ui_inventory_tick(player->inventory, state);
	if(player_entity->state.tick != NULL) player_entity->state.tick(player_entity, state);
}

void player_render(Player* player, GameState* state)
{
	entity_render(player->entity, state);
}


void player_destroy(Player* player)
{
	entity_destroy(player->entity);
	free(player->inventory);
	free(player);
}
