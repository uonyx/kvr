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
  kvr::context * m_ctx;

public:

  void setUp ()
  {
    m_ctx = kvr::create_context ();
  }

  void tearDown ()
  {
    kvr::destroy_context (m_ctx);
  }

  void testMap (void)
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
