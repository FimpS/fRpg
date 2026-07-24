#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "raylib.h"
#include "types.h"
#include "../lib/v2.h"
#include "enum.h"
#include "struct.h"

#define LOG_MODE 1

#define INF 1000000

static inline Vector2 vector2(f32 x, f32 y) { return (Vector2) {x, y}; }
static inline Vector2 Vector2Midpoint(Vector2 v, Vector2 u) { return (Vector2) {v.x + u.x / 2.0, v.y + u.y / 2.0}; }
static inline V2 Vector2V2(Vector2 v) { return (V2) { (i32) v.x, (i32) v.y}; }
static inline Vector2 V2Vector2(V2 v) { return (Vector2) { (f32) v.x, (f32) v.y}; }
static inline f32 frand(const f32 from, const f32 to) { return from + ( (f32)rand() / RAND_MAX ) * (to - from); }
static inline bool AAB(Rectangle r, Vector2 p)
{
	return p.x >= r.x 
		&& p.x <= r.x + r.width
		&& p.y >= r.y 
		&& p.y <= r.y + r.height;
}
static inline Vector2 GetScreenPosition() { return (Vector2) { GetScreenWidth(), GetScreenHeight() }; }
static inline u32 vector2_to_vector_index(u32 x, u32 y, u32 width) { return x + y * width; }
static inline i32 mini32(const i32 x, const i32 y) { return x < y ? x : y; }
static inline i32 maxi32(const i32 x, const i32 y) { return x > y ? x : y; }
static inline i32 minu32(const u32 x, const u32 y) { return x < y ? x : y; }
static inline i32 maxu32(const u32 x, const u32 y) { return x > y ? x : y; }
static inline i32 minf32(const f32 x, const f32 y) { return x < y ? x : y; }
static inline i32 maxf32(const f32 x, const f32 y) { return x > y ? x : y; }

#define P_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define P_ERROR(s, ...) { printf("ERROR: "); printf(s, ##__VA_ARGS__); }
#define P_LOG(s, ...) do { if(LOG_MODE) { printf("LOG: %-16s%-8d", P_FILENAME, __LINE__); printf(s, ##__VA_ARGS__); } } while(0)


i32 get_tick();
void tick_tick();


