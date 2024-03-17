# json.h
![C/C++ CI](https://github.com/mrpossoms/json.h/workflows/C/C++%20CI/badge.svg)

json.h is a C header only library designed to make working with json in C as asthetically pleasing and ergonomic as possible. For example, defining a json structure using json.h could take the following form:

```C
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
```

You can retrieve a value from the structure using `json_get`

```C
json_value_t* baz_boop_prob = json_get (root, "baz.boop.prob");
assert (baz_boop_prob);
assert (baz_boop_prob->type == JSON_VAL_FLT);
assert (baz_boop_prob->num == 0.5);
```

This structure can be serialized to a string like so:

```C
char json_str[1024];

json_serialize (&root, json_str, sizeof(json_str), NULL);
```

Or if you would rather allocate exactly the number of bytes needed (this walks the structure twice):

```C
// count the number of bytes the string needs
size_t json_str_len = json_serialize (&root, NULL, 0, &print_desc);

// allocate and serialize
char* json_str = (char*)alloca (json_str_len); // use allocation method of choice
json_serialize (&root, json_str, json_str_len, &print_desc);
```

A serialized json string can then be parsed back into a `json_t` structure like so:

```C
json_t* json = json_deserialize ((char*)json_str);
```

Be sure to null check `json` as null is returned if parsing fails.

## Caveats

This library offers an incomplete json implementation. It does not (yet) support arrays, and other less common json attributes.

## Requirements

To best utilize this boilerplate please ensure you have installed the following
* Python3+
* Pip
* C/C++ toolchain
* GNU make
* doxygen
* clang-format

## Usage

The Makefile included at the repository root can be used to do almost
everything you could want to do. This includes fetching, installing and
building dependencies, building documentation and of course the software
itself. Helpful make rules include the following.

* `test` - Build and run the test suite
* `format` - Run clang-format explicitly (normally executed as a pre-commit
  hook).
* `docs` - Run Doxygen and build documentation.
* `clean` - Delete all build artifacts.
