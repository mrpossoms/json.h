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
		char json[] = "{\"foo\": \"hello\", \"bar\" : \"world\" , "
		              "\"age\": 42}";
		char* str_ptr = json;
		json_t* node = _parse_json_node (&str_ptr, &ctx);
		assert (node);

		unsigned pair_count = 0;
		bool found_foo = false, found_bar = false, found_age = false;
		for (json_pair_t* pair = node->pairs; pair->key; pair++)
		{
			pair_count++;
			if (strncmp ("foo", pair->key, 3) == 0)
			{
				found_foo = true;
				assert (pair->value.type == JSON_VAL_STR);
				assert (strncmp (pair->value.str, "hello", 5)
				        == 0);
			}
			if (strncmp ("bar", pair->key, 3) == 0)
			{
				found_bar = true;
				assert (pair->value.type == JSON_VAL_STR);
				assert (strncmp (pair->value.str, "world", 5)
				        == 0);
			}
			if (strncmp ("age", pair->key, 3) == 0)
			{
				found_age = true;
				assert (pair->value.type == JSON_VAL_FLT);
				assert (pair->value.num == 42);
			}
		}
		assert (pair_count == 3);
		assert (found_foo && found_bar && found_age);
	}

	{ // parse invalid
		char json[] = "this aint json";
		char* str_ptr = json;
		json_t* node = _parse_json_node (&str_ptr, &ctx);
		assert (!node);
	}

	{ // parse nested object
		char json[] = "{\"foo\": {\"bar\" : \"world\"}, \"age\": 42}";
		char* str_ptr = json;
		json_t* node = _parse_json_node (&str_ptr, &ctx);
		assert (node);

		unsigned pair_count = 0;
		bool found_foo = false, found_age = false;
		for (json_pair_t* pair = node->pairs; pair->key; pair++)
		{
			pair_count++;
			if (strncmp ("foo", pair->key, 3) == 0)
			{
				found_foo = true;
				assert (pair->value.type == JSON_VAL_OBJ);
				assert (pair->value.obj);

				json_t* child = pair->value.obj;
				assert (child->pairs[0].value.type
				        == JSON_VAL_STR);
				assert (strncmp ("bar", child->pairs[0].key, 3)
				        == 0);
				assert (strncmp ("world",
				                 child->pairs[0].value.str,
				                 5)
				        == 0);
			}
			if (strncmp ("age", pair->key, 3) == 0)
			{
				found_age = true;
				assert (pair->value.type == JSON_VAL_FLT);
				assert (pair->value.num == 42);
			}
		}
		assert (pair_count == 2);
		assert (found_foo && found_age);
	}
}
