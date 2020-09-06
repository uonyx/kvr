/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
* Copyright (c) 2015 Ubaka Onyechi
*
* kvr is free software distributed under the MIT license.
* See https://raw.githubusercontent.com/uonyx/kvr/master/LICENSE for details.
*/

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "../src/kvr.h"
#include <stdio.h>
#include "allocators.h"

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// Basic examples of API usage and the allocator interface.

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void example_basics ()
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
  array->push (256);
  array->push (42);
  array->push (-1);
  array->push (-255);

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

  // clean up
  ctx->destroy_value (map);

  // destroy context
  kvr::ctx::destroy (ctx);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void example_basics_allocator ()
{
  // instantiate arena allocator reserving 2kb of stack memory
  arena_allocator<2048> allocator;

  // create a kvr context with the arena allocator
  kvr::ctx *ctx = kvr::ctx::create (&allocator);

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
  array->push (256);
  array->push (42);
  array->push (-1);
  array->push (-255);

  // change street value  
  map->find ("street")->set_string ("pigeon");

  // remove null value
  map->remove ("null");

  // serialize to JSON with the arena allocator
  kvr::obuffer buf (128, &allocator);
  map->encode (kvr::CODEC_JSON, &buf);

  // get memory usage
  printf ("\nMemory used: %zu bytes\n", allocator.get_memory_usage ());

  // clean up
  ctx->destroy_value (map);

  // destroy context
  kvr::ctx::destroy (ctx);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int main ()
{
  example_basics ();

  example_basics_allocator ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
