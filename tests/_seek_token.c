#include ".test.h"
#include "json.h"

TEST
{
	char* haystack = "foo:\"bar\"";

	{ // Find the colon character
		char* colon = _seek_token(":", haystack, false);
		assert(colon[0] == ':');
	}

	{ // Find the colon character and consume it
		char* double_quote = _seek_token(":", haystack, true);
		assert(double_quote[0] == '\"');
	}

	{ // Fail to find a token
		char* null = _seek_token("?", haystack, true);
		assert(null == NULL);

		char* still_null = _seek_token("?", haystack, false);
		assert(still_null == NULL);
	}
}
