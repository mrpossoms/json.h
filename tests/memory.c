#include ".test.h"
#include "json.h"

typedef struct
{
	size_t size;
} ctrl_t;

size_t allocated = 0;

void*
my_alloc (size_t size)
{
	void* ptr = malloc (size + sizeof (ctrl_t));
	ctrl_t* ctrl = (ctrl_t*)ptr;
	ctrl->size = size;
	printf ("malloc: %db + %db\n", allocated, size);
	allocated += size;
	return (void*)(ctrl + 1);
}

void*
my_realloc (void* ptr, size_t size)
{
	ctrl_t* ctrl = &((ctrl_t*)ptr)[-1];
	printf ("realloc: %db + %db\n", allocated, size - ctrl->size);
	allocated -= ctrl->size;
	ctrl = realloc (ctrl, size + sizeof (ctrl_t));
	ctrl->size = size;
	allocated += size;
	return (void*)(ctrl + 1);
}

void
my_free (void* ptr)
{
	ctrl_t* ctrl = &((ctrl_t*)ptr)[-1];
	printf ("free: %db - %db\n", allocated, ctrl->size);
	allocated -= ctrl->size;
	free (ctrl);
}

/**
 * A test is nothing more than a stripped down C program
 * returning 0 is success. Use asserts to check for errors
 */
TEST
{
	const char src[] = "{\"foo\":100,\"bar\":\"Hello "
	                   "world\",\"baz\":{\"bat\":true,\"boop\":{\"prob\":0."
	                   "500000}},\"bap\":null}";

	json_allocator_t alloc = {
		.alloc = my_alloc,
		.realloc = my_realloc,
		.free = my_free,
	};

	json_t* json = json_deserialize_alloc ((char*)src, alloc);
	assert (json);
	printf ("bytes allocated: %zu\n", allocated);
	json_free_alloc (alloc, json);
	assert (allocated == 0);
}
