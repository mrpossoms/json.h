#include ".test.h"
#include "json.h"

/**
 * A test is nothing more than a stripped down C program
 * returning 0 is success. Use asserts to check for errors
 */
TEST
{
	json_t root = JSON (
	        { "foo", JSON_INT (100) },
	        { "bar", JSON_STR ("Hello world") },
	        { "baz",
	          JSON_OBJ (
	                  { "bat", JSON_BOOL (true) },
	                  { "boop",
	                    JSON_OBJ ({ "prob", JSON_FLT (0.5) }, {}
	                              // each object must end with an empty pair
	                              ) },
	                  {} // each object must end with an empty pair
	                  ) },
	        { "bap", JSON_NULL },
	        {} // each object must end with an empty pair
	);

	json_print_desc_t print_desc = {
		.indent_char = ' ',
		.indent_size = 0,
		.key_value_spacing = 0,
		.newline = "",
	};

	// count the number of bytes the string needs
	size_t json_str_len = json_serialize (&root, NULL, 0, &print_desc);

	// allocate and serialize
	char* json_str = (char*)alloca (json_str_len);
	json_serialize (&root, json_str, json_str_len, &print_desc);
	printf ("%s\n", json_str);

	const char expected[] = "{\"foo\":100,\"bar\":\"Hello "
	                        "world\",\"baz\":{\"bat\":true,\"boop\":{"
	                        "\"prob\":0.500000}},\"bap\":null}";

	assert (strcmp (json_str, expected) == 0);
}
