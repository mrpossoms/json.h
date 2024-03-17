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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <wctype.h>

struct json_node;

enum json_value_type {
	JSON_VAL_NULL = 0,
	JSON_VAL_OBJ,
	JSON_VAL_STR,
	JSON_VAL_FLT,
	JSON_VAL_INT,
	JSON_VAL_BOOL,
};

struct json_value {
	enum json_value_type type;
	union {
		struct json_node* obj;
		const char* str;
		double num;
		int boolean;
	};
};

#define JSON_FLT(x) ((struct json_value){.type=JSON_VAL_FLT, .num=(x)})
#define JSON_INT(x) ((struct json_value){.type=JSON_VAL_INT, .num=(x)})
#define JSON_STR(x) ((struct json_value){.type=JSON_VAL_STR, .str=(x)})
#define JSON_BOOL(x) ((struct json_value){.type=JSON_VAL_BOOL, .boolean=(x)})
#define JSON_NULL ((struct json_value){.type=JSON_VAL_NULL, .obj=NULL})
#define JSON_OBJ(...) {.type=JSON_VAL_OBJ, .obj=&(struct json_node){(struct json_pair[]){__VA_ARGS__}}}
#define JSON(...) {(struct json_pair[]){__VA_ARGS__}}

struct json_pair {
	const char* key;
	struct json_value value;
};

struct json_node{
	struct json_pair* pairs;
};
typedef struct json_node json_t;

struct json_print_desc {
	int8_t indent_char;
	uint8_t indent_size;
	uint8_t key_value_spacing;
	char* newline;
};
typedef struct json_print_desc json_print_desc_t;

typedef struct {

} json_parse_ctx_t;


static struct json_node* _json_parse_node(char** json_str, json_parse_ctx_t* ctx);

static size_t _repeat_char(int depth, char c, char** buf_out)
{
	if (*buf_out)
	{
		memset(*buf_out, c, depth);
		(*buf_out)[depth] = '\0';
		*buf_out += depth;
	}

	return depth;
}

static ssize_t _write_str(char** buf_out, char* buf_end, const char* fmtstr, ...)
{
	static char tmp[256];
	char* dst = *buf_out;
	char* dst_end = buf_end;

	if (!dst)
	{
		dst = tmp;
		dst_end = tmp + sizeof(tmp);
	}

	va_list args;
	va_start(args, fmtstr);
	ssize_t size = vsnprintf(dst, dst_end - dst, fmtstr, args);
	va_end(args);

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

static size_t _json_print(struct json_node* node, int depth, char** buf_out, char* buf_out_end, struct json_print_desc* desc)
{
	size_t size = 0;

	if (node == NULL)
	{
		return size;
	}

	depth += 1;
	for (int i = 0; node->pairs[i].key != NULL; i++) 
	{

		size += _repeat_char(desc->indent_size * depth, desc->indent_char, buf_out);
		size += _write_str(buf_out, buf_out_end, "\"%s\":", node->pairs[i].key);
		size += _repeat_char(desc->key_value_spacing, ' ', buf_out);
		
		switch (node->pairs[i].value.type) {
			case JSON_VAL_NULL:
				size += _write_str(buf_out, buf_out_end, "null%s", desc->newline);
				break;
			case JSON_VAL_OBJ:
				size += _write_str(buf_out, buf_out_end, "{%s", desc->newline);
				size += _json_print(node->pairs[i].value.obj, depth, buf_out, buf_out_end, desc);
				size += _repeat_char(desc->indent_size * depth, desc->indent_char, buf_out); 
				size += _write_str(buf_out, buf_out_end, "}");
				break;
			case JSON_VAL_STR:
				size += _write_str(buf_out, buf_out_end, "\"%s\"", node->pairs[i].value.str);
				break;
			case JSON_VAL_FLT:
				size += _write_str(buf_out, buf_out_end, "%f", node->pairs[i].value.num);
				break;
			case JSON_VAL_INT:
				size += _write_str(buf_out, buf_out_end, "%d", (int)node->pairs[i].value.num);
				break;
			case JSON_VAL_BOOL:
				size += _write_str(buf_out, buf_out_end, "%s", node->pairs[i].value.boolean ? "true" : "false");
				break;
		}

		size += _write_str(buf_out, buf_out_end, "%s%s", node->pairs[i + 1].key == NULL ? "" : ",", desc->newline);
	}

	return size;
}

static size_t json_serialize(struct json_node* node, char* buf, int buffer_size, struct json_print_desc* desc)
{
	if (desc == NULL) {
		static struct json_print_desc default_desc = {
			.indent_char='\t',
			.indent_size=1,
			.newline="\n",
			.key_value_spacing=1
		};
		desc = &default_desc;
	}

	assert(desc->newline);

	char* buf_end = buf + buffer_size + 1;
	char** buf_out = &buf;

	size_t size = 0;
	size += _write_str(buf_out, buf_end, "{%s", desc->newline);
	size += _json_print(node, 0, buf_out, buf_end, desc);
	size += _write_str(buf_out, buf_end, "}%s", desc->newline);
	return size;
}

static char* _seek_token(const char* needle, char* haystack, bool to_end)
{
	const char* n_ptr = needle;
	const char* match_start = NULL;
	for (unsigned i = 0; haystack[i] != '\0'; i++)
	{
		const char* c = haystack + i;

		if (*c == *n_ptr)
		{
			if (!match_start) { match_start = c; }

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

static char* _eat_whitespace(char* haystack)
{
	while(iswspace(haystack[0])) haystack++;
	return haystack;
}

struct json_value _parse_json_value(char** json_str, json_parse_ctx_t* ctx)
{
	*json_str = _eat_whitespace(*json_str);

	switch((*json_str)[0])
	{
	case '"': // string
		{
			// Find the closing double quote skipping any escaped double quotes
			// in the string
			(*json_str)++; // skip the opening quote
			char* str_begin = *json_str;
			char* str_end = *json_str;
			while(str_end[-1] == '\\' || str_end[0] != '"')
			{
				str_end = _seek_token("\"", *json_str, false);
			}

			*json_str = str_end + 1;

			return (struct json_value){
				.type = JSON_VAL_STR,
				.str = str_begin,
			};
		}
		break;
	case '{': // object
		return (struct json_value){
			.type = JSON_VAL_OBJ,
			.obj = _json_parse_node(json_str, ctx),
		};
		break;
	default:
		if (isnumber((*json_str)[0]) || (*json_str)[0] == '-' || (*json_str)[0] == '+')
		{
			char* num_end = NULL;
			
			// TODO: handle errors when parsing number
			double d = strtod(*json_str, &num_end);

			return (struct json_value) {
				.type = JSON_VAL_FLT,
				.num = d,
			};

			*json_str = num_end + 1;
		}
		break;
	}

	return (struct json_value){

	};
}

static struct json_pair* _parse_json_pair(char** json_str, json_parse_ctx_t* ctx)
{
	char* key_start = _seek_token("\"", *json_str, true);
	char* key_end = _seek_token("\"", *json_str, false);

	if (!key_end || !key_start) { return NULL; }

	*json_str = key_end + 1;

	char* val_start = _seek_token(":", *json_str, true);

}



static struct json_node* _json_parse_node(char** json_str, json_parse_ctx_t* ctx)
{

}

static json_t json_deserialize(char* json_str)
{
	json_parse_ctx_t ctx = {};
	json_t out;
	// _json_parse_node

	return out;
}

#endif

#endif /* __JSON_H__ */
