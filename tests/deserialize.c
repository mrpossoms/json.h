#include ".test.h"
#include "json.h"

/**
 * A test is nothing more than a stripped down C program
 * returning 0 is success. Use asserts to check for errors
 */
TEST
{
	const char src[] = "{\"foo\":100,\"bar\":\"Hello "
	                   "world\",\"baz\":{\"bat\":true,\"boop\":{\"prob\":0."
	                   "500000}},\"bap\":null}";

	json_t* json = json_deserialize ((char*)src);
	assert (json);
	json_free (json);
}
