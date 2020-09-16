kvr: key-value runtime [alpha]
========================================================================

[![Build Status] (https://travis-ci.org/uonyx/kvr.svg)](https://travis-ci.org/uonyx/kvr)

All I wanted was a stand-alone, low-level C++ dictionary with a simple API, and powerful data transformation and serialization functionality. I've ended up with **kvr** - a runtime that provides reference-counted keys and dynamically typed values.

### Features
- Simple API
	* Low-level, extensible, and pluggable
	* No templates, no exceptions, no operator overloading, no modern C++ here
	* Explicit function calls with raw pointers everywhere :o
- Self-contained and cross-platform
	* No dependency on STL
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
- Memory-efficent (or tries to be)
	* Keys (within the same context) are reference-counted
	* Values are 16/32 bytes on 32/64-bit systems (not counting the extra memory required strings, maps, arrays)
	* Support for custom memory allocators like [these...](https://github.com/uonyx/kvr/blob/master/example/allocators.h)
- Custom serialization stream interface
	* File stream? Compression stream? Encryption stream? Yes you can; for [example...](https://github.com/uonyx/kvr/blob/master/example/streams.h)
- Powerful Diff and Patch functionality 
	* kvr's true raison d'�tre

### Compatibiity
C++98 or beyond. So far tested on:
- Visual C++ 2015 on Windows (32/64-bit)
- GCC 4.2.1 and Clang 3.2 on Mac OS X (64-bit)
- GCC 4.6.3 and Clang 3.4 on Linux (64-bit)

### Installation
Simply add the 'src/' directory to your project and include 'kvr.h' to use

### Example Usage
This simple example demonstrates common use cases:

```cpp
#include "kvr.h"

int main ()
{
  // create a kvr context
  kvr::ctx *ctx = kvr::ctx::create ();

  // create a map
  kvr::value *map = ctx->create_value ()->as_map ();

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
```

More [examples](https://github.com/uonyx/kvr/tree/master/example).

### Building Examples and Tests
```sh
$ ./build_tests_examples.sh
$ cd build/
```

### Limitations and Caveats
- Maximum map key length is 65535
- No throw exception guarantee
- No UTF8-validation on strings (until serialization)
- No documenation yet (kvr.h has sparse comments though)

### Alternatives
- [nlohmann::json](https://github.com/nlohmann/json)
- [folly/dynamic](https://github.com/facebook/folly/blob/master/folly/docs/Dynamic.md)
- [Poco::Dynamic](http://pocoproject.org/docs-1.5.0/Poco.Dynamic.html)
- [[GitHub Search]](https://github.com/search?l=C%2B%2B&q=variant&type=Repositories&utf8=%E2%9C%93)
