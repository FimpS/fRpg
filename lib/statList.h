#pragma once

#include <stddef.h>
#include <types.h>


#define DEFINE_STATIC_LIST(T, Name, Prefix, CAP)                     \
    typedef struct {                                                 \
        T data[CAP];                                                 \
        u64 len;                                                     \
    } Name;                                                          \
																	 \
	static inline Name Prefix##_init() {							 \
		return (Name) { 											 \
			.data = { 0 },											 \
			.len = 0,												 \
		};															 \
	}																 \
																	 \
    static inline u32 Prefix##_len(const Name *l) {                  \
        return l->len;                                               \
    }                                                                \
                                                                     \
    static inline u32 Prefix##_capacity(const Name *l) {             \
        (void)l;                                                     \
        return (CAP);                                                \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_empty(const Name *l) {	             \
        return l->len == 0;                                          \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_is_full(const Name *l) {              \
        return l->len >= (CAP);                                      \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_push(Name *l, T value) {              \
        if (l->len >= (CAP)) return 0;                               \
        l->data[l->len ++] = value;                                  \
        return 1;                                                    \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_pop(Name *l, T *out) {                \
        if (l->len == 0) return 0;                                   \
        l->len--;                                                    \
        if (out) *out = l->data[l->len];                             \
        return 1;                                                    \
    }                                                                \
                                                                     \
    static inline T *Prefix##_get(Name *l, u32 index) {              \
        if (index >= l->len) return NULL;                            \
        return &l->data[index];                                      \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_set(Name *l, u32 index, T value) {    \
        if (index >= l->len) return 0;                               \
        l->data[index] = value;                                      \
        return 1;                                                    \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_add(Name *l, u32 index, T value) {    \
        if (index > l->len || l->len >= (CAP)) return 0;             \
        for (u32 i = l->len; i > index; i--) {                       \
            l->data[i] = l->data[i - 1];                             \
        }                                                            \
        l->data[index] = value;                                      \
        l->len++;                                                    \
        return 1;                                                    \
    }                                                                \
                                                                     \
    static inline i32 Prefix##_del(Name *l, u32 index) {          	 \
        if (index >= l->len) return 0;                               \
        for (u32 i = index; i + 1 < l->len; i++) {                   \
            l->data[i] = l->data[i + 1];                             \
        }                                                            \
        l->len --;                                                   \
        return 1;                                                    \
    }                                                                \
                                                                     \
    static inline void Prefix##_clear(Name *l) {                     \
        l->len = 0;                                                  \
    }

