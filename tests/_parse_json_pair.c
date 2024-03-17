#include ".test.h"
#include "json.h"

TEST
{
	json_parse_ctx_t ctx = { .alloc_desc = {
		                         .alloc = malloc,
		                         .realloc = realloc,
		                         .free = free,
		                 } };

	{ // parse pair with str val
		char haystack[] = "\"foo\": \"bar\"";
		char* str_ptr = haystack;
		json_pair_t pair = _parse_json_pair (&str_ptr, &ctx);
		assert (strncmp (pair.key, "foo", 3) == 0);
		assert (pair.value.type == JSON_VAL_STR);
		assert (strncmp (pair.value.str, "bar", 3) == 0);
	}

	{ // parse pair with num val
		char haystack[] = "\"foo\": 13.37";
		char* str_ptr = haystack;
		json_pair_t pair = _parse_json_pair (&str_ptr, &ctx);
		assert (strncmp (pair.key, "foo", 3) == 0);
		assert (pair.value.type == JSON_VAL_FLT);
		assert (pair.value.num == 13.37);
	}

	{ // parse pair with num val, end on comma
		char haystack[] = "\"foo\": 13.37   ,";
		char* str_ptr = haystack;
		json_pair_t pair = _parse_json_pair (&str_ptr, &ctx);
		assert (strncmp (pair.key, "foo", 3) == 0);
		assert (pair.value.type == JSON_VAL_FLT);
		assert (pair.value.num == 13.37);
		assert (str_ptr[0] == ',');
	}

	{ // parse pair with bool val - true end on comma
		char haystack[] = "\"foo\": true,";
		char* str_ptr = haystack;
		json_pair_t pair = _parse_json_pair (&str_ptr, &ctx);
		assert (strncmp (pair.key, "foo", 3) == 0);
		assert (pair.value.type == JSON_VAL_BOOL);
		assert (pair.value.boolean == true);
		assert (str_ptr[0] == ',');
	}

	{ // parse pair with bool val - false
		char haystack[] = "\"foo\": false";
		char* str_ptr = haystack;
		json_pair_t pair = _parse_json_pair (&str_ptr, &ctx);
		assert (strncmp (pair.key, "foo", 3) == 0);
		assert (pair.value.type == JSON_VAL_BOOL);
		assert (pair.value.boolean == false);
	}

	{ // parse pair invalid value
		char haystack[] = "this aint json";
		char* str_ptr = haystack;
		json_pair_t pair = _parse_json_pair (&str_ptr, &ctx);
		assert (!pair.key);
	}
}
