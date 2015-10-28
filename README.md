kvr: key-value runtime 
========================================================================

[![Build Status] (https://travis-ci.org/uonyx/kvr.svg)](https://travis-ci.org/uonyx/kvr)

*Currently somewhere between a dictionary engine and a key-value store*

## Features
- Easy-to-use API
- Variant value types:
	* Integer
	* Floating point (Double-precision), 
	* Strings (UTF-8 encoded)
	* Boolean
	* Map
	* Array
	* Null
- Serialization to/from:
	* [JSON](http://json.org/) (using [rapidJSON](https://github.com/miloyip/rapidjson/))
	* [CBOR](http://cbor.io/)
	* [MessagePack](http://msgpack.org/)
- Custom serialization stream interface
- Diff and Patch functionality

## Compatibiity
C++98: So far tested on
- Visual C++ 2015 on Windows
- GCC 4.2.1 and Clang 3.2 on Mac OS X

## Install
Simply add the src/ directory to your project and include the header "kvr.h" to use

## Sample Usage
This simple example demonstrates common use cases

~~~~~~~~~~cpp
#include "kvr.h"

int main ()
{
  // create a kvr context
  kvr::ctx *ctx = kvr::ctx::create ();

  // create a map
  kvr::value *map = ctx->create_value ()->conv_map ();

  // populate map
  map->insert ("street", "sesame");
  map->insert ("flag", true);
  map->insert ("int", 123);
  map->insert ("pi", 3.1416);
  map->insert_null ("null");
  kvr::value *array = map->insert_array ("array");

  // populate child array
  array->push (-1);
  array->push (42);

  // remove null value
  map->remove ("null");

  // iterate through the map and print keys
  kvr::pair p;
  kvr::value::cursor cur (map);
  while (cur.get (&p))
  {
    kvr::key *key = p.get_key ();
    printf ("%s\n", key->get_string ());
  }

  // change street value  
  map->find ("street")->set_string ("pigeon");

  // read value of 2nd element of map's array  
  int64_t i = map->find ("array")->element (1)->get_integer ();
  if (i == 42)
    printf ("%s\n", "got it");

  // encode to json memory output buffer
  kvr::obuffer obuf;
  map->encode (kvr::CODEC_JSON, &obuf);

  // clean up
  ctx->destroy_value (map);

  // destroy context
  kvr::ctx::destroy (ctx);
}
~~~~~~~~~~

More [examples](https://github.com/uonyx/kvr/tree/master/sample).

## Caveats
Currently no support for custom memory allocators (yet).
