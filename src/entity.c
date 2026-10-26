#include <stdlib.h>
#include <stdio.h>

#include "../include/entity.h"
#include "../include/item.h"
#include "../include/skills.h"
#include "../include/player.h"
#include "../include/skilltree.h"
#include "../include/entity_data.h"
#include "../include/global.h"

/* PRIVATE */

bool entity_path_blocked(Vector2 pos, GameState* state)
{
	Map* map = state->map;
	Tile tile = map_get_tile(state->map, Vector2V2(pos));
	if(tile.solid) return true;
	if(pos.x < 0 || pos.y < 0 || pos.x >= map->dim.x || pos.y >= map->dim.y) return false;

	return false;
}

WalkPath entity_get_path(Entity* self, GameState* state, const Vector2 end)
{
	if(entity_path_blocked(end, state)) return (WalkPath) { 0 };

	if(path_line_of_sight(self, end, state))
	{
		return path_get_line_path(self, end);
	}
	else
	{
		return path_get_any_path(self, end, state);
	}
	return (WalkPath) { 0 };
}

static void entity_pop_swap_from_grid(Entity* self, GameState* state)
{
	Map* map = state->map;
	MapEntityActiveGrid* grid = &map->active_grid;
	Entity* last_entity = dynList_get(grid->entities, dynList_len(grid->entities) - 1);

	last_entity->grid_status.index = self->grid_status.index;
	grid->entities->v[self->grid_status.index] = last_entity;
	grid->entities->len --;
}

void entity_check_active_grid_range(Entity* self, GameState* state)
{
	Map* map = state->map;
	MapEntityActiveGrid* grid = &state->map->active_grid;
	if(!(Vector2Length(Vector2Subtract(self->mid_pos, state->player->entity->pos)) < grid->range) )
	{
		if(self->grid_status.active)
		{
			self->grid_status.active = false;
			entity_pop_swap_from_grid(self, state);	
		}
		return;
	}
	if(self->grid_status.active) return;


	self->grid_status.active = true;
	self->grid_status.index = dynList_len(grid->entities);
	dynList_push(map->active_grid.entities, self);
}
static void entity_check_active_grid(Entity* self, Vector2 new, Vector2 old, GameState* state)
{
	Map* map = state->map;
	MapCamera* cam = state->map->camera;
	MapEntityActiveGrid* grid = &state->map->active_grid;
	bool new_tile = (u32) new.x - (u32) old.x != 0 || (u32) new.y - (u32) old.y != 0;

	if(!new_tile) return;
	//P_LOG("new.x: %f, old.x: %f\n", (f32) new.x, (f32) old.x);
	entity_check_active_grid_range(self, state);

}

static void entity_move_collision(Entity* self, GameState* state)
{

	MapEntityActiveGrid* grid = &state->map->active_grid;
	Vector2 push = { 0.0, 0.0 };

	for(i32 i = 0; i < grid->entities->len; i++)
	{
		Entity* other = dynList_get(grid->entities, i);
		if(other == self) continue;
		Vector2 distpos = Vector2Subtract(self->mid_pos, other->mid_pos);
		f32 dist = Vector2Length(distpos);
		f32 body = other->data.dim.x >= other->data.dim.y ? other->data.dim.x : other->data.dim.y;

		if(dist <= body * 0.75 && dist > 0.01f) push = Vector2Scale(distpos, (body * 0.75 - dist) / dist);

	}
	//P_LOG("%d\n", grid->entities->len);
	self->pos = Vector2Add(self->pos, Vector2Scale(push, 01.1));
}

Entity* entity_get_entity_AAB(GameState* state, const Vector2 pos)
{
	MapEntityActiveGrid* grid = &state->map->active_grid;

	for(i32 i = 0; i < grid->entities->len; i++)
	{
		Entity* entity = dynList_get(grid->entities, i);
		const Rectangle hitbox = { entity->pos.x, entity->pos.y, entity->data.dim.x, entity->data.dim.y };
		if(AAB(hitbox, pos))
		{
			return entity;
		}
	}
	return NULL;
}

Entity* entity_get_entity_AABB(GameState* state, const Rectangle rec)
{
	MapEntityActiveGrid* grid = &state->map->active_grid;

	for(i32 i = 0; i < grid->entities->len; i++)
	{
		Entity* entity = dynList_get(grid->entities, i);
		const Rectangle hitbox = { entity->pos.x, entity->pos.y, entity->data.dim.x, entity->data.dim.y };
		if(AABB(hitbox, rec))
		{
			return entity;
		}
	}
	return NULL;
}

void entity_move_along_path(Entity* self, GameState* state)
{
	Map* map = state->map;
	WalkPath* path = &self->path;

	const Vector2 target = path->pos[path->current];
	Vector2 old_pos = self->pos;
	Vector2 dir = Vector2Subtract(target, self->pos);

	self->speed = entity_calculate_speed(self, state); //Should be done in common update
	if (Vector2Length(dir) > 0.1f && path->count > path->current)
	{
		dir = Vector2Normalize(dir);
		Vector2 new_pos = Vector2Add(self->pos, Vector2Scale(dir, self->speed));
		self->pos = new_pos;
		//entity_check_active_grid(self, new_pos, old_pos, state);
		entity_move_collision(self, state);
	}
	else
	{
		path->current ++;
	}
}

void entity_handle_standard_pathing(Entity* self, GameState* state, bool condition)
{
	Map* map = state->map;
	WalkPath* path = &self->path;
	Vector2 target = path->pos[path->current];

	Vector2 dir = Vector2Subtract(target, self->pos);


	if(condition)
	{
		self->path = entity_get_path(self, state, self->target->pos);
	}

	entity_move_along_path(self, state);


}



Item* entity_get_item_equipment_item(Entity* self, EquipmentSlot type)
{
	return &self->equipment->items[type];
}

ItemEquipment* entity_get_item_equipment(Entity* self, EquipmentSlot type)
{
	return &entity_get_item_equipment_item(self, type)->data.equipment;
}

static f32 entity_get_agility(Entity* self)
{
	
	const f32 entity_agility = self->stats.utility.agility;
	const f32 boots_agility = entity_get_item_equipment(self, ITEM_SLOT_LEGS)->utility.agility;
	const f32 ring1_agility = entity_get_item_equipment(self, ITEM_SLOT_RING1)->utility.agility;
	const f32 ring2_agility = entity_get_item_equipment(self, ITEM_SLOT_RING2)->utility.agility;
	const f32 ring3_agility = entity_get_item_equipment(self, ITEM_SLOT_RING3)->utility.agility;
	const f32 ring4_agility = entity_get_item_equipment(self, ITEM_SLOT_RING4)->utility.agility;
	const f32 agility_sum = entity_agility +
							boots_agility +
							ring1_agility +
							ring2_agility +
							ring3_agility +
							ring4_agility;

	return agility_sum / 1000.0;
}

f32 entity_calculate_speed(Entity* self, GameState* state)
{
	f32 speed = self->data.base_speed;
	f32 agility = entity_get_agility(self);
	if(IsKeyDown(KEY_SPACE)) return self->data.base_speed * 1.5;

	f32 speed_multiplier = maxf32(0.0, 1.0 + self->speed_multiplier);
	return (self->data.base_speed + agility) * (1.0 + self->speed_multiplier);
}

/*--------------------------------- Damage ---------------------------------*/

static u32 entity_elemental_damage_calculation(u32 atk, u32 def, u32 dmg, u32 mul)
{
	return ( ( (f32) dmg ) + ( ( (f32) atk) * ( (f32) (mul) / 100.0 )) ) * (1.0 - (f32) def / 100.0); //bad
}

static const Color damage_color_table[] = {
	WHITE,
	BLUE,
	ORANGE,
	YELLOW,
	GOLD,
	BLACK,
};
static Color entity_determine_damage_tint(u32 phys, u32 fire, u32 magic, u32 bolt, u32 holy, u32 dark)
{
	u32 highest = phys;
	u32 index = 0;
	if(magic >= highest) { highest = magic; index = 1; }
	if(fire >= highest) { highest = fire; index = 2; }
	if(bolt >= highest) { highest = bolt; index = 3; }
	if(holy >= highest) { highest = holy; index = 4; }
	if(dark >= highest) { highest = dark; index = 5; }

	return damage_color_table[index];
}


void entity_heal(Entity* target, const u32 amount, GameState* state)
{
	target->stats.health += amount;
}


// Returns false if not enough mana
bool entity_consume_mana(Entity* self, const u32 cost, GameState* state)
{
	if(self->stats.mana < cost) return false;

	self->stats.mana -= cost;
	state->ui_elements->stats.mana_decay_timer = 0;

	return true;
}

void entity_do_damage(Entity* attacker, Entity* defender, const ElementalPower damage, const ElementalPower multipler, GameState* state)
{
	if(defender->stats.health <= 0) return;
	const ElementalPower def = defender->stats.defense;
	const ElementalPower atk = attacker->stats.attack;
	const ElementalPower dmg = damage;
	const ElementalPower mul = multipler;
	const u32 total_phys = entity_elemental_damage_calculation(atk.physical, def.physical, dmg.physical, mul.physical);
	const u32 total_fire = entity_elemental_damage_calculation(atk.fire, def.fire, dmg.fire, mul.fire);
	const u32 total_magic = entity_elemental_damage_calculation(atk.magic, def.magic, dmg.magic, mul.magic);
	const u32 total_bolt = entity_elemental_damage_calculation(atk.bolt, def.bolt, dmg.bolt, mul.bolt);
	const u32 total_holy = entity_elemental_damage_calculation(atk.holy, def.holy, dmg.holy, mul.holy);
	const u32 total_dark = entity_elemental_damage_calculation(atk.dark, def.dark, dmg.dark, mul.dark);

	const u32 total_damage = total_phys + total_fire + total_magic + total_bolt + total_holy + total_dark;
	Color tint = entity_determine_damage_tint(total_phys, total_fire, total_magic, total_bolt, total_holy, total_dark);
	state->ui_elements->stats.health_decay_timer = 0;
	ui_text_damage_add(total_damage, defender->pos, defender->data.dim, tint, state);
	defender->health_display_timer = 0;
	defender->stats.health -= total_damage;

}

/*--------------------------------- Damage ---------------------------------*/



//Speed multiplier for states which multiplied with self->speed.base?
//
//Better do maybe have a few fields that are multipliers which are addative
//Say you have speed_mult = 1.0; then you just add to it speed_mult += amazing_gear.speed;
//Then Just self->speed *= speed_mult; So never multiply something temporary
//Perhaps only affect base_speed with lvls and keep a constant if you want to revert
void entity_player_determine_movement_direction(Entity* self, GameState* state)
{
	f32 speed = entity_calculate_speed(self, state);
	Vector2 dir = {
		0.0,
		0.0
	};

	if(IsKeyDown(KEY_D)) { dir.x += 1.0; };
	if(IsKeyDown(KEY_A)) { dir.x -= 1.0; };
	if(IsKeyDown(KEY_S)) { dir.y += 1.0; };
	if(IsKeyDown(KEY_W)) { dir.y -= 1.0; };
	dir = Vector2Normalize(dir);

	if(dir.x != 0.0 || dir.y != 0.0) self->speed = speed; else self->speed = 0.0;

	self->facing_angle = atan2(dir.y, dir.x);
}

Vector2 entity_get_midpoint(Entity* self)
{
	return Vector2Midpoint(self->pos, self->data.dim);
}

bool entity_in_range(Vector2 p, Vector2 u, f32 range)
{
	const f32 dist = Vector2Distance(p, u);
	return dist <= range;
}


#define NEW_LINE() printf("\n");

f32 entity_check_tile_bounds_horizontal(Vector2 new_pos, const Vector2 old_pos, Entity* self, GameState* state)
{
	Map* map = state->map;
	const i32 first_tile = (i32) (new_pos.y);
	const i32 last_tile = (i32) (new_pos.y + self->data.dim.y - 0.001f);
	const i32 width = new_pos.x > old_pos.x ? (i32) (new_pos.x + self->data.dim.x) : new_pos.x;
	bool collided = false;
	for (i32 i = first_tile; i <= last_tile; i++)
	{
		Tile tile = map_get_tile(map, (V2) { width, i } );

		if (tile.solid)
		{
			collided = true; break;
		}
	}

	if (collided)
	{
		return new_pos.x > old_pos.x ? width - self->data.dim.x : width + 1.0;
	}
	return new_pos.x;

}

f32 entity_check_tile_bounds_vertical(Vector2 new_pos, const Vector2 old_pos, Entity* self, GameState* state)
{
	Map* map = state->map;
	const i32 first_tile = (i32) (new_pos.x);
	const i32 last_tile = (i32) (new_pos.x + self->data.dim.x - 0.001f);
	const i32 height = new_pos.y > old_pos.y ? (i32) (new_pos.y + self->data.dim.y) : new_pos.y;
	bool collided = false;
	for (i32 i = first_tile; i <= last_tile; i++)
	{
		Tile tile = map_get_tile(map, (V2) { i, height } );

		if (tile.solid)
		{
			collided = true; break;
		}
	}

	if (collided)
	{
		return new_pos.y > old_pos.y ? height - self->data.dim.y : height + 1.0;
	}
	return new_pos.y;

}

void entity_move(Entity* self, GameState* state)
{
	Map* map = state->map;

	const f32 angle = self->facing_angle;
	const Vector2 old_pos = self->pos;
	const const const const Vector2 velocity = Vector2Scale( (Vector2) { cos(angle), sin(angle) }, self->speed);

	const Vector2 candidate_horizontal = Vector2Add(old_pos, (Vector2) { velocity.x, 0.0 } );
	const Vector2 candidate_vertical = Vector2Add(old_pos, (Vector2) { 0.0, velocity.y } );

	Vector2 new_pos = { 
		new_pos.x = entity_check_tile_bounds_horizontal(candidate_horizontal, old_pos, self, state),
		new_pos.y = entity_check_tile_bounds_vertical(candidate_vertical, old_pos, self, state)
	};

	entity_check_active_grid(self, new_pos, old_pos, state);
	self->pos = new_pos;
	entity_move_collision(self, state);

}

Entity* entity_player_init(GameState* state)
{
	Entity* player = entity_new(ENTITY_PLAYER, (Vector2) {14.0, 14.0} );
	player->path = (WalkPath) {0};
	return player;
}

/* PRIVATE */

/* PUBLIC */

bool entity_AAB(Entity* e, Vector2 p)
{
	return p.x >= e->pos.x 
		&& p.x <= e->pos.x + e->data.dim.x 
		&& p.y >= e->pos.y
		&& p.y <= e->pos.y + e->data.dim.y;
}

Entity* entity_get_vector(GameState* state, Vector2 pos)
{
	DynList* entities = state->map->entities;
	const u32 len = dynList_len(entities);
	for(i32 i = 0; i < len; i++)
	{
		Entity* e = dynList_get(entities, i);
		//if(Vector2V2(e->pos) == Vector2Equals
	}
}

void entity_handle_target(Entity* self)
{
	Entity* target = self->target;
	if(target != NULL)
	{
		if(target->state.type == ESTYPE_CLEAR) self->target = NULL;
	}
}

void entity_level_up(Entity* self, GameState* state)
{
	const u32 diff = self->stats.experience - experience_level_table[self->stats.level];
	self->stats.experience = 0;
	self->stats.level ++;
	entity_gain_experience(self, diff, state);
	state->player->skill_tree->skill_points += 25;
}

void entity_gain_experience(Entity* self, const u32 experience, GameState* state)
{
	self->stats.experience += experience;
	if(self->stats.experience >= experience_level_table[self->stats.level])
	{
		entity_level_up(self, state);
	}
}

static void entity_die(Entity* self, GameState* state)
{
	quest_register_entry(state->player, QUEST_OBJECTIVE_KILL, (QuestObjectiveData) {
			.kill.kill_type = self->type,
			} );
	entity_gain_experience(state->player->entity, 7, state);
}

#define HEALTH_DISPLAY_LIMIT 300
void entity_pre_tick(Entity* self, GameState* state)
{
	self->mid_pos = Vector2Midpoint(self->pos, self->data.dim);
	self->state.timer ++;

	skills_tick_all(self, state);
	if(get_tick() % 300 == 0)
	{
		entity_check_active_grid_range(self, state);
	}

	if(self->data.targetable && self->stats.health <= 0)
	{
		self->state.type = ESTYPE_DEAD;
	}

	if(self->state.type == ESTYPE_DEAD)
	{
		entity_die(self, state);
		self->state.type = ESTYPE_CLEAR;
	}

	entity_handle_target(self);	
}

void entities_tick(DynList* entities, GameState* state)
{

	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		if(e->state.type == ESTYPE_CLEAR) 
		{
			dynList_pop_swap(entities, i --);
			entity_destroy(e);
			continue;
		}
		if(IsKeyPressed(KEY_O))
		{
			e->state.type = ESTYPE_DEAD;
			continue;
			//P_LOG("Killed all entities, in map->entities\n");
		}

		entity_pre_tick(e, state);
		if( e->state.tick != NULL ) e->state.tick(e, state);
	}
}

Rectangle entity_get_render_frame(Entity* self, GameState* state)
{
	EntityAnimation* animation = &self->state.animation;
	if(animation->stop_timer == 0) return animation->frames[0];
	const u32 frame_index = ( (animation->timer ++ ) / animation->stop_timer ) % animation->amount_frames;
	return animation->frames[ frame_index ];
}

static void entity_healthbar_render(Entity* self, GameState* state)
{
	if(self->health_display_timer ++ >= HEALTH_DISPLAY_LIMIT) return;
	if(self->stats.health < 0) return;
	Gfx* gfx = state->gfx;
	MapCamera* cam = state->map->camera;
	Rectangle src = {0,0,16,16};
	Color health_tint = RED;
	Color outline_tint = GRAY;
	Vector2 box_dim = { 100, 16 };
	Vector2 box_dim_health = { box_dim.x * ( (f32) self->stats.health / self->data.base_health), 16 };
	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			src, 
			(Rectangle) {
			(self->pos.x - cam->offset.x) * cam->tile_len    - (box_dim.x / 2.0 - (self->data.dim.x * TILE_LEN) / 2.0), 
			(self->pos.y - cam->offset.y) * cam->tile_len    - 25, 
			box_dim.x, box_dim.y
			}, 
			(Vector2) {0}, 
			0.0, 
			outline_tint);
	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			src, 
			(Rectangle) {
			(self->pos.x - cam->offset.x) * cam->tile_len    - (box_dim.x / 2.0 - (self->data.dim.x * TILE_LEN) / 2.0), 
			(self->pos.y - cam->offset.y) * cam->tile_len    - 25, 
			box_dim_health.x, box_dim.y
			}, 
			(Vector2) {0}, 
			0.0, 
			health_tint);
}

static void entity_name_render(Entity* self, GameState* state)
{
	MapCamera* cam = state->map->camera;
	const u8* name = entity_get_name(self->data.type);
	const u8* pool = TextFormat("%s [lvl:%d]", name, self->stats.level);
	const Vector2 pos1 = ui_center_text(map_convert_map_to_screen(self->mid_pos, state->map),
			pool,
			20.0,
			state);
	const Vector2 pos = Vector2Subtract(pos1, (Vector2) { 0.0, 60.0 });	

	ui_draw_text(pool, pos, 20.0, WHITE, state);
	//ui_draw_element(&state->gfx->texs[TEXTURE_GAME_UI], (Rectangle) {0,0,16,16}, (Rectangle) {self->mid_pos.x, self->mid_pos.y, 1, 1}, RED);
}

void entity_render(Entity* self, GameState* state)
{
	MapCamera* cam = state->map->camera;
	const u32 rgb_values = 255 * 1;
	const u32 rgb = 255;
	Color color = { rgb_values, rgb_values, rgb_values, rgb};

	Rectangle src_frame = entity_get_render_frame(self, state);

	DrawTexturePro(state->gfx->texs[self->data.spritesheet_index], 
			src_frame, 
			(Rectangle) {
			(self->pos.x - cam->offset.x) * cam->tile_len, 
			(self->pos.y - cam->offset.y) * cam->tile_len, 
			self->data.dim.x * cam->tile_len, 
			self->data.dim.y * cam->tile_len
			}, 
			(Vector2) {0}, 
			0.0, 
			color);
}
static void  entity_ui_render(DynList* entities, GameState* state)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		entity_healthbar_render(e, state);
		entity_name_render(e, state);
	}
}
void entities_render(DynList* entities, GameState* state)
{
	for(i32 i = 0; i < dynList_len(entities); i++)
	{
		Entity* e = dynList_get(entities, i);
		entity_render(e, state);
	}
	entity_ui_render(entities, state);
}

const u8* entity_get_name(EntityType type)
{
	return entity_data_table[type].name;
}


Entity* entity_new_editor(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));

	newe->data = entity_data_table[type];
	newe->type = type;

	newe->pos = pos;
	newe->mid_pos = Vector2Midpoint(pos, newe->data.dim);
	newe->state = entity_state_table[newe->data.start_state];

	newe->speed = 0.0;
	//newe->light = (EntityLight) { 0 };
	newe->path = (WalkPath) { 0 };
	newe->target = NULL;

	// IDK Yet
	newe->aggro_range = 0.0;
	newe->facing_angle = 0.0;
	newe->id = 0;

	return newe;

}

Entity* entity_new(EntityType type, Vector2 pos)
{
	Entity* newe = malloc(sizeof(Entity));
	*newe = (Entity) { 0 };

	newe->data = entity_data_table[type];
	newe->type = type;

	newe->pos = pos;
	newe->mid_pos = Vector2Midpoint(pos, newe->data.dim);
	newe->state = entity_state_table[newe->data.start_state];

	newe->health_display_timer = 301;
	newe->speed = 0.0;
	newe->light_frequency = rand() % 40;
	newe->path = (WalkPath) { 0 };
	newe->target = NULL;

	// IDK Yet
	newe->aggro_range = 0.0;
	newe->facing_angle = 0.0;
	newe->id = 0;


	newe->skill_channels = (EntitySkillChannels) { 0 };

	newe->stats = (EntityStats) {
		.level = 1,
		.experience = 0,
		.defense = 0,
		.attack = { 100, 30, 0, 0, 0, 0 },
		.utility = { 10 },
		.max_health = newe->data.base_health,
		.health = newe->data.base_health,
		.max_mana = 1000,
		.mana = 832,
	};

	newe->equipment = entity_equipment_new();

	return newe;
}

void entity_destroy(Entity* e)
{
	free(e);
	free(e->equipment);
}

/* PUBLIC */


