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
// msgpack_read_context
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct msgpack_read_context
{
  msgpack_read_context (kvr::value *value) : m_root (value), m_pair (NULL), m_depth (0)
  {
    memset (m_stack, 0, sizeof (m_stack));
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  bool read_null ()
  {
    bool success = false;

    KVR_ASSERT_SAFE (m_depth != 0, success);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT (node);
    KVR_ASSERT (node->is_map () || node->is_array ());

    if (node->is_map ())
    {
      KVR_ASSERT_SAFE (m_pair, false);
      kvr::value *pv = m_pair->get_value ();
      KVR_ASSERT_SAFE (pv && pv->is_null (), false);
      m_pair = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      node->push_null ();
      success = true;
    }

    return success;
  }

  bool read_boolean (bool b)
  {
    bool success = false;

    KVR_ASSERT_SAFE (m_depth != 0, success);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT (node);
    KVR_ASSERT (node->is_map () || node->is_array ());

    if (node->is_map ())
    {
      KVR_ASSERT_SAFE (m_pair, false);
      kvr::value *pv = m_pair->get_value ();
      KVR_ASSERT_SAFE (pv && pv->is_null (), false);
#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
      pv->conv_boolean ();
#endif
      pv->set_boolean (b);
      m_pair = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      node->push (b);
      success = true;
    }

    return success;
  }

  bool read_integer (int64_t i)
  {
    bool success = false;

    KVR_ASSERT_SAFE (m_depth != 0, success);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT (node);
    KVR_ASSERT (node->is_map () || node->is_array ());

    if (node->is_map ())
    {
      KVR_ASSERT_SAFE (m_pair, false);
      kvr::value *pv = m_pair->get_value ();
      KVR_ASSERT_SAFE (pv && pv->is_null (), false);
#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
      pv->conv_number_i ();
#endif
      pv->set_number_i (i);
      m_pair = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      node->push (i);
      success = true;
    }

    return success;
  }

  bool _read_uint64 (uint64_t u)
  {
    KVR_ASSERT (m_depth != 0);
    KVR_ASSERT (false && "not supported");
    return false;
  }

  bool read_float (double d)
  {
    bool success = false;

    KVR_ASSERT_SAFE (m_depth != 0, success);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT (node);
    KVR_ASSERT (node->is_map () || node->is_array ());

    if (node->is_map ())
    {
      KVR_ASSERT_SAFE (m_pair, false);
      kvr::value *pv = m_pair->get_value ();
      KVR_ASSERT_SAFE (pv && pv->is_null (), false);
#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
      pv->conv_number_f ();
#endif
      pv->set_number_f (d);
      m_pair = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      node->push (d);
      success = true;
    }

    return success;
  }

  bool read_string (const char *str, size_t length, bool copy)
  {
    bool success = false;

    KVR_ASSERT_SAFE (m_depth != 0, success);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT (node);
    KVR_ASSERT (node->is_map () || node->is_array ());

    if (node->is_map ())
    {
      KVR_ASSERT_SAFE (m_pair, false);
      kvr::value *pv = m_pair->get_value ();
      KVR_ASSERT_SAFE (pv && pv->is_null (), false);
      pv->conv_string ();
      pv->set_string (str, (kvr::sz_t) length);
      m_pair = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      kvr::value *vstr = node->push_null (); KVR_ASSERT (vstr);
      vstr->conv_string ();
      vstr->set_string (str, (kvr::sz_t) length);
      success = true;
    }

    return success;
  }

  bool read_object_start ()
  {
    bool success = false;
    kvr::value *node = NULL;

    if (m_depth > 0)
    {
      node = m_stack [m_depth - 1];
      KVR_ASSERT (node);
      KVR_ASSERT (node->is_map () || node->is_array ());

      if (node->is_map ())
      {
        KVR_ASSERT_SAFE (m_pair, false);
        node = m_pair->get_value ();
        KVR_ASSERT_SAFE (node && node->is_null (), false);
        node->conv_map ();
        m_pair = NULL;
        success = true;
      }
      else if (node->is_array ())
      {
        node = node->push_map ();
        success = true;
      }
    }
    else
    {
      node = m_root->conv_map ();
      success = true;
    }

    KVR_ASSERT (m_depth < KVR_CONSTANT_MAX_TREE_DEPTH);
    m_stack [m_depth++] = node;

    return success;
  }

  bool read_key (const char *str, size_t length, bool copy)
  {
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT_SAFE (node && node->is_map (), false);

    KVR_ASSERT (!m_pair);
    m_pair = node->insert_null (str);
    return (m_pair != NULL);
  }

  bool read_object_end (size_t memberCount)
  {
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT_SAFE (node && node->is_map (), false);
    KVR_ASSERT (node->size () == (kvr::sz_t) memberCount);
    m_stack [--m_depth] = NULL;
    return true;
  }

  bool read_array_start ()
  {
    bool success = false;
    kvr::value *node = NULL;

    if (m_depth > 0)
    {
      node = m_stack [m_depth - 1];
      KVR_ASSERT (node);
      KVR_ASSERT (node->is_map () || node->is_array ());

      if (node->is_map ())
      {
        KVR_ASSERT_SAFE (m_pair, false);
        node = m_pair->get_value ();
        KVR_ASSERT_SAFE (node && node->is_null (), false);
        node->conv_array ();
        m_pair = NULL;
        success = true;
      }
      else if (node->is_array ())
      {
        node = node->push_array ();
        success = true;
      }
    }
    else
    {
      node = m_root->conv_array ();
      success = true;
    }

    KVR_ASSERT (m_depth < KVR_CONSTANT_MAX_TREE_DEPTH);
    m_stack [m_depth++] = node;

    return success;
  }

  bool read_array_end (size_t elementCount)
  {
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT_SAFE (node && node->is_array (), false);
    KVR_ASSERT (node->length () == (kvr::sz_t) elementCount);
    m_stack [--m_depth] = NULL;
    return true;
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  kvr::value  * m_stack [KVR_CONSTANT_MAX_TREE_DEPTH];
  kvr::value  * m_root;
  kvr::pair   * m_pair;
  kvr::sz_t     m_depth;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// msgpack_write_context
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct msgpack_write_context
{
  msgpack_write_context (const kvr::value *val, kvr::ostream *ostream) : m_root (val), m_stream (ostream)
  {
    KVR_ASSERT (m_root);
    KVR_ASSERT (m_stream);
    m_stream->seek (0);
#if KVR_DEBUG
    m_stream->set_eos (0);
#endif
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
    if (size <= 15)
    {
      uint8_t sz = (uint8_t) size;
      put ((9u << 4) | sz);
    }
    else if (size < 0xffff)
    {
      put (0xdc);
      uint16_t sz = bigendian16 (size);
      put ((uint8_t *) &sz, 2);
    }
    else if (size < 0xffffffff)
    {
      put (0xdd);
      uint32_t sz = bigendian32 (size);
      put ((uint8_t *) &sz, 4);
    }
    else
    {
      return false;
    }

    return true;
  }

  bool write_map (kvr::sz_t size)
  {    
    if (size <= 15)
    {
      uint8_t sz = (uint8_t) size;
      put ((8u << 4) | sz);
    }
    else if (size < 0xffff)
    {
      put (0xde);
      uint16_t sz = bigendian16 (size);
      put ((uint8_t *) &sz, 2);
    }
    else if (size < 0xffffffff)
    {
      put (0xdf);      
      uint16_t sz = bigendian32 (size);
      put ((uint8_t *) &sz, 4);
    }

    return true;
  }

  bool write_string (const char *str, kvr::sz_t slen)
  {
    if (slen <= 31)
    {
      uint8_t len = (uint8_t) slen;
      put ((5u << 5) | len);
    }
    else if (slen <= 0xff)
    {
      uint8_t len = (uint8_t) slen;
      put (0xd9);
      put (len);
    }
    else if (slen <= 0xffff)
    {
      put (0xda);
      uint16_t len = bigendian16 (slen);
      put ((uint8_t *) &len, 2);
    }
    else if (slen <= 0xffffffff)
    {
      put (0xdb);
      uint32_t len = bigendian32 (slen);
      put ((uint8_t *) &len, 4);
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
    if (i64 > 0) // unsigned
    {
      if (i64 <= 127)
      {
        uint8_t i = (uint8_t) i64;
        put (i);
      }
      else if (i64 <= 0xff)
      {
        uint8_t i = (uint8_t) i64;
        put (0xcc);
        put (i);
      }
      else if (i64 <= 0xffff)
      {
        put (0xcd);
        uint16_t i = bigendian16 (i64);
        put ((uint8_t *) &i, 2);
      }
      else if (i64 <= 0xffffffff)
      {
        put (0xce);
        uint32_t i = bigendian32 (i64);
        put ((uint8_t *) &i, 4);
      }
      else // max is int64_t;
      {
        put (0xd3);
        uint64_t i = bigendian64 (i64);
        put ((uint8_t *) &i, 8);
      }
    }
    else // signed
    {
      const int64_t nint5 = -((1 << 5) - 1);
      const int64_t nint8 = -((1 << 7) - 1);
      const int64_t nint16 = -((1 << 15) - 1);
      const int64_t nint32 = -((1LL << 31) - 1);

      if (i64 >= nint5)
      {
        uint8_t i = (uint8_t) i64;
        put ((7u << 5) | i);
      }
      else if (i64 >= nint8)
      {
        uint8_t i = (uint8_t) i64;
        put (0xd0);
        put (i);
      }
      else if (i64 >= nint16)
      {
        put (0xd1);
        uint16_t i = bigendian16 (i64);
        put ((uint8_t *) &i, 2);
      }
      else if (i64 >= nint32)
      {
        put (0xd2);
        uint32_t i = bigendian32 (i64);
        put ((uint8_t *) i, 4);
      }
      else
      {
        put (0xd3);
        uint64_t i = bigendian64 (i64);
        put ((uint8_t *) &i, 8);
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
      put ((uint8_t *) &fi, 4);
    }
    else
    {
      put (0xcb);
      union { double f; uint64_t i; } mem;
      mem.f = (double) f;
      uint64_t fi = bigendian64 (mem.i);
      put ((uint8_t *) &fi, 8);
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

  void put (uint8_t byte)
  {
    if (m_stream->full ())
    {
      size_t newcap = (write_approx_size (m_root) + 7u) & ~7u;
      m_stream->resize (newcap);
    }

    m_stream->put (byte);
  }

  void put (uint8_t *bytes, size_t count)
  {
    if (m_stream->full ())
    {
      size_t newcap = (write_approx_size (m_root) + 7u) & ~7u;
      m_stream->resize (newcap);
    }

    m_stream->put (bytes, count);
  }

  uint8_t * push (size_t count)
  {
    if (m_stream->full ())
    {
      size_t newcap = (write_approx_size (m_root) + 7u) & ~7u;
      m_stream->resize (newcap);
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

  bool write_stream ()
  {
    KVR_ASSERT (m_root);

    bool success = write_stream (m_root);
#if KVR_DEBUG
    if (success) { m_stream->set_eos (0); }
#endif
    return success;
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

  kvr::ostream *m_stream;
  const kvr::value *m_root;

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  static size_t write_approx_size (const kvr::value *val);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

size_t msgpack_write_context::write_approx_size (const kvr::value *val)
{
  size_t size = 0;

  //////////////////////////////////
  if (val->is_map ())
  //////////////////////////////////
  {
    kvr::sz_t msz = val->size ();

    if (msz <= 15)
    {
      size += 1;
    }
    else if (msz <= 0xffff)
    {
      size += 3;
    }
    else if (msz <= 0xffffffff)
    {
      size += 5;
    }

    kvr::value::cursor c = val->fcursor ();
    kvr::pair  *p = c.get ();
    while (p)
    {
      kvr::key *k = p->get_key ();
      kvr::value *v = p->get_value ();

      kvr::sz_t klen = k->get_length ();

      if (klen <= 31)
      {
        size += 1;
      }
      else if (klen <= 0xff)
      {
        size += 2;
      }
      else if (klen <= 0xffff)
      {
        size += 3;
      }
      else
      {
        size += 5;
      }

      size += klen;
      size += write_approx_size (v);

      p = c.get ();
    }
  }

  //////////////////////////////////
  else if (val->is_array ())
  //////////////////////////////////
  {
    kvr::sz_t alen = val->length ();

    if (alen <= 15)
    {
      size += 1;
    }
    else if (alen <= 0xffff)
    {
      size += 3;
    }
    else if (alen <= 0xffffffff)
    {
      size += 5;
    }

    for (kvr::sz_t i = 0, c = val->length (); i < c; ++i)
    {
      kvr::value *v = val->element (i);
      size += write_approx_size (v);
    }
  }

  //////////////////////////////////
  else if (val->is_string ())
  //////////////////////////////////
  {
    kvr::sz_t slen = 0;
    const char *str = val->get_string (&slen);

    if (slen <= 31)
    {
      size += 1;
    }
    else if (slen <= 0xff)
    {
      size += 2;
    }
    else if (slen <= 0xffff)
    {
      size += 3;
    }
    else
    {
      size += 5;
    }

    size += slen;
  }

  //////////////////////////////////
  else if (val->is_number_i ())
  //////////////////////////////////
  {
    int64_t n = val->get_number_i ();

    if (n > 0) // unsigned
    {
      if (n <= 127)
      {
        size += 1;
      }
      else if (n <= 0xff)
      {
        size += 2;
      }
      else if (n <= 0xffff)
      {
        size += 3;
      }
      else if (n <= 0xffffffff)
      {
        size += 5;
      }
      else
      {
        size += 9;
      }
    }
    else // signed
    {
      const int64_t nint5 = -((1 << 5) - 1);
      const int64_t nint8 = -((1 << 7) - 1);
      const int64_t nint16 = -((1 << 15) - 1);
      const int64_t nint32 = -((1LL << 31) - 1);

      if (n >= nint5)
      {
        size += 1;
      }
      else if (n >= nint8)
      {
        size += 2;
      }
      else if (n >= nint16)
      {
        size += 3;
      }
      else if (n >= nint32)
      {
        size += 5;
      }
      else
      {
        size += 9;
      }
    }
  }

  //////////////////////////////////
  else if (val->is_number_f ())
  //////////////////////////////////
  {
    const double fmax = std::numeric_limits<float>::max ();
    double n = val->get_number_f ();
    if (n <= fmax)
    {
      size += 5;
    }
    else
    {
      size += 9;
    }
  }

  //////////////////////////////////
  else if (val->is_boolean ())
  //////////////////////////////////
  {
    size += 1;
  }

  //////////////////////////////////
  else if (val->is_null ())
  //////////////////////////////////
  {
    size += 1;
  }

  return size;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// kvr_msgpack
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool kvr_msgpack::read (kvr::value *dest, const uint8_t *data, size_t size)
{
  KVR_ASSERT (false && "not implemented");

  bool success = false;

  msgpack_read_context ctx (dest);
  
  size_t sz = size;
  size_t pos = 0;
  size_t adv = 0;

  bool error = false;

  while (!error && (pos < sz))
  {
    adv = 0;

    uint8_t curr = data [pos];

    switch (curr)
    {
      case 0xc0: // null
      {
        error = !ctx.read_null ();
        ++adv;
        break;
      }

      case 0xc2: // false
      {
        error = !ctx.read_boolean (false);
        ++adv;
        break;
      }

      case 0xc3: // true
      {
        error = !ctx.read_boolean (true);
        ++adv;
        break;
      }

      default:
      {
        // todo: map, array, integer, float, string
        break;
      }
    }

    if (!error) { pos += adv; }
  }

  return success;  
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool kvr_msgpack::write (const kvr::value *src, kvr::ostream *ostr)
{
  bool success = false;

  msgpack_write_context ctx (src, ostr);
  success = ctx.write_stream ();

#if KVR_DEBUG && 1
  kvr::ostream hex (512);
  if (kvr_internal::hex_encode (ostr->bytes (), ostr->tell (), &hex))
  {
    hex.set_eos (0);
    const char *hexstr = (const char *) hex.bytes ();
    fprintf (stderr, "msgpack: %s\n", hexstr);

#if 1
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
  return msgpack_write_context::write_approx_size (val);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
