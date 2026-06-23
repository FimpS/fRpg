#include <stdbool.h>
#include <stdio.h>

#include "fstring.h"

//return true if s1 > s2
static bool string_compare_alpha(String s1, String s2)
{
	const u32 shortest_len = s1.len < s2.len ? s1.len : s2.len;

	for(i32 i = 0; i < shortest_len; i++)
	{
		const u8 c1 = s1.body[i];
		const u8 c2 = s2.body[i];
		if(c1 > c2) return true;
		if(c1 < c2) return false;
	}

	return s1.len < s2.len;
}

static void string_swap(String *s1, String* s2)
{
	String tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

static i32 partition(String* string_array, i32 low, i32 high)
{
	String pivot = string_array[high];
	i32 i = low - 1;

	for (i32 j = low; j < high; ++j)
	{
		if(string_compare_alpha(string_array[j], pivot))
		{
			i ++;
			string_swap(&string_array[i], &string_array[j]);
		}
	}

	string_swap(&string_array[i + 1], &string_array[high]);
	return i + 1;
}

static void string_quick_sort(String* string_array, i32 low, i32 high)
{
	while (low < high)
	{
		i32 pivot = partition(string_array, low, high);

		if (pivot - low < high - pivot)
		{
			string_quick_sort(string_array, low, pivot - 1);
			low = pivot + 1;
		}
		else
		{
			string_quick_sort(string_array, pivot + 1, high);
			high = pivot - 1;
		}
	}
}

void string_sort(String* string_array, const u32 len)
{
	string_quick_sort(string_array, 0, (i32) (len - 1));	
}

StringView string_view_new(const u8* str)
{
	return (StringView) {
		.body = str,
		.len = strlen(str),
	};
}

String string_new(const u8* str)
{
	const u32 str_len = strlen(str);

	String new_str = {
		.body = '\0',
		.len = str_len,
	};
	strcpy(new_str.body, str);
	return new_str;
}
