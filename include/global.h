#ifndef GLOBAL_H
#define GLOBAL_H


#include "raylib.h"
#include "types.h"

static inline Vector2 vector2(f32 x, f32 y) { return (Vector2) {x, y}; }
#define P_ERROR(s, ...) { printf("ERROR: "); printf(s, ##__VA_ARGS__); }


#endif
