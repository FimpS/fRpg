#ifndef V2_H
#define V2_H

#include <stdbool.h>

#include "types.h"

typedef struct V2f
{
	f32 x;
	f32 y;
} V2f;

typedef struct V2
{
	i32 x;
	i32 y;
} V2;

/* V2 */

/* ESSENTIAL */

V2 v2_new(i32 x, i32 y);
V2 v2f_v2(const V2f p);
f32 v2_len(const V2 p);
V2 v2_sign(const V2 p);
V2 v2_scale(const V2 p, const i32 scalar);
V2 v2_add(const V2 p, const V2 transform);
V2 v2_sub(const V2 p, const V2 transform);
V2 v2_mul(const V2 p, const V2 transform);

/* ESSENTIAL */

/* REC */

V2 v2_mid(const V2 p, const V2 dim);
f32 v2_theta(const V2 self, const V2 other);
f32 v2_range(const V2 self, const V2 other);
f32 v2_inrange(const V2 self, const V2 other, f32 dist);

/* REC */

/* V2 */

/* V2f */

/* ESSENTIAL */

V2f v2f_new(f32 x, f32 y);
V2f v2_v2f(const V2 p);
f32 v2f_len(const V2f p);
V2f v2f_sign(const V2f p);
V2f v2f_scale(const V2f p, const f32 scalar);
V2f v2f_add(const V2f p, const V2f transform);
V2f v2f_sub(const V2f p, const V2f transform);
V2f v2f_mul(const V2f p, const V2f transform);
V2f v2f_norm(const V2f p);

/* ESSENTIAL */

/* REC */

V2f v2f_mid(const V2f p, const V2f dim);
f32 v2f_theta(const V2f self, const V2f other);
f32 v2f_range(const V2f self, const V2f other);
bool v2f_inrange(const V2f self, const V2f other, f32 dist);

/* REC */

/* V2f */

/* IMPLEMENTATION */

#include <math.h>

/* V2 */

V2 v2_new(i32 x, i32 y)
{
	return (V2) {x, y};
}

V2 v2f_v2(const V2f p)
{
	return (V2) { (i32) p.x, (i32) p.y };
}

f32 v2_len(const V2 p)
{
	return sqrt(p.y * p.y + p.x * p.x);
}

V2 v2_sign(const V2 p)
{
	return (V2) {- p.x, - p.y};
}

V2 v2_scale(const V2 p, const i32 scalar)
{
	return (V2) {p.x * scalar, p.y * scalar};
}

V2 v2_add(const V2 p, const V2 transform)
{
	return (V2) {p.x + transform.x, p.y + transform.y};
}

V2 v2_sub(const V2 p, const V2 transform)
{
	return (V2) {p.x - transform.x, p.y - transform.y};
}

V2 v2_mul(const V2 p, const V2 transform)
{
	return (V2) {p.x * transform.x, p.y * transform.y};
}

/* V2 REC */

V2 v2_mid(const V2 p, const V2 dim)
{
	return (V2) {p.x + dim.x / 2, p.y + dim.y / 2};
}

f32 v2_theta(const V2 self, const V2 other)
{
	const V2 diff = v2_sub(self, other);
	return atan2(diff.y, diff.x);
}

f32 v2_range(const V2 self, const V2 other)
{
	return v2_len(v2_sub(self, other));
}

f32 v2_inrange(const V2 self, const V2 other, f32 dist)
{
	return v2_len(v2_sub(self, other)) <= dist;
}

/* V2F */

V2f v2f_new(f32 x, f32 y)
{
	return (V2f) {x, y};
}

V2f v2_v2f(const V2 p)
{
	return (V2f) { (f32) p.x, (f32) p.y };
}

f32 v2f_len(const V2f p)
{
	return sqrt(p.y * p.y + p.x * p.x);
}

V2f v2f_sign(const V2f p)
{
	return (V2f) {-p.x, -p.y};
}

V2f v2f_scale(const V2f p, const f32 scalar)
{
	return (V2f) {p.x * scalar, p.y * scalar};
}

V2f v2f_add(const V2f p, const V2f transform)
{
	return (V2f) {p.x + transform.x, p.y + transform.y};
}

V2f v2f_sub(const V2f p, const V2f transform)
{
	return (V2f) {p.x - transform.x, p.y - transform.y};
}

V2f v2f_mul(const V2f p, const V2f transform)
{
	return (V2f) {p.x * transform.x, p.y * transform.y};
}

V2f v2f_norm(const V2f p)
{
	const u32 inv_len = 1 / v2f_len(p);
	return (V2f) {p.x * inv_len, p.y * inv_len};
}

/* V2f REC */

V2f v2f_mid(const V2f p, const V2f dim)
{
	return (V2f) {p.x + dim.x / 2, p.y + dim.y / 2};
}

f32 v2f_theta(const V2f self, const V2f other)
{
	const V2f diff = v2f_sub(self, other);
	return atan2(diff.y, diff.x);
}

f32 v2f_range(const V2f self, const V2f other)
{
	return v2f_len(v2f_sub(self, other));
}

bool v2f_inrange(const V2f self, const V2f other, f32 dist)
{
	return v2f_len(v2f_sub(self, other)) <= dist;
}


/* IMPLEMENTATION */

#endif
