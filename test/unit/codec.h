/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <cxxtest/TestSuite.h>
#include "../src/kvr.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class kvrTestSuiteCodec : public CxxTest::TestSuite
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

  void tearDown ()
  {
    kvr::ctx::destroy (m_ctx);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testJSON ()
  {
    ///////////////////////////////
    // set up
    ///////////////////////////////

    bool ok = false;
    const char json [] = "{\"sesame\":\"street\",\"t\":true,\"f\":false,\"n\":null,\"i\":123,\"pi\":3.1416,\"a\":[1,256,4096,-1,-255,-256,-4095]}";

    kvr::value *val0 = m_ctx->create_value ()->conv_map ();
    ok = val0->decode (kvr::CODEC_JSON, (const uint8_t *) json, sizeof (json));
    TS_ASSERT (ok);

    ///////////////////////////////
    // encode
    ///////////////////////////////

    size_t obufsz = val0->calculate_encode_size (kvr::CODEC_JSON);
    kvr::obuffer obuf (obufsz);
    ok = val0->encode (kvr::CODEC_JSON, &obuf);
    TS_ASSERT (ok);
    TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

    if (strcmp (json, (const char *) obuf.get_data ()) != 0)
    {
      TS_WARN ("json decode string mismatch");
    }

    ///////////////////////////////
    // decode
    ///////////////////////////////

    kvr::value *val1 = m_ctx->create_value ()->conv_map ();
    ok = val1->decode (kvr::CODEC_JSON, obuf.get_data (), obuf.get_size ());
    TS_ASSERT (ok);

    ///////////////////////////////
    // verify
    ///////////////////////////////

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

  void testCBOR ()
  {
    ///////////////////////////////
    // set up
    ///////////////////////////////

    bool ok = false;

    kvr::value *val0 = m_ctx->create_value ()->conv_map ();
    {
      val0->insert ("sesame", "street");
      val0->insert ("t", true);
      val0->insert ("f", false);
      val0->insert_null ("n");
      val0->insert ("i", 123);
      val0->insert ("pi", 3.1416);
      kvr::value *a = val0->insert_array ("a");
      {
        a->push (1);
        a->push (256);
        a->push (4096);
        a->push (-1);
        a->push (-255);
        a->push (-256);
        a->push (-4095);
      }
    }

    ///////////////////////////////
    // encode
    ///////////////////////////////

    size_t obufsz = val0->calculate_encode_size (kvr::CODEC_CBOR);
    kvr::obuffer obuf (obufsz);

    ok = val0->encode (kvr::CODEC_CBOR, &obuf);
    TS_ASSERT (ok);
    TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

    ///////////////////////////////
    // decode
    ///////////////////////////////

    kvr::value *val1 = m_ctx->create_value ();
    ok = val1->decode (kvr::CODEC_CBOR, obuf.get_data (), obuf.get_size ());
    TS_ASSERT (ok);

    ///////////////////////////////
    // verify
    ///////////////////////////////

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

  void testMsgPack ()
  {
    ///////////////////////////////
    // set up
    ///////////////////////////////

    bool ok = false;

    kvr::value *val0 = m_ctx->create_value ()->conv_map ();
    {
      val0->insert ("sesame", "street");
      val0->insert ("t", true);
      val0->insert ("f", false);
      val0->insert_null ("n");
      val0->insert ("i", 123);
      val0->insert ("pi", 3.1416);
      kvr::value *a = val0->insert_array ("a");
      {
        a->push (1);
        a->push (256);
        a->push (4096);
        a->push (-1);
        a->push (-255);
        a->push (-256);
        a->push (-4095);
      }
    }

    ///////////////////////////////
    // encode
    ///////////////////////////////

    size_t obufsz = val0->calculate_encode_size (kvr::CODEC_MSGPACK);
    kvr::obuffer obuf (obufsz);

    ok = val0->encode (kvr::CODEC_MSGPACK, &obuf);
    TS_ASSERT (ok);
    TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

    ///////////////////////////////
    // decode
    ///////////////////////////////

    kvr::value *val1 = m_ctx->create_value ();
    ok = val1->decode (kvr::CODEC_MSGPACK, obuf.get_data (), obuf.get_size ());
    TS_ASSERT (ok);

    ///////////////////////////////
    // verify
    ///////////////////////////////

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

  void testInterchange ()
  {
    ///////////////////////////////
    // set up
    ///////////////////////////////

    kvr::value *val = m_ctx->create_value ()->conv_map ();
    {
      val->insert ("sesame", "street");
      val->insert ("t", true);
      val->insert ("f", false);
      val->insert_null ("n");
      val->insert ("i", 123);
      val->insert ("pi", 3.1416);
      kvr::value *a = val->insert_array ("a");
      {
        a->push (1);
        a->push (256);
        a->push (4096);
        a->push (-1);
        a->push (-255);
        a->push (-256);
        a->push (-4095);
      }
    }

    kvr::value *val_json = m_ctx->create_value ();
    kvr::value *val_cbor = m_ctx->create_value ();
    kvr::value *val_mspk = m_ctx->create_value ();

    ///////////////////////////////
    // json encode/decode
    ///////////////////////////////
    {
      size_t obufsz = val->calculate_encode_size (kvr::CODEC_JSON);
      kvr::obuffer obuf (obufsz);

      bool ok = val->encode (kvr::CODEC_JSON, &obuf);
      TS_ASSERT (ok);
      TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

      val_json->decode (kvr::CODEC_JSON, obuf.get_data (), obuf.get_size ());
    }

    ///////////////////////////////
    // cbor encode/decode
    ///////////////////////////////
    {
      size_t obufsz = val->calculate_encode_size (kvr::CODEC_CBOR);
      kvr::obuffer obuf (obufsz);

      bool ok = val->encode (kvr::CODEC_CBOR, &obuf);
      TS_ASSERT (ok);
      TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

      val_cbor->decode (kvr::CODEC_CBOR, obuf.get_data (), obuf.get_size ());
    }

    ///////////////////////////////
    // msgpack encode/decode
    ///////////////////////////////
    {
      size_t obufsz = val->calculate_encode_size (kvr::CODEC_MSGPACK);
      kvr::obuffer obuf (obufsz);

      bool ok = val->encode (kvr::CODEC_MSGPACK, &obuf);
      TS_ASSERT (ok);
      TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

      val_mspk->decode (kvr::CODEC_MSGPACK, obuf.get_data (), obuf.get_size ());
    }

    ///////////////////////////////
    // verify
    ///////////////////////////////

    TS_ASSERT_EQUALS (val_json->hashcode (), val_cbor->hashcode ());
    TS_ASSERT_EQUALS (val_cbor->hashcode (), val_mspk->hashcode ());

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (val);
    m_ctx->destroy_value (val_json);
    m_ctx->destroy_value (val_cbor);
    m_ctx->destroy_value (val_mspk);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
