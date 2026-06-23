#pragma once

#include <types.h>
#include <string.h>
#define MAX_STRING_LEN 64

typedef struct String
{
	u8 body[MAX_STRING_LEN];
	u32 len;
} String;

typedef struct StringView
{
	const u8* body;
	u32 len;
} StringView;



void string_sort(String* string_array, const u32 len);
StringView string_view_new(const u8* str);
String string_new(const u8* str);
