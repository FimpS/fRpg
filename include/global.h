#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

#include "raylib.h"
#include "types.h"
#include "../lib/v2.h"

#define LOG_MODE 1

#define INF 1000000

static inline Vector2 vector2(f32 x, f32 y) { return (Vector2) {x, y}; }
static inline Vector2 Vector2Midpoint(Vector2 v, Vector2 u) { return (Vector2) {v.x + u.x / 2.0, v.y + u.y / 2.0}; }
static inline V2 Vector2V2(Vector2 v) { return (V2) { (i32) v.x, (i32) v.y}; }
static inline f32 frand(Vector2 bounds) { return bounds.x + ( (f32)rand() / RAND_MAX ) * bounds.y; }
static inline bool AAB(Rectangle r, Vector2 p)
{
	return p.x >= r.x 
		&& p.x <= r.x + r.width
		&& p.y >= r.y 
		&& p.y <= r.y + r.height;
}
static inline Vector2 GetScreenPosition() { return (Vector2) { GetScreenWidth(), GetScreenHeight() }; }
static inline u32 vector2_to_vector_index(u32 x, u32 y, u32 width) { return x + y * width; }
#define P_ERROR(s, ...) { printf("ERROR: "); printf(s, ##__VA_ARGS__); }
#define P_LOG(s, ...) { if(LOG_MODE) { printf("LOG: "); printf(s, ##__VA_ARGS__); } }


#endif
