#include ".test.h"
#include "json.h"

TEST
{
	char* str = "  hello ";

	{ // land on 'h'
		char* str_out = _eat_whitespace (str);
		assert (str_out[0] == 'h');

		// calling again from 'h' returns 'h'
		str_out = _eat_whitespace (str_out);
		assert (str_out[0] == 'h');
	}
}
