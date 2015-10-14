/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <cxxtest/TestSuite.h>
#include "../src/kvr.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class kvrTestSuiteAPI : public CxxTest::TestSuite
{
  kvr::ctx * m_ctx;

public:

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void setUp ()
  {
    m_ctx = kvr::ctx::create ();
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void tearDown ()
  {
    kvr::ctx::destroy (m_ctx);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testCopy ()
  {
    ///////////////////////////////
    // set up
    ///////////////////////////////

    kvr::value *val0 = m_ctx->create_value ();
    kvr::value *val1 = m_ctx->create_value ();

    ///////////////////////////////
    // boolean
    ///////////////////////////////

#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
    val0->conv_boolean ();
#endif
    val0->set_boolean (true);
    val1->copy (val0);

    TS_ASSERT (val0->is_boolean ());
    TS_ASSERT (val1->is_boolean ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // string
    ///////////////////////////////

#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
    val0->conv_string ();
#endif
    val0->set_string ("test_copy");
    val1->copy (val0);

    TS_ASSERT (val0->is_string ());
    TS_ASSERT (val1->is_string ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // integer
    ///////////////////////////////

#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
    val0->conv_integer ();
#endif
    val0->set_integer (42);
    val1->copy (val0);

    TS_ASSERT (val0->is_integer ());
    TS_ASSERT (val1->is_integer ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // float
    ///////////////////////////////

#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
    val0->conv_float ();
#endif
    val0->set_float (3.142);
    val1->copy (val0);

    TS_ASSERT (val0->is_float ());
    TS_ASSERT (val1->is_float ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // map
    ///////////////////////////////

#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
    val0->conv_map ();
#endif
    val0->insert ("one", 1);
    val0->insert ("pi", 3.14);
    val0->insert ("text", "the quick brown fox jumped over the moon");
    val0->insert_map ("child")->insert ("apple", "seed");
    val1->copy (val0);

    TS_ASSERT (val0->is_map ());
    TS_ASSERT (val1->is_map ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // array
    ///////////////////////////////

#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
    val0->conv_array ();
#endif
    val0->push (1);
    val0->push (3.14);
    val0->push ("the quick brown fox jumped over the moon");
    val0->push_map ()->insert ("apple", "seed");
    val1->copy (val0);

    TS_ASSERT (val0->is_array ());
    TS_ASSERT (val1->is_array ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // null
    ///////////////////////////////

    val0->conv_null ();
    val1->copy (val0);

    TS_ASSERT (val0->is_null ());
    TS_ASSERT (val1->is_null ());
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (val0);
    m_ctx->destroy_value (val1);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testSearch (void)
  {
    ///////////////////////////////
    // set up tree
    ///////////////////////////////

    kvr::value *map = m_ctx->create_value ()->conv_map ();
    {
      map->insert ("name", "elmo");
      map->insert ("address", "1 sesame street");
      map->insert ("age", 19);
      kvr::value *m = map->insert_map ("phrases");
      {
        m->insert ("eat", "chow time!");
        m->insert ("play", "lalalalaa");
        m->insert ("sleep", "zzzzzzzz");
      }
      kvr::value *scores = map->insert_array ("scores");
      {
        scores->push (30.5);
        scores->push (50.0);
        scores->push (100);
        scores->push (900000000LL);
      }
      kvr::value *collection = map->insert_array ("collection");
      {
        kvr::value *c1 = collection->push_map ();
        {
          c1->insert ("int", 0);
          c1->insert ("dbl", 2.7);
          c1->insert ("str", "one");
          c1->insert ("bool", false);
        }
        kvr::value *c2 = collection->push_map ();
        {
          c2->insert ("int", 1);
          c2->insert ("dbl", 4.7);
          c2->insert ("str", "two");
          c2->insert ("bool", true);
        }
      }
    }

    ///////////////////////////////
    // search
    ///////////////////////////////

    {
      const char *path [] = { "phrases", "sleep" };
      kvr::sz_t pathLen = sizeof (path) / sizeof (path [0]);
      kvr::value *sv = map->search (path, pathLen);
      TS_ASSERT (sv);
      const char *sleep = sv->get_string ();
      TS_ASSERT_EQUALS (strcmp (sleep, "zzzzzzzz"), 0);
    }

    {
      const char *path [] = { "scores", "2" };
      kvr::sz_t pathLen = sizeof (path) / sizeof (path [0]);
      kvr::value *sv = map->search (path, pathLen);
      TS_ASSERT (sv);
      int64_t iv = sv->get_integer ();
      TS_ASSERT_EQUALS (iv, 100);
    }

    {
      const char *path = "collection.0";
      kvr::value *sv = map->search (path);
      TS_ASSERT (sv);
      const char *strv = sv->find ("str")->get_string ();
      TS_ASSERT_EQUALS (strcmp (strv, "one"), 0);
    }

    {
      const char *path = "collection.@int=1";
      kvr::value *sv = map->search (path);
      TS_ASSERT (sv);
      double flv = sv->find ("dbl")->get_float ();
      TS_ASSERT_EQUALS (flv, 4.7);
    }

    m_ctx->destroy_value (map);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testMap ()
  {
    const kvr::key    * null_key = NULL;
    const kvr::value  * null_val = NULL;

    kvr::value *map = m_ctx->create_value ()->conv_map ();
    TS_ASSERT (map->is_map ());

    // inserts
    {
      kvr::value *vi = map->insert ("int", 16LL);
      kvr::value *vf = map->insert ("float", 3.14);
      kvr::value *vs = map->insert ("string", "hello world");
      kvr::value *vb = map->insert ("boolean", true);
      kvr::value *vn = map->insert_null ("null");
      kvr::value *va = map->insert_array ("array");
      kvr::value *vm = map->insert_map ("map");

      TS_ASSERT (vi->is_integer ());
      TS_ASSERT (vf->is_float ());
      TS_ASSERT (vs->is_string ());
      TS_ASSERT (vb->is_boolean ());
      TS_ASSERT (vn->is_null ());
      TS_ASSERT (va->is_array ());
      TS_ASSERT (vm->is_map ());
    }

    // find
    {
      kvr::value *vi = map->find ("int");
      kvr::value *vf = map->find ("float");
      kvr::value *vs = map->find ("string");
      kvr::value *vb = map->find ("boolean");
      kvr::value *vn = map->find ("null");
      kvr::value *vg = map->find ("garbage");

      TS_ASSERT_DIFFERS (vi, null_val);
      TS_ASSERT_DIFFERS (vf, null_val);
      TS_ASSERT_DIFFERS (vs, null_val);
      TS_ASSERT_DIFFERS (vb, null_val);
      TS_ASSERT_DIFFERS (vn, null_val);
      TS_ASSERT_EQUALS  (vg, null_val);

      TS_ASSERT_EQUALS (vi->get_integer (), 16LL);
      TS_ASSERT_EQUALS (vf->get_float (), 3.14);
      TS_ASSERT_EQUALS (vb->get_boolean (), true);
      TS_ASSERT (strcmp (vs->get_string (), "hello world") == 0);
      TS_ASSERT (vn->is_null ());
    }

    // cursor
    {
      kvr::sz_t count = 0;
      kvr::value::cursor cur = map->fcursor ();
      kvr::pair p;

      while (cur.get (&p))
      {
        kvr::key *k = p.get_key ();
        kvr::value *v = p.get_value ();

        TS_ASSERT_DIFFERS (k, null_key);
        TS_ASSERT_DIFFERS (v, null_val);

        count++;
      }

      kvr::sz_t size = map->size ();
      TS_ASSERT_EQUALS (size, count);
    }

    m_ctx->destroy_value (map);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
