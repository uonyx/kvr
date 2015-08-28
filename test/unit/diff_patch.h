/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <cxxtest/TestSuite.h>
#include "../src/kvr.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class kvrTestSuiteDiffPatch : public CxxTest::TestSuite
{
  kvr * m_ctx;

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
    const kvr::value *kvr_null = NULL;

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

      TS_ASSERT_DIFFERS (vi, kvr_null);
      TS_ASSERT_DIFFERS (vf, kvr_null);
      TS_ASSERT_DIFFERS (vs, kvr_null);
      TS_ASSERT_DIFFERS (vb, kvr_null);
      TS_ASSERT_DIFFERS (vn, kvr_null);
      TS_ASSERT_EQUALS  (vg, kvr_null);

      TS_ASSERT_EQUALS (vi->get_integer (), 16LL);
      TS_ASSERT_EQUALS (vf->get_float (), 3.14);
      TS_ASSERT_EQUALS (vb->get_boolean (), true);
      TS_ASSERT (strcmp (vs->get_string (), "hello world") == 0);
      TS_ASSERT (vn->is_null ());
    }

    m_ctx->destroy_value (map);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
