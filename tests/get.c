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

	json_value_t* baz_bat = json_get (json, "baz.bat");
	assert (baz_bat);
	assert (baz_bat->type == JSON_VAL_BOOL);
	assert (baz_bat->boolean == true);

	json_value_t* baz_boop_prob = json_get (json, "baz.boop.prob");
	assert (baz_boop_prob);
	assert (baz_boop_prob->type == JSON_VAL_FLT);
	assert (baz_boop_prob->num == 0.5);

	json_value_t* baz_zoop_prob = json_get (json, "baz.zoop.prob");
	assert (!baz_zoop_prob);

	json_free (json);
}
