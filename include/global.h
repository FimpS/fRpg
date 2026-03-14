#ifndef GLOBAL_H
#define GLOBAL_H


#include "raylib.h"
#include "types.h"

static inline Vector2 vector2(f32 x, f32 y) { return (Vector2) {x, y}; }
static inline Vector2 Vector2Midpoint(Vector2 v, Vector2 u) { return (Vector2) {v.x + u.x / 2.0, v.y + u.y / 2.0}; }
static inline V2 Vector2V2(Vector2 v) { return (V2) { (i32) v.x, (i32) v.y}; }
static inline f32 frand(Vector2 bounds) { return bounds.x + ( (f32)rand() / RAND_MAX ) * bounds.y; }

#define P_ERROR(s, ...) { printf("ERROR: "); printf(s, ##__VA_ARGS__); }


#endif
