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

    size_t obufsz = val0->approx_encode_size (kvr::CODEC_JSON);
    kvr::obuffer obuf (obufsz);
    ok = val0->encode (kvr::CODEC_JSON, &obuf);
    TS_ASSERT (ok);
    TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

    ///////////////////////////////
    // decode
    ///////////////////////////////

    kvr::value *val1 = m_ctx->create_value ()->conv_map ();
    ok = val1->decode (kvr::CODEC_JSON, obuf.get_data (), obuf.get_size ());
    TS_ASSERT (ok);

    ///////////////////////////////
    // verify
    ///////////////////////////////

    TS_ASSERT_EQUALS (val0->hash (), val1->hash ());

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

    size_t obufsz = val0->approx_encode_size (kvr::CODEC_CBOR);
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

    TS_ASSERT_EQUALS (val0->hash (), val1->hash ());

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

    size_t obufsz = val0->approx_encode_size (kvr::CODEC_MSGPACK);
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

    TS_ASSERT_EQUALS (val0->hash (), val1->hash ());

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
      size_t obufsz = val->approx_encode_size (kvr::CODEC_JSON);
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
      size_t obufsz = val->approx_encode_size (kvr::CODEC_CBOR);
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
      size_t obufsz = val->approx_encode_size (kvr::CODEC_MSGPACK);
      kvr::obuffer obuf (obufsz);

      bool ok = val->encode (kvr::CODEC_MSGPACK, &obuf);
      TS_ASSERT (ok);
      TS_ASSERT_LESS_THAN_EQUALS (obuf.get_size (), obufsz);

      val_mspk->decode (kvr::CODEC_MSGPACK, obuf.get_data (), obuf.get_size ());
    }

    ///////////////////////////////
    // verify
    ///////////////////////////////

    TS_ASSERT_EQUALS (val_json->hash (), val_cbor->hash ());
    TS_ASSERT_EQUALS (val_cbor->hash (), val_mspk->hash ());

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (val);
    m_ctx->destroy_value (val_json);
    m_ctx->destroy_value (val_cbor);
    m_ctx->destroy_value (val_mspk);
  }

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  void testSampleStream ()
  {
    ///////////////////////////////
    // utility
    ///////////////////////////////

    class hex_ostream : public kvr::ostream
    {
    public:

      hex_ostream (size_t size) : m_os (size) {}
      void put (uint8_t byte) { this->put_hex (byte); }
      void write (uint8_t *bytes, size_t count) { for (size_t i = 0; i < count; ++i) { this->put_hex (bytes [i]); } }
      void flush () { m_os.flush (); }
      const uint8_t *buffer () { return m_os.buffer (); }

    private:

      void put_hex (uint8_t byte)
      {
        static const char lut [] = "0123456789abcdef";
        char hi = lut [(byte >> 4)];
        char lo = lut [(byte & 15)];
        m_os.put (hi);
        m_os.put (lo);
      }

    public:

      static bool encode_buffer (const uint8_t *data, size_t size, kvr::mem_ostream *ostr)
      {
        if (data && (size > 0))
        {
          size_t tcap = (size * 2) + 1;
          ostr->reserve (tcap);

          static const char lut [] = "0123456789abcdef";
          for (size_t i = 0; i < size; ++i)
          {
            uint8_t c = data [i];
            char hi = lut [(c >> 4)];
            char lo = lut [(c & 15)];
            ostr->put (hi);
            ostr->put (lo);
          }
          return true;
        }
        return false;
      }

      kvr::mem_ostream m_os;
    };

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

    ///////////////////////////////
    // encode to mem, then to hex
    ///////////////////////////////

    kvr::mem_ostream mem_ostr (512);
    {
      kvr::obuffer obuf;
      bool ok = val->encode (kvr::CODEC_MSGPACK, &obuf);
      TS_ASSERT (ok);
      ok = hex_ostream::encode_buffer (obuf.get_data (), obuf.get_size (), &mem_ostr);
      TS_ASSERT (ok);
      mem_ostr.flush ();
    }

    ///////////////////////////////
    // encode directly to hex
    ///////////////////////////////

    hex_ostream hex_ostr (8192);
    {
      bool ok = val->encode (kvr::CODEC_MSGPACK, &hex_ostr);
      TS_ASSERT (ok);
      hex_ostr.flush ();
    }

    ///////////////////////////////
    // verify
    ///////////////////////////////

    const char *str0 = (const char *) mem_ostr.buffer ();
    const char *str1 = (const char *) hex_ostr.buffer ();

    TS_ASSERT_EQUALS (strcmp (str0, str1), 0);

    ///////////////////////////////
    // clean up
    ///////////////////////////////

    m_ctx->destroy_value (val);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
