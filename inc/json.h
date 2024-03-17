/**
 * @file json.h
 *
 * TODO desc
 *
 * All Rights Reserved
 *
 * Author: Kirk Roerig [mr.possoms@gmail.com]
 */

#ifndef __JSON_H__

#ifndef TB_JSON
#define TB_JSON
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

struct json_node;

enum json_value_type
{
	JSON_VAL_INVALID = 0,
	JSON_VAL_NULL,
	JSON_VAL_OBJ,
	JSON_VAL_STR,
	JSON_VAL_FLT,
	JSON_VAL_INT,
	JSON_VAL_BOOL,
};

struct json_value
{
	enum json_value_type type;
	union
	{
		struct json_node* obj;
		const char* str;
		double num;
		bool boolean;
	};
};
typedef struct json_value json_value_t;

#define JSON_FLT(x) ((struct json_value){ .type = JSON_VAL_FLT, .num = (x) })
#define JSON_INT(x) ((struct json_value){ .type = JSON_VAL_INT, .num = (x) })
#define JSON_STR(x) ((struct json_value){ .type = JSON_VAL_STR, .str = (x) })
#define JSON_BOOL(x)                                                           \
	((struct json_value){ .type = JSON_VAL_BOOL, .boolean = (x) })
#define JSON_NULL ((struct json_value){ .type = JSON_VAL_NULL, .obj = NULL })
#define JSON_OBJ(...)                                                          \
	{                                                                      \
		.type = JSON_VAL_OBJ, .obj = &(struct json_node)               \
		{                                                              \
			.pairs = (struct json_pair[]) { __VA_ARGS__ }          \
		}                                                              \
	}
#define JSON(...)                                                              \
	{                                                                      \
		.pairs = (struct json_pair[]) { __VA_ARGS__ }                  \
	}

struct json_pair
{
	const char* key;
	struct json_value value;
};
typedef struct json_pair json_pair_t;

struct json_node
{
	bool heap_allocated;
	struct json_pair* pairs;
};
typedef struct json_node json_t;

struct json_print_desc
{
	int8_t indent_char;
	uint8_t indent_size;
	uint8_t key_value_spacing;
	char* newline;
};
typedef struct json_print_desc json_print_desc_t;

typedef struct
{
	void* (*alloc) (size_t bytes);
	void (*free) (void* ptr);
	void* (*realloc) (void* ptr, size_t new_size);
} json_allocator_t;

typedef struct
{
	json_allocator_t alloc_desc;
} json_parse_ctx_t;

static struct json_node* _parse_json_node (char** json_str,
                                           json_parse_ctx_t* ctx);

static size_t
_repeat_char (int depth, char c, char** buf_out)
{
	if (*buf_out)
	{
		memset (*buf_out, c, depth);
		(*buf_out)[depth] = '\0';
		*buf_out += depth;
	}

	return depth;
}

static ssize_t
_write_str (char** buf_out, char* buf_end, const char* fmtstr, ...)
{
	static char tmp[256];
	char* dst = *buf_out;
	char* dst_end = buf_end;

	if (!dst)
	{
		dst = tmp;
		dst_end = tmp + sizeof (tmp);
	}

	va_list args;
	va_start (args, fmtstr);
	ssize_t size = vsnprintf (dst, dst_end - dst, fmtstr, args);
	va_end (args);

	if (dst != tmp)
	{
		*buf_out = dst + size;

		if (*buf_out >= dst_end)
		{
			return -1;
		}
	}

	return size;
}

static size_t
_json_print (struct json_node* node,
             int depth,
             char** buf_out,
             char* buf_out_end,
             struct json_print_desc* desc)
{
	size_t size = 0;

	if (node == NULL || node->pairs == NULL)
	{
		return size;
	}

	depth += 1;
	for (int i = 0; node->pairs[i].key != NULL; i++)
	{

		size += _repeat_char (
		        desc->indent_size * depth, desc->indent_char, buf_out);
		size += _write_str (
		        buf_out, buf_out_end, "\"%s\":", node->pairs[i].key);
		size += _repeat_char (desc->key_value_spacing, ' ', buf_out);

		switch (node->pairs[i].value.type)
		{
		case JSON_VAL_NULL:
			size += _write_str (
			        buf_out, buf_out_end, "null%s", desc->newline);
			break;
		case JSON_VAL_OBJ:
			size += _write_str (
			        buf_out, buf_out_end, "{%s", desc->newline);
			size += _json_print (node->pairs[i].value.obj,
			                     depth,
			                     buf_out,
			                     buf_out_end,
			                     desc);
			size += _repeat_char (desc->indent_size * depth,
			                      desc->indent_char,
			                      buf_out);
			size += _write_str (buf_out, buf_out_end, "}");
			break;
		case JSON_VAL_STR:
			size += _write_str (buf_out,
			                    buf_out_end,
			                    "\"%s\"",
			                    node->pairs[i].value.str);
			break;
		case JSON_VAL_FLT:
			size += _write_str (buf_out,
			                    buf_out_end,
			                    "%f",
			                    node->pairs[i].value.num);
			break;
		case JSON_VAL_INT:
			size += _write_str (buf_out,
			                    buf_out_end,
			                    "%d",
			                    (int)node->pairs[i].value.num);
			break;
		case JSON_VAL_BOOL:
			size += _write_str (buf_out,
			                    buf_out_end,
			                    "%s",
			                    node->pairs[i].value.boolean
			                            ? "true"
			                            : "false");
			break;
		case JSON_VAL_INVALID:
			size += _write_str (buf_out, buf_out_end, "ERROR");
		}

		size += _write_str (buf_out,
		                    buf_out_end,
		                    "%s%s",
		                    node->pairs[i + 1].key == NULL ? "" : ",",
		                    desc->newline);
	}

	return size;
}

static inline size_t
json_serialize (struct json_node* node,
                char* buf,
                int buffer_size,
                struct json_print_desc* desc)
{
	if (desc == NULL)
	{
		static struct json_print_desc default_desc
		        = { .indent_char = '\t',
			    .indent_size = 1,
			    .newline = "\n",
			    .key_value_spacing = 1 };
		desc = &default_desc;
	}

	assert (desc->newline);

	char* buf_end = buf + buffer_size + 1;
	char** buf_out = &buf;

	size_t size = 0;
	size += _write_str (buf_out, buf_end, "{%s", desc->newline);
	size += _json_print (node, 0, buf_out, buf_end, desc);
	size += _write_str (buf_out, buf_end, "}%s", desc->newline);
	return size;
}

static void
json_free_alloc (json_allocator_t alloc, json_t* json)
{
	if (!json)
		return;
	if (!json->heap_allocated)
		return;

	for (json_pair_t* pair = json->pairs; pair->key; pair++)
	{
		if (pair->value.type == JSON_VAL_OBJ)
		{
			json_free_alloc (alloc, pair->value.obj);
		}
	}

	alloc.free (json->pairs);
	alloc.free (json);
}

static void
json_free (json_t* json)
{
	json_free_alloc (
	        (json_allocator_t){
	                .alloc = malloc,
	                .realloc = realloc,
	                .free = free,
	        },
	        json);
}

static char*
_seek_token (const char* needle, char* haystack, bool to_end)
{
	if (!haystack)
		return NULL;

	const char* n_ptr = needle;
	const char* match_start = NULL;
	for (unsigned i = 0; haystack[i] != '\0'; i++)
	{
		const char* c = haystack + i;

		if (*c == *n_ptr)
		{
			if (!match_start)
			{
				match_start = c;
			}

			n_ptr++;

			if (*n_ptr == '\0')
			{ // reached terminator of the needle
				break;
			}
		}
		else
		{
			match_start = NULL;
			n_ptr = needle;
		}
	}

	return (char*)((to_end ? n_ptr - needle : 0) + match_start);
}

static char*
_eat_whitespace (char* haystack)
{
	while (iswspace (haystack[0]))
		haystack++;
	return haystack;
}

struct json_value
_parse_json_value (char** json_str, json_parse_ctx_t* ctx)
{
	*json_str = _eat_whitespace (*json_str);

	struct json_value out = {};

	switch ((*json_str)[0])
	{
	case '"': // string
	{
		// Find the closing double quote skipping any escaped double
		// quotes in the string
		(*json_str)++; // skip the opening quote
		char* str_begin = *json_str;
		char* str_end = *json_str;
		while (str_end[-1] == '\\' || str_end[0] != '"')
		{
			str_end = _seek_token ("\"", *json_str, false);

			if (!str_end)
			{ // We ran into a null terminator, not the closing
			  // quote return invalid
				return out;
			}
		}

		*json_str = str_end + 1;

		// TODO: either modify in place to null terminate, or change the
		// str type to include an end or len
		out.type = JSON_VAL_STR;
		out.str = str_begin;
	}
	break;
	case '{': // object
		out.type = JSON_VAL_OBJ,
		out.obj = _parse_json_node (json_str, ctx);
		break;
	case 't': // boolean
	case 'f':
		out.boolean = strncmp (*json_str, "true", 4) == 0;

		if (out.boolean == false
		    && strncmp (*json_str, "false", 5) != 0)
		{ // Wasn't false. Return invalid
			return out;
		}

		out.type = JSON_VAL_BOOL;
		*json_str += out.boolean ? 4 : 5;
		break;
	case 'n': // null
		out.type = strncmp (*json_str, "null", 4) == 0
		                   ? JSON_VAL_NULL
		                   : JSON_VAL_INVALID;
	default:
		if (isdigit ((*json_str)[0]) || (*json_str)[0] == '-'
		    || (*json_str)[0] == '+')
		{
			char* num_end = NULL;

			// TODO: handle errors when parsing number
			double d = strtod (*json_str, &num_end);

			*json_str = num_end;

			return (struct json_value){
				.type = JSON_VAL_FLT,
				.num = d,
			};
		}
		break;
	}

	return out;
}

static json_pair_t
_parse_json_pair (char** json_str, json_parse_ctx_t* ctx)
{
	json_pair_t out = {};
	char* key_start = _seek_token ("\"", *json_str, true);
	char* key_end = _seek_token ("\"", key_start, false);

	if (!key_end || !key_start)
	{
		return out;
	}

	*json_str = key_end + 1;
	*json_str = _seek_token (":", *json_str, true);

	if (!json_str)
	{
		return out;
	}

	struct json_value val = _parse_json_value (json_str, ctx);

	if (val.type == JSON_VAL_INVALID)
	{
		return out;
	}

	*json_str = _eat_whitespace (*json_str);

	out.key = key_start;
	out.value = val;

	return out;
}

static struct json_node*
_parse_json_node (char** json_str, json_parse_ctx_t* ctx)
{
	char* obj_start = _seek_token ("{", *json_str, true);

	if (!obj_start)
	{
		return NULL;
	}

	struct json_node* out
	        = ctx->alloc_desc.alloc (sizeof (struct json_node));
	unsigned pair_idx = 0; // just the sentinel for now

	out->heap_allocated = true;
	out->pairs = (json_pair_t*)ctx->alloc_desc.alloc (sizeof (json_pair_t)
	                                                  * (pair_idx + 1));

	while (true) // TODO bounds check
	{
		json_pair_t pair = _parse_json_pair (json_str, ctx);

		if (!pair.key)
		{
			json_free (out);
			out = NULL;
		}

		out->pairs[pair_idx] = pair;

		pair_idx += 1;
		out->pairs = (json_pair_t*)ctx->alloc_desc.realloc (
		        out->pairs, sizeof (json_pair_t) * (pair_idx + 1));
		out->pairs[pair_idx] = (json_pair_t){}; // sentinel

		if ((*json_str)[0] == ',')
		{ // more pairs follow
			*json_str += 1;
		}
		else
		{ // this was the last pair
			break;
		}
	}

	char* obj_end = _seek_token ("}", *json_str, true);

	if (!obj_end)
	{ // destroy node
		json_free_alloc (ctx->alloc_desc, out);
		out = NULL;
	}
	else
	{
		*json_str = obj_end;
	}

	return out;
}

static inline json_t*
json_deserialize_alloc (char* json_str, json_allocator_t alloc_desc)
{
	if (!alloc_desc.alloc)
	{
		alloc_desc.alloc = malloc;
		alloc_desc.realloc = realloc;
		alloc_desc.free = free;
	}

	json_parse_ctx_t ctx = {
		.alloc_desc = alloc_desc,
	};
	return _parse_json_node (&json_str, &ctx);
}

static inline json_t*
json_deserialize (char* json_str)
{
	json_parse_ctx_t ctx = { .alloc_desc = {
		                         .alloc = malloc,
		                         .realloc = realloc,
		                         .free = free,
		                 } };
	return _parse_json_node (&json_str, &ctx);
}

static inline json_value_t*
json_get (const json_t* json, const char* path)
{
	const char* comp_end
	        = (const char*)_seek_token (".", (char*)path, false);
	size_t comp_len = comp_end ? comp_end - path : strlen (path);
	bool target = !comp_end || comp_end[0] != '.';

	for (json_pair_t* pair = json->pairs; pair->key; pair++)
	{
		if (strncmp (pair->key, path, comp_len) == 0)
		{
			if (target)
			{
				return &pair->value;
			}
			else if (pair->value.type == JSON_VAL_OBJ)
			{
				return json_get (pair->value.obj, comp_end + 1);
			}
			else
			{
				break;
			}
		}
	}

	return NULL;
}

#endif

#endif /* __JSON_H__ */
