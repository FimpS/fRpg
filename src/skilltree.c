#include "skilltree.h"
#include "struct.h"
#include "ui.h"
#include "map.h"
#include "enum.h"

/* ----------------------------------- SKILL TREE DATA ----------------------------------- */

const SkillTreeNodeData warlock_skill_tree[] = {
	[SKILL_TYPE_FIREBALL] = {
		.type = SKILL_TYPE_FIREBALL,
		.pos = {350, 200},
		.level = 0,
		.skill_point_cost = 15,
		.neighbors = {
			0,
		},
	},
	[SKILL_TYPE_PLACEHOLDER] = {
		.type = SKILL_TYPE_PLACEHOLDER,
		.pos = {300, 500},
		.level = 0,
		.skill_point_cost = 15,
		.neighbors = {
			SKILL_TYPE_ONE,
			SKILL_TYPE_TWO,
			SKILL_TYPE_THREE,
			//SKILL_TYPE_FOUR,
		},
	},
	[SKILL_TYPE_ONE] = {
		.type = SKILL_TYPE_ONE,
		.pos = {50, 700},
		.level = 0,
		.skill_point_cost = 15,
		.neighbors = {
			0
		},
	},
	[SKILL_TYPE_TWO] = {
		.type = SKILL_TYPE_TWO,
		.pos = {100, 200},
		.level = 0,
		.skill_point_cost = 15,
		.neighbors = {
			SKILL_TYPE_THREE
		},
	},
	[SKILL_TYPE_THREE] = {
		.type = SKILL_TYPE_THREE,
		.pos = {200, 300},
		.level = 0,
		.skill_point_cost = 15,
		.neighbors = {
			0
		},
	},
	[SKILL_TYPE_FOUR] = {
		.type = SKILL_TYPE_FOUR,
		.pos = {350, 500},
		.skill_point_cost = 5,
		.level = 0,
		.neighbors = {
			SKILL_TYPE_FIREBALL,
			0,
		},
	},
};


/* ----------------------------------- SKILL TREE DATA ----------------------------------- */


/* --------------------------------- SKILL TREE FUNCTIONS -------------------------------- */



typedef enum
{
	SKILL_TREE_DOWN,
	SKILL_TREE_UP,
	SKILL_TREE_LEFT,
	SKILL_TREE_RIGHT,
} SkillTreeDirection;

SkillTree* skill_tree_new() //TODO change to specify class
{
	SkillTree* skill_tree = malloc(sizeof(SkillTree));

	(*skill_tree) = (SkillTree) {
		.root = skill_tree_warlock_tree(),
		.active = false,
		.holding_node = NULL,
		.skill_points = 100,
	};

	return skill_tree;
}

SkillTreeNode* skill_tree_init_node(SkillType type)
{
	SkillTreeNode* new = malloc(sizeof(SkillTreeNode));
	(*new) = (SkillTreeNode) {
		.type = type,
		.skill_point_cost = 15,
		.level = 0,
		.pos = { 0 },
		.neighbors = { NULL },
	};

	return new;
}

static const i32 skill_keys[] = {
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_F,
	KEY_G,
	KEY_X,
	KEY_C,
};

#define MAX_VISITED_SKILL_TREE 32
static bool skill_tree_nodes_visited[MAX_VISITED_SKILL_TREE];

static void skill_tree_reset_visited()
{
	const u32 len = MAX_VISITED_SKILL_TREE;
	for(i32 i = 0; i < len; i++)
	{
		skill_tree_nodes_visited[i] = false;
	}
}

static bool skill_tree_prereq_met(SkillTreeNode* node)
{
	const u32 len = MAX_SKILL_TREE_ADJACENT;
	for(i32 i = 0; i < len; i++)
	{
		SkillTreeNode* neighbor = node->neighbors[i];
		if(neighbor != NULL && neighbor->level <= 0) //TODO Could chagne to neighbor->level > node->prereqs
		{
			return false;
		}
	}
	return true;
}

static void skill_tree_draw_straight_arrow(Vector2 end, Vector2 start, Color line_tint, Color outline_tint, bool draw_outline)
{
	const u32 length = 48;
	const f32 thickness = 4.0f;
	const f32 outline_thickness = 6.0f;

	const f32 tri_len = 8.0f;
	const f32 tri_width =4.0f;

	const f32 outline_tri_len = 12.0f;
	const f32 outline_tri_width = 6.0f;

	const f32 arrow_offset = 32.0f;

	const Vector2 start_center = { start.x + length / 2, start.y + length / 2 };
	const Vector2 end_center = { end.x + length / 2, end.y + length / 2 };

	const Vector2 delta = Vector2Subtract(end_center, start_center);

	const f32 start_offset = length / 2;
	const bool straight = fabsf(delta.x) < 1.0f || fabsf(delta.y) < 1.0f;

	const Vector2 direction = Vector2Normalize(Vector2Subtract(end_center, start_center));

	const Vector2 first_direction = straight ? direction : Vector2Normalize(Vector2Subtract(end_center, start_center));

	const Vector2 shortened_start = Vector2Add(start_center, Vector2Scale(first_direction, start_offset));

	const Vector2 perpendicular = { -direction.y, direction.x };

	const Vector2 tip = Vector2Subtract(end_center, Vector2Scale(direction, arrow_offset));

	const Vector2 base = Vector2Subtract(tip, Vector2Scale(direction, tri_len));

	const Vector2 p1 = tip;
	const Vector2 p2 = Vector2Add(base, Vector2Scale(perpendicular, tri_width));
	const Vector2 p3 = Vector2Subtract(base, Vector2Scale(perpendicular, tri_width));

	const f32 outline_tip_offset = 3.0f;

	const Vector2 outline_tip = Vector2Add(tip, Vector2Scale(direction, outline_tip_offset));
	const Vector2 outline_base = Vector2Subtract(outline_tip, Vector2Scale(direction, outline_tri_len));

	const Vector2 op1 = outline_tip;
	const Vector2 op2 = Vector2Add(outline_base, Vector2Scale(perpendicular, outline_tri_width));
	const Vector2 op3 = Vector2Subtract(outline_base, Vector2Scale(perpendicular, outline_tri_width));

	Vector2 line_tip = Vector2Subtract(tip, Vector2Scale(direction, 8.0));
	if(draw_outline)
	{
		DrawLineEx(shortened_start, line_tip, outline_thickness, outline_tint);
		DrawTriangle(op1, op3, op2, outline_tint);
	}
	else
	{
		DrawLineEx(shortened_start, line_tip, thickness, line_tint);
		DrawTriangle(p1, p3, p2, line_tint);
	}

}

static void skill_tree_draw_arrow(Vector2 end, Vector2 start, Color line_tint, Color outline_tint, bool draw_outline)
{
	if(start.x == end.x || start.y == end.y) 
	{
		skill_tree_draw_straight_arrow(end, start, line_tint, outline_tint, draw_outline);
		return;
	}

	const u32 length = 48;
	const f32 thickness = 4.0f;
	const f32 outline_thickness = 6.0f;

	const f32 tri_len = 8.0f;
	const f32 tri_width = 4.0f;

	const f32 outline_tri_len = 12.0f;
	const f32 outline_tri_width = 6.0f;

	const f32 arrow_offset = 32.0f;

	const Vector2 start_center = { start.x + length / 2, start.y + length / 2 };
	const Vector2 end_center = { end.x + length / 2, end.y + length / 2 };

	const Vector2 delta = Vector2Subtract(end_center, start_center);

	const bool vertical = fabsf(delta.x) < 1.0f;
	const bool horizontal = fabsf(delta.y) < 1.0f;

	const Vector2 corner = vertical || horizontal
		? start_center
		: fabsf(delta.x) > fabsf(delta.y)
		? (Vector2) { end_center.x, start_center.y }
	: (Vector2) { start_center.x, end_center.y };

	const f32 corner_extension = 2.0f;
	const f32 corner_outline_extension = 3.0f;

	const Vector2 extended_corner = Vector2Add(corner,
			Vector2Scale(Vector2Normalize(Vector2Subtract(corner, start_center)), corner_extension));
	const Vector2 extended_outline_corner = Vector2Add(corner,
			Vector2Scale(Vector2Normalize(Vector2Subtract(corner, start_center)), corner_outline_extension));



	const f32 start_offset = length / 2;


	const bool straight = fabsf(delta.x) < 1.0f || fabsf(delta.y) < 1.0f;

	const Vector2 direction = Vector2Normalize(Vector2Subtract(end_center, corner));

	const Vector2 first_direction = straight ? direction : Vector2Normalize(Vector2Subtract(corner, start_center));

	const Vector2 shortened_start = Vector2Add(start_center, Vector2Scale(first_direction, start_offset));

	const Vector2 perpendicular = { -direction.y, direction.x };

	const Vector2 tip = Vector2Subtract(end_center, Vector2Scale(direction, arrow_offset));

	const Vector2 base = Vector2Subtract(tip, Vector2Scale(direction, tri_len));

	const Vector2 p1 = tip;
	const Vector2 p2 = Vector2Add(base, Vector2Scale(perpendicular, tri_width));
	const Vector2 p3 = Vector2Subtract(base, Vector2Scale(perpendicular, tri_width));

	const f32 outline_tip_offset = 3.0f;

	const Vector2 outline_tip = Vector2Add(tip, Vector2Scale(direction, outline_tip_offset));
	const Vector2 outline_base = Vector2Subtract(outline_tip, Vector2Scale(direction, outline_tri_len));

	const Vector2 op1 = outline_tip;
	const Vector2 op2 = Vector2Add(outline_base, Vector2Scale(perpendicular, outline_tri_width));
	const Vector2 op3 = Vector2Subtract(outline_base, Vector2Scale(perpendicular, outline_tri_width));

	Vector2 line_tip = Vector2Subtract(tip, Vector2Scale(direction, 8.0));
	if(draw_outline)
	{
		DrawLineEx(shortened_start, extended_outline_corner, outline_thickness, outline_tint);
		DrawLineEx(corner, line_tip, outline_thickness, outline_tint);
		DrawTriangle(op1, op3, op2, outline_tint);
	}
	else
	{
		DrawLineEx(shortened_start, extended_corner, thickness, line_tint);
		DrawLineEx(corner, line_tip, thickness, line_tint);
		DrawTriangle(p1, p3, p2, line_tint);
	}
}

static void skill_tree_node_render(SkillTreeNode* node, GameState* state)
{
	if(node == NULL) return;
	if(skill_tree_nodes_visited[node->type]) return;
	skill_tree_nodes_visited[node->type] = true;	

	Gfx* gfx = state->gfx;
	const Vector2 mouse_pos = GetMousePosition();
	const u32 length = 48;
	const Rectangle src = { node->type * 16, state->player->class * 16, 16, 16 };
	const Rectangle dst = { node->pos.x, node->pos.y, length, length };
	SkillTreeNode* holder = state->player->skill_tree->holding_node;

	Color tint = DARKGRAY;
	SkillTree* tree = state->player->skill_tree;
	if(AAB(dst, mouse_pos))
	{
		tint = GREEN;
		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			if(tree->skill_points >= node->skill_point_cost && skill_tree_prereq_met(node))
			{
				tree->skill_points -= node->skill_point_cost;
				node->level ++;
			}
		}
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && holder == NULL)
		{
			holder = node;
		}
	}
	if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) 
	{
		if(holder != NULL && holder->level > 0)
		{
			const u32 hotbar_len = MAX_HOTBAR_SKILLS;
			for(i32 i = 0; i < hotbar_len; i++)
			{
				const u32 width = 42;
				const Rectangle start_box = state->ui_elements->hotbar.start_location;
				const Rectangle box = { start_box.x + (i * (start_box.width + 4)), start_box.y, start_box.width, start_box.width };
				if(AAB(box, mouse_pos))
				{
					state->ui_elements->hotbar.skills[i] = holder->type;
				}
			}
		}
		holder = NULL;
		state->player->skill_tree->holding_node = holder;
	}



	if(node->level != 0) tint = WHITE;


	const u32 len = MAX_SKILL_TREE_ADJACENT;
	state->player->skill_tree->holding_node = holder;
	for(i32 i = 0; i < len; i++)
	{
		SkillTreeNode* neighbor = node->neighbors[i];
		if(neighbor == NULL) continue;
		Color line_tint = {135, 135, 135, 255};
		Color outline_tint = {200, 200, 200, 255};
		if(neighbor->level > 0) 
		{
			line_tint = (Color) {195, 50, 50, 255};
		}
		skill_tree_draw_arrow(node->pos, neighbor->pos, line_tint, outline_tint, true);

		skill_tree_node_render(neighbor, state);
	}

	ui_draw_element(&gfx->texs[TEXTURE_SKILL_DISPLAY], src, dst, tint);
	if(node->skill_point_cost <= tree->skill_points)
	{
		ui_draw_element(&gfx->texs[TEXTURE_GAME_UI], (Rectangle) { 0, 128, 16, 16 }, dst, (Color) {255,255,255,80});
	}

	Vector2 skill_level_pos = { node->pos.x, node->pos.y };
	Rectangle box_end = { skill_level_pos.x, skill_level_pos.y, length, length / 2.0 };
	Rectangle src_end = { 16, 128, 16, 16 };
	ui_draw_element(&gfx->texs[TEXTURE_GAME_UI], src_end, box_end, WHITE);
	ui_draw_text(TextFormat("%d / 5", node->level), (Vector2) { skill_level_pos.x + length / 2 - 12, skill_level_pos.y }, 16, WHITE, state);


}


static void skill_tree_arrow_line_render(SkillTreeNode* node, GameState* state, bool outline)
{
	if(node == NULL) return;
	if(skill_tree_nodes_visited[node->type]) return;
	skill_tree_nodes_visited[node->type] = true;

	const u32 len = MAX_SKILL_TREE_ADJACENT;
	for(i32 i = 0; i < len; i++)
	{
		SkillTreeNode* neighbor = node->neighbors[i];
		if(neighbor == NULL) continue;

		Color line_tint = {135, 135, 135, 255};
		Color outline_tint = {110, 110, 110, 255};

		if(neighbor->level > 0)
		{
			line_tint = (Color) {195, 50, 50, 255};
			outline_tint = line_tint;
		}


		skill_tree_draw_arrow(node->pos, neighbor->pos, line_tint, outline_tint, outline);
		skill_tree_arrow_line_render(neighbor, state, outline);
	}
}


static void skill_tree_render_nodes(SkillTreeNode* nodes, GameState* state)
{
	const u32 skill_count = sizeof(warlock_skill_tree) / sizeof(warlock_skill_tree[0]);
	for(i32 i = 0; i < skill_count; i++)
	{
		if(skill_tree_nodes_visited[i]) continue;
		if(nodes[i].type == SKILL_TYPE_NONE) continue;

		skill_tree_node_render(&nodes[i], state);
	}
}

static void skill_tree_render_arrows(SkillTreeNode* nodes, GameState* state)
{
	const u32 skill_count = sizeof(warlock_skill_tree) / sizeof(warlock_skill_tree[0]);
	for(i32 i = 0; i < skill_count; i++)
	{
		if(skill_tree_nodes_visited[i]) continue;
		if(nodes[i].type == SKILL_TYPE_NONE) continue;

		skill_tree_arrow_line_render(&nodes[i], state, false);
	}
}

void skill_tree_render(SkillTreeNode* root, GameState* state)
{
	Gfx* gfx = state->gfx;
	skill_tree_reset_visited();
	skill_tree_node_render(root, state);	

	SkillTreeNode* nodes = root - root->type;

	skill_tree_render_nodes(nodes, state);
	skill_tree_reset_visited();
	skill_tree_render_arrows(nodes, state);
	

	SkillTreeNode* holder = state->player->skill_tree->holding_node;
	const Vector2 mouse_pos = GetMousePosition();
	const f32 length = 42.0;
	if(holder != NULL) 
	{
		const Rectangle src = { holder->type * 16, state->player->class * 16, 16, 16 };
		ui_draw_element(&gfx->texs[TEXTURE_SKILL_DISPLAY], src,
				(Rectangle) { mouse_pos.x - length / 4.0, mouse_pos.y - length / 4.0, length, length }, WHITE);
	}
}


SkillTreeNode* skill_tree_warlock_tree()
{
	const u32 skill_count = sizeof(warlock_skill_tree) / sizeof(warlock_skill_tree[0]);
	SkillTreeNode* skill_nodes = malloc(sizeof(SkillTreeNode) * skill_count);
	for(i32 i = 0; i < skill_count; i++)
	{
		SkillTreeNodeData data = warlock_skill_tree[i];
		skill_nodes[i] = (SkillTreeNode) {
			.type = data.type,
			.level = data.level,
			.pos = data.pos,
			.skill_point_cost = data.skill_point_cost,
		};

		for(i32 j = 0; j < MAX_SKILL_TREE_ADJACENT; j++)
		{
			SkillType neighbor_type = data.neighbors[j];
			if(neighbor_type == SKILL_TYPE_NONE)
			{
				skill_nodes[i].neighbors[j] = NULL;
			}
			else
			{
				skill_nodes[i].neighbors[j] = &skill_nodes[neighbor_type];
			}
		}
	}

	return &skill_nodes[SKILL_TYPE_FIREBALL];
}

/* --------------------------------- SKILL TREE FUNCTIONS -------------------------------- */


