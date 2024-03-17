#include ".test.h"
#include "json.h"

TEST
{
	json_parse_ctx_t ctx;

	{ // string
		char str[] = " \"bar\" ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		assert (val.type == JSON_VAL_STR);
		assert (val.str[0] == 'b');
		printf ("end: %d\n", str_ptr[0]);
		assert (str_ptr[0] == ' ');
	}

	{ // string - bad
		char str[] = " \"bar ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		assert (val.type == JSON_VAL_INVALID);
	}

	{ // number
		char str[] = " 13.37 ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		printf ("num: %f\n", val.num);
		assert (val.type == JSON_VAL_FLT);
		assert (val.num == 13.37);
		assert (str_ptr[0] == ' ');
	}

	{ // number - bad
		char str[] = " :) ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		// printf("num: %f\n", val.num);
		assert (val.type == JSON_VAL_INVALID);
		// assert(val.num == 13.37);
		// assert(str_ptr[0] == ' ');
	}

	{ // bool - true
		char str[] = " true ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		assert (val.type == JSON_VAL_BOOL);
		assert (val.boolean == true);
		assert (str_ptr[0] == ' ');
	}

	{ // bool - true - bad
		char str[] = " tru ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		assert (val.type == JSON_VAL_INVALID);
	}

	{ // bool - false
		char str[] = " false ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		assert (val.type == JSON_VAL_BOOL);
		assert (val.boolean == false);
		assert (str_ptr[0] == ' ');
	}

	{ // bool - false - bad
		char str[] = " fal ";
		char* str_ptr = &str[0];
		struct json_value val = _parse_json_value (&str_ptr, &ctx);
		assert (val.type == JSON_VAL_INVALID);
	}
}
