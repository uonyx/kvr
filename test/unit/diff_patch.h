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

#include <cxxtest/TestSuite.h>
#include "../src/kvr.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class kvrTestSuiteDiffPatch : public CxxTest::TestSuite
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

  void testMap ()
  {
    ///////////////////////////////
    // set up original
    ///////////////////////////////

    kvr::value *map0 = m_ctx->create_value ()->conv_map ();
    map0->insert ("bill", 15);
    map0->insert ("jane", 74);
    map0->insert ("toby", 21);
    kvr::value *m = map0->insert_map ("mapping");
    m->insert ("a", "zero");
    m->insert ("b", "one");
    kvr::value *a = map0->insert_array ("distance");
    a->push_map ()->insert ("yard", 1.1);
    a->push (3.14);
    a->push (5.0);
    kvr::value *am = a->push_map ();
    am->insert ("int", 5);
    am->insert ("dbl", 4.5);
    am->insert_null ("nowt");

    ///////////////////////////////
    // set up modified
    ///////////////////////////////

    kvr::value *map1 = m_ctx->create_value ()->copy (map0);
    TS_ASSERT_EQUALS (map0->hash (), map1->hash ());
    map1->find ("bill")->set_integer (9000LL);
    kvr::value *v1m = map1->find ("mapping");
    v1m->remove ("a");
    v1m->find ("b")->set_string ("thirteen");
    v1m->insert ("c", "sixty-four");
    kvr::value *v1a = map1->find ("distance");
    v1a->element (0)->insert ("yard", 10);
    v1a->pop ();
    v1a->pop ();

    ///////////////////////////////
    // generate diff
    ///////////////////////////////

    kvr::value *diff = m_ctx->create_value ()->diff (map0, map1);

    ///////////////////////////////
    // apply patch
    ///////////////////////////////

    map0->patch (diff);

    ///////////////////////////////
    // verify diff/patch
    ///////////////////////////////

    TS_ASSERT_EQUALS (map0->hash (), map1->hash ());

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (map0);
    m_ctx->destroy_value (map1);
    m_ctx->destroy_value (diff);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testArray ()
  {
    ///////////////////////////////
    // set up original
    ///////////////////////////////

    kvr::value *array0 = m_ctx->create_value ()->conv_array ();
    array0->push (500.0);
    array0->push ("ace");
    array0->push (false);
    array0->push_map ()->insert ("temp", 0);
    array0->push_null ();
    array0->push_array ()->push ("nothing");

    ///////////////////////////////
    // set up modified
    ///////////////////////////////

    kvr::value *array1 = m_ctx->create_value ()->copy (array0);
    TS_ASSERT_EQUALS (array0->hashcode (), array1->hashcode ());
    array1->element (0)->set_float (175.5);
    array1->element (1)->set_string ("algorithm");
    array1->element (2)->set_boolean (true);
    array1->pop ();
    array1->pop ();

    ///////////////////////////////
    // generate diff
    ///////////////////////////////

    kvr::value *diff = m_ctx->create_value ()->diff (array0, array1);

    ///////////////////////////////
    // apply patch
    ///////////////////////////////

    array0->patch (diff);

    ///////////////////////////////
    // verify diff/patch
    ///////////////////////////////

    TS_ASSERT_EQUALS (array0->hashcode (), array1->hashcode ());

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (array0);
    m_ctx->destroy_value (array1);
    m_ctx->destroy_value (diff);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testOtherVariants ()
  {
    ///////////////////////////////
    // set up original
    ///////////////////////////////

    kvr::value *int0 = m_ctx->create_value ()->conv_integer ();
    kvr::value *float0 = m_ctx->create_value ()->conv_float ();
    kvr::value *bool0 = m_ctx->create_value ()->conv_boolean ();
    kvr::value *string0 = m_ctx->create_value ()->conv_string ();

    int0->set_integer (9876543);
    float0->set_float (1234.56);
    bool0->set_boolean (false);
    string0->set_string ("one");

    ///////////////////////////////
    // set up modified
    ///////////////////////////////

    kvr::value *int1 = m_ctx->create_value ()->conv_integer ();
    kvr::value *float1 = m_ctx->create_value ()->conv_float ();
    kvr::value *bool1 = m_ctx->create_value ()->conv_boolean ();
    kvr::value *string1 = m_ctx->create_value ()->conv_string ();

    int1->set_integer (3456789);
    float1->set_float (65.4321);
    bool1->set_boolean (true);
    string1->set_string ("one hundred");

    ///////////////////////////////
    // generate diff
    ///////////////////////////////

    kvr::value *diff0 = m_ctx->create_value ()->diff (int0, int1);
    kvr::value *diff1 = m_ctx->create_value ()->diff (float0, float1);
    kvr::value *diff2 = m_ctx->create_value ()->diff (bool0, bool1);
    kvr::value *diff3 = m_ctx->create_value ()->diff (string0, string1);

    ///////////////////////////////
    // apply patch
    ///////////////////////////////

    int0->patch (diff0);
    float0->patch (diff1);
    bool0->patch (diff2);
    string0->patch (diff3);

    ///////////////////////////////
    // verify diff/patch
    ///////////////////////////////

    TS_ASSERT_EQUALS (int0->hashcode (), int1->hashcode ());
    TS_ASSERT_EQUALS (float0->hashcode (), float1->hashcode ());
    TS_ASSERT_EQUALS (bool0->hashcode (), bool1->hashcode ());
    TS_ASSERT_EQUALS (string0->hashcode (), string1->hashcode ());

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (int0);
    m_ctx->destroy_value (float0);
    m_ctx->destroy_value (bool0);
    m_ctx->destroy_value (string0);

    m_ctx->destroy_value (int1);
    m_ctx->destroy_value (float1);
    m_ctx->destroy_value (bool1);
    m_ctx->destroy_value (string1);

    m_ctx->destroy_value (diff0);
    m_ctx->destroy_value (diff1);
    m_ctx->destroy_value (diff2);
    m_ctx->destroy_value (diff3);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
