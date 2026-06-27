#include "npc.h"
#include "ui.h"

#define NPC_INTERACTION_RANGE 3.0


void npc_tick(NPC* self, GameState* state) //TODO change to npc->tick(), when adding different npcs
{
	const Vector2 mouse_cords = map_get_mouse_cords(state->map);
	if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) &&
	   entity_AAB(self->entity, mouse_cords) &&
	   entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE))
	{
		self->inventory->active = true;
		state->player->inventory->active = true;
		P_LOG("Clicked\n");
	}

	if(IsKeyPressed(KEY_F1) || 
       !entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE + 2.0))
	{
		self->inventory->active = false;
		state->player->inventory->active = false;
	}
}

void npcs_tick(DynList* npcs, GameState* state)
{
	const u32 len = npcs->len;

	for(i32 i = 0; i < len; i++)
	{
		NPC* npc = dynList_get(npcs, i);
		if(npc->inventory->active)
		{
			npc->inventory->tick(npc->inventory, state);
		}
		npc_tick(npc, state);
	}
}

void npcs_ui_render(DynList* npcs, GameState* state)
{
	const u32 len = npcs->len;

	for(i32 i = 0; i < len; i++)
	{
		NPC* npc = dynList_get(npcs, i);
		if(npc->inventory->active) 
		{
			ui_inventory_render(npc->inventory, state);
		}
	}

}

void npcs_render(DynList* npcs, GameState* state)
{
	const u32 len = npcs->len;

	for(i32 i = 0; i < len; i++)
	{
		NPC* npc = dynList_get(npcs, i);
		if(npc->inventory->active) 
		{
			ui_inventory_render(npc->inventory, state);
		}
		entity_render(npc->entity, state);
	}
}

const InventoryType npc_to_inventory_table[] = {
	INVENTORY_TYPE_SHOP,
	INVENTORY_TYPE_SHOP,
};

NPC* npc_new(NPCType type, Vector2 pos)
{
	NPC* npc = malloc(sizeof(NPC));
	npc->entity = entity_new(ENTITY_SHOP_NPC, pos);
	npc->inventory = ui_inventory_new(npc_to_inventory_table[type]);


	return npc;
}

void npc_destroy(NPC* npc)
{
	entity_destroy(npc->entity);
	free(npc->inventory);
	free(npc);
}
