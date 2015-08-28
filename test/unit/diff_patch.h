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

  void testDiffAndPatch (void)
  {
    //const kvr::value *null_val = NULL;

    kvr::value *val0 = m_ctx->create_value ();

    ///////////////////////////////
    // 1. set up source
    ///////////////////////////////

    val0->insert ("bill", 15LL);
    val0->insert ("jane", 74LL);
    val0->insert ("toby", 21LL);

    kvr::value *m = val0->insert_map ("father");
    kvr::value *a = val0->insert_array ("mother");

    m->insert ("a", "zero");
    m->insert ("b", "one");
    a->push (3.14);
    a->push (5.0);

    kvr::value *am = a->push_map ();

    am->insert ("int", 5LL);
    am->insert ("dbl", 4.5);
    am->insert ("bool", "true");
    am->insert ("str1", "supah");
    am->insert_null ("nowt");
    am->insert ("int3", -5LL);
    am->insert ("dbl3", -4.5);
    am->insert_null ("nonce");
    am->remove ("nonce");
    am->insert ("bool3", false);
    am->insert ("str13", "dupah");

    ///////////////////////////////
    // set up destination
    ///////////////////////////////

    // copy
    kvr::value *val1 = m_ctx->create_value ()->copy (val0);
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    // modify
    val1->find ("bill")->set_integer (9000);
    kvr::value *v1m = val1->find ("father");
    v1m->remove ("a");
    v1m->find ("b")->set_string ("jacob123");
    v1m->insert ("vettel", "f1");
    kvr::value *v1a = val1->find ("mother");
    v1a->pop ();
    v1a->pop ();

    ///////////////////////////////
    // generate diff
    ///////////////////////////////

    kvr::value *diff = m_ctx->create_value ();
    diff->diff (val0, val1);

    ///////////////////////////////
    // apply patch
    ///////////////////////////////

    val0->patch (diff);
    TS_ASSERT_EQUALS (val0->hashcode (), val1->hashcode ());

    // clean up
    m_ctx->destroy_value (val0);
    m_ctx->destroy_value (val1);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
