kvr: key-value runtime [alpha]
========================================================================

[![Build Status] (https://travis-ci.org/uonyx/kvr.svg)](https://travis-ci.org/uonyx/kvr)

**kvr** is a runtime system that provides dynamically typed values with reference-counted keys. In essence it is a stand-alone low-level C++ dictionary engine with powerful data transformation and serialization features, and most importantly, a simple API. 

It's all I've ever wanted.

### Features
- Simple API
	* No modern C++ here. C-programmer friendly with raw pointers everywhere :O (for people like me who think they know what they're doing)
- Supported value types:
	* Integer (64-bit signed)
	* Floating Point (Double-precision)
	* String (UTF-8)
	* Boolean
	* Map
	* Array
	* Null
- Supported serialization codecs/formats:
	* [JSON](http://json.org/) (using [RapidJSON](https://github.com/miloyip/rapidjson/))
	* [CBOR](http://cbor.io/)
	* [MessagePack](http://msgpack.org/)
- Custom serialization stream interface
	* File stream? Compression stream? Encryption stream? Yes you can; for [example...](https://github.com/uonyx/kvr/blob/master/example/streams.h)
- Powerful Diff and Patch functionality 
	* kvr's true raison d'être
- Memory-efficent (or tries to be)
	* Values are 16/32 bytes on 32/64-bit systems
	* Keys are reference-counted (if created within the same context)
	* Values are dynamically created. (All your heap are belong to us)
- Self-contained and cross-platform
	* Uses some STL (and TR1 for C++03 compilers because unordered_map)

### Compatibiity
Requires C++03 (TR1) or beyond. So far tested on:
- Visual C++ 2015 on Windows (32/64-bit)
- GCC 4.2.1 and Clang 3.2 on Mac OS X (64-bit)
- GCC 4.6.3 and Clang 3.4 on Linux (64-bit)

### Installation
Simply add the 'src/' directory to your project and include 'kvr.h' to use

### Example Usage
This simple example demonstrates common use cases:

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

More [examples](https://github.com/uonyx/kvr/tree/master/example).

### Caveats
- No documenation yet (kvr.h has sparse comments though)
- No exception safety (so please use that 'if' statement)
- No support for custom memory allocators (yet)
- No UTF8-validation on strings (until serialization)

### Alternatives
And guess what I discovered after I got kvr to alpha :/
- [folly/dynamic](https://github.com/facebook/folly/blob/master/folly/docs/Dynamic.md)
	* Sweet API built with modern C++ and throws exceptions
	* Battle-tested
	* JSON-ony serialization support
	* Depends on [boost](http://www.boost.org/) and other folly data structures		
- [poco::Dynamic](http://pocoproject.org/docs-1.5.0/Poco.Dynamic.html)

### License
[MIT](https://raw.githubusercontent.com/uonyx/kvr/master/LICENSE)
