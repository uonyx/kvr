/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "kvr_msgpack.h"
#include "kvr_internal.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (KVR_LITTLE_ENDIAN)
#define bigendian16(X) kvr_internal::byteswap16 (X)
#define bigendian32(X) kvr_internal::byteswap32 (X)
#define bigendian64(X) kvr_internal::byteswap64 (X)
#else
#define bigendian16(X) (X)
#define bigendian32(X) (X)
#define bigendian64(X) (X)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct msgpack_write_context
{
  msgpack_write_context (kvr::stream *stream) : m_stream (stream)
  {
    KVR_ASSERT (m_stream);
    m_stream->seek (0);
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  bool write_null ()
  {
    put (0xc0);
    return true;
  }

  bool write_array (kvr::sz_t size)
  {
    const uint64_t sz16 = (1 << 16) - 1;
    const uint64_t sz32 = (1ULL << 32) - 1;
    
    if (size <= 15)
    {
      uint8_t sz = (uint8_t) size;
      put ((9u << 4) | sz);
    }
    else if (size < sz16)
    {
      put (0xdc);
      uint16_t sz = bigendian16 (size);
      uint8_t *buf = push (2);
      memcpy (buf, &sz, 2);
    }
    else if (size < sz32)
    {
      put (0xdd);
      uint32_t sz = bigendian32 (size);
      uint8_t *buf = push (4);
      memcpy (buf, &sz, 4);
    }
    else
    {
      return false;
    }

    return true;
  }

  bool write_map (kvr::sz_t size)
  {
    const uint64_t sz16 = (1 << 16) - 1;
    const uint64_t sz32 = (1ULL << 32) - 1;
    
    if (size <= 15)
    {
      uint8_t sz = (uint8_t) size;
      put ((8u << 4) | sz);
    }
    else if (size < sz16)
    {
      put (0xde);
      uint16_t sz = bigendian16 (size);
      uint8_t *buf = push (2);
      memcpy (buf, &sz, 2);
    }
    else if (size < sz32)
    {
      put (0xdf);      
      uint16_t sz = bigendian32 (size);
      uint8_t *buf = push (4);
      memcpy (buf, &sz, 4);
    }

    return true;
  }

  bool write_string (const char *str, kvr::sz_t slen)
  {
    const uint64_t str8 = (1 << 8) - 1;
    const uint64_t str16 = (1 << 16) - 1;
    const uint64_t str32 = (1ULL << 32) - 1;

    if (slen <= 31)
    {
      uint8_t len = (uint8_t) slen;
      put ((5u << 5) | len);
    }
    else if (slen <= str8)
    {
      uint8_t len = (uint8_t) slen;
      put (0xd9);
      put (len);
    }
    else if (slen <= str16)
    {
      put (0xda);
      uint16_t len = bigendian16 (slen);
      uint8_t *buf = push (2);
      memcpy (buf, &len, 2);
    }
    else if (slen <= str32)
    {
      put (0xdb);
      uint32_t len = bigendian32 (slen);
      uint8_t *buf = push (4);
      memcpy (buf, &len, 4);
    }
    else
    {
      return false;
    }

    for (kvr::sz_t i = 0; i < slen; ++i)
    {
      put (str [i]);
    }

    return true;
  }

  bool write_integer (int64_t i64)
  {
    if (i64 < 0) // signed
    {
      const int64_t nint5 =   -((1 << 5) - 1);
      const int64_t nint8 =   -((1 << 7) - 1);
      const int64_t nint16 =  -((1 << 15) - 1);
      const int64_t nint32 =  -((1LL << 31) - 1);

      if (i64 >= nint5)
      {
        uint8_t i = (uint8_t) i64;
        put ((7u << 5) | i);
      }
      else if (i64 > nint8)
      {
        uint8_t i = (uint8_t) i64;
        put (0xd0);
        put (i);
      }
      else if (i64 > nint16)
      {
        put (0xd1);
        uint16_t i = bigendian16 (i64);
        uint8_t *buf = push (2);
        memcpy (buf, &i, 2);
      }
      else if (i64 > nint32)
      {
        put (0xd2);
        uint32_t i = bigendian32 (i64);
        uint8_t *buf = push (4);
        memcpy (buf, &i, 4);
      }
      else // max is int64_t
      {
        put (0xd3);
        uint64_t i = bigendian64 (i64);
        uint8_t *buf = push (8);
        memcpy (buf, &i, 8);
      }
    }
    else  // unsigned
    {
      const uint64_t uint7 = (1 << 7) - 1;
      const uint64_t uint8 = (1 << 8) - 1;
      const uint64_t uint16 = (1 << 16) - 1;
      const uint64_t uint32 = (1ULL << 32) - 1;

      if (i64 <= uint7)
      {
        uint8_t i = (uint8_t) i64;
        put (i);
      }
      else if (i64 <= uint8)
      {
        uint8_t i = (uint8_t) i64;
        put (0xcc);
        put (i);
      }
      else if (i64 <= uint16)
      {
        put (0xcd);
        uint16_t i = bigendian16 (i64);
        uint8_t *buf = push (2);
        memcpy (buf, &i, 2);
      }
      else if (i64 <= uint32)
      {
        put (0xce);
        uint32_t i = bigendian32 (i64);
        uint8_t *buf = push (4);
        memcpy (buf, &i, 4);
      }
      else // max is int64_t;
      {
        put (0xd3); 
        uint64_t i = bigendian64 (i64);
        uint8_t *buf = push (8);
        memcpy (buf, &i, 8);
      }
    }

    return true;
  }

  bool write_float (double f)
  {
    const double fmax = std::numeric_limits<float>::max ();   
    if (f <= fmax)
    {
      put (0xca);      
      union { float f; uint32_t i; } mem;
      mem.f = (float) f;
      uint32_t fi = bigendian32 (mem.i);
      uint8_t *buf = push (4);
      KVR_ASSERT (buf);
      memcpy (buf, &fi, 4);
    }
    else
    {
      put (0xcb);
      union { double f; uint64_t i; } mem;
      mem.f = (double) f;
      uint64_t fi = bigendian64 (mem.i);
      uint8_t *buf = push (8);
      memcpy (buf, &fi, 8);
    }

    return true;
  }

  bool write_boolean (bool b)
  {
    put (b ? 0xc3 : 0xc2);
    return true; 
  }
  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  void put (uint8_t ch)
  {
    if (m_stream->full ())
    {
      m_stream->resize (m_stream->capacity () * 2);
    }

    m_stream->put (ch);
  }

  uint8_t * push (size_t count)
  {
    if (m_stream->full ())
    {
      m_stream->resize (m_stream->capacity () * 2);
    }

    return m_stream->push (count);
  }

  void pop (size_t count)
  {
    m_stream->pop (count);
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  bool write_stream (const kvr::value *src)
  {
    KVR_ASSERT (src);

    bool success = false;

    const kvr::value *val = src;

    //////////////////////////////////
    if (val->is_map ())
    //////////////////////////////////
    {
      kvr::sz_t msz = val->size ();
      bool ok = write_map (msz);
      kvr::value::cursor c = val->fcursor ();
      kvr::pair *p = c.get ();      
      while (p && ok)
      {
        kvr::key *k = p->get_key ();
        ok &= write_string (k->get_string (), k->get_length ());

        kvr::value *v = p->get_value ();
        ok &= write_stream (v);

        p = c.get ();
      }

      success = ok; 
    }

    //////////////////////////////////
    else if (val->is_array ())
    //////////////////////////////////
    {
      kvr::sz_t alen = val->length ();
      bool ok = write_array (alen);
      for (kvr::sz_t i = 0; (i < alen) && ok; ++i)
      {
        kvr::value *v = val->element (i);
        ok &= write_stream (v);
      }
      success = ok;
    }

    //////////////////////////////////
    else if (val->is_string ())
    //////////////////////////////////
    {
      kvr::sz_t slen = 0;
      const char *str = val->get_string (&slen);
      success = write_string (str, slen);
    }

    //////////////////////////////////
    else if (val->is_number_i ())
    //////////////////////////////////
    {
      int64_t n = val->get_number_i ();
      success = write_integer (n);
    }

    //////////////////////////////////
    else if (val->is_number_f ())
    //////////////////////////////////
    {
      double n = val->get_number_f ();
      success = write_float (n);
    }

    //////////////////////////////////
    else if (val->is_boolean ())
    //////////////////////////////////
    {
      bool b = val->get_boolean ();
      success = write_boolean (b);
    }

    //////////////////////////////////
    else if (val->is_null ())
    //////////////////////////////////
    {
      success = write_null ();
    }

    return success;
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  kvr::stream *m_stream;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool kvr_msgpack::read (kvr::value *dest, const uint8_t *data, size_t size)
{
  bool success = false;
  return success;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool kvr_msgpack::write (const kvr::value *src, kvr::stream *str)
{
  bool success = false;

  msgpack_write_context ctx (str);
  success = ctx.write_stream (src);

#if KVR_DEBUG && 1
  kvr::stream hex (512);
  if (kvr_internal::hex_encode (str->bytes (), str->tell (), &hex))
  {
    hex.setEOS ();
    const char *hexstr = (const char *) hex.bytes ();
    fprintf (stderr, "msgpack: %s\n", hexstr);

#if 0
    FILE *fp = NULL;
    fopen_s (&fp, "msgpack_out.txt", "w");
    fwrite (hexstr, 1, hex.tell (), fp);
    fclose (fp);
#endif
  }
#endif

  return success;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

size_t kvr_msgpack::write_approx_size (const kvr::value *val)
{
  KVR_ASSERT (false);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
