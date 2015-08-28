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

static const uint8_t MSGPACK_HEADER_NULL        = 0xc0;
static const uint8_t MSGPACK_HEADER_BOOL_FALSE  = 0xc2;
static const uint8_t MSGPACK_HEADER_BOOL_TRUE   = 0xc3;
static const uint8_t MSGPACK_HEADER_UNSIGNED_8  = 0xcc;
static const uint8_t MSGPACK_HEADER_UNSIGNED_16 = 0xcd;
static const uint8_t MSGPACK_HEADER_UNSIGNED_32 = 0xce;
static const uint8_t MSGPACK_HEADER_UNSIGNED_64 = 0xcf;
static const uint8_t MSGPACK_HEADER_SIGNED_5    = 7u << 5;
static const uint8_t MSGPACK_HEADER_SIGNED_8    = 0xd0;
static const uint8_t MSGPACK_HEADER_SIGNED_16   = 0xd1;
static const uint8_t MSGPACK_HEADER_SIGNED_32   = 0xd2;
static const uint8_t MSGPACK_HEADER_SIGNED_64   = 0xd3;
static const uint8_t MSGPACK_HEADER_FLOAT_32    = 0xca;
static const uint8_t MSGPACK_HEADER_FLOAT_64    = 0xcb;
static const uint8_t MSGPACK_HEADER_ARRAY_4     = 9u << 4;
static const uint8_t MSGPACK_HEADER_ARRAY_16    = 0xdc;
static const uint8_t MSGPACK_HEADER_ARRAY_32    = 0xdd;
static const uint8_t MSGPACK_HEADER_MAP_4       = 8u << 4;
static const uint8_t MSGPACK_HEADER_MAP_16      = 0xde;
static const uint8_t MSGPACK_HEADER_MAP_32      = 0xdf;
static const uint8_t MSGPACK_HEADER_STRING_5    = 5u << 5;
static const uint8_t MSGPACK_HEADER_STRING_8    = 0xd9;
static const uint8_t MSGPACK_HEADER_STRING_16   = 0xda;
static const uint8_t MSGPACK_HEADER_STRING_32   = 0xdb;

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// msgpack_read_context
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct msgpack_read_context
{
  msgpack_read_context (kvr::value *value) : m_root (value), m_temp (NULL), m_depth (0)
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
      KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
      m_temp = NULL;
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
      KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
      m_temp->conv_boolean ();
#endif
      m_temp->set_boolean (b);
      m_temp = NULL;
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
      KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
      m_temp->conv_integer ();
#endif
      m_temp->set_integer (i);
      m_temp = NULL;
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
      KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
#if KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF
      m_temp->conv_float ();
#endif
      m_temp->set_float (d);
      m_temp = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      node->push (d);
      success = true;
    }

    return success;
  }

  bool read_string (const char *str, kvr::sz_t length)
  {
    bool success = false;

    KVR_ASSERT (str);
    KVR_ASSERT_SAFE (m_depth != 0, success);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT (node);
    KVR_ASSERT (node->is_map () || node->is_array ());

    if (node->is_map ())
    {
      KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
      m_temp->conv_string ();
      m_temp->set_string (str, length);
      m_temp = NULL;
      success = true;
    }
    else if (node->is_array ())
    {
      kvr::value *vstr = node->push_null (); KVR_ASSERT (vstr);
      vstr->conv_string ();
      vstr->set_string (str, length);
      success = true;
    }

    return success;
  }

  bool read_map_start (kvr::sz_t size)
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
        node = m_temp;
        KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
        m_temp->conv_map (size);
        m_temp = NULL;
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
      node = m_root->conv_map (size);
      success = true;
    }

    KVR_ASSERT (m_depth < KVR_CONSTANT_MAX_TREE_DEPTH);
    m_stack [m_depth++] = node;

    return success;
  }

  bool read_key (const char *str, kvr::sz_t length)
  {
    KVR_ASSERT (str);
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT_SAFE (node && node->is_map (), false);

    KVR_ASSERT (!m_temp);

    char key [KVR_CONSTANT_MAX_KEY_LENGTH];
    KVR_ASSERT (length < KVR_CONSTANT_MAX_KEY_LENGTH);
    kvr_strncpy (key, KVR_CONSTANT_MAX_KEY_LENGTH, str, length);

    m_temp = node->insert_null (key);
    return (m_temp != NULL);
  }

  bool read_map_end (kvr::sz_t size)
  {
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT_SAFE (node && node->is_map (), false);
    KVR_ASSERT (node->size () == size);
    m_stack [--m_depth] = NULL;
    return true;
  }

  bool read_array_start (kvr::sz_t length)
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
        node = m_temp;
        KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
        m_temp->conv_array (length);
        m_temp = NULL;
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
      node = m_root->conv_array (length);
      success = true;
    }

    KVR_ASSERT (m_depth < KVR_CONSTANT_MAX_TREE_DEPTH);
    m_stack [m_depth++] = node;

    return success;
  }

  bool read_array_end (kvr::sz_t length)
  {
    kvr::value *node = m_stack [m_depth - 1];
    KVR_ASSERT_SAFE (node && node->is_array (), false);
    KVR_ASSERT (node->length () == length);
    m_stack [--m_depth] = NULL;
    return true;
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  kvr::value  * m_stack [KVR_CONSTANT_MAX_TREE_DEPTH];
  kvr::value  * m_root;
  kvr::value  * m_temp;
  kvr::sz_t     m_depth;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// msgpack_reader
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class msgpack_reader
{
public:

  enum parse_error
  {
    PARSE_ERROR_OK,
    PARSE_ERROR_EOS,
  };

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  msgpack_reader (kvr::istream *istr) : m_stream (istr), m_error (PARSE_ERROR_OK)
  {
    m_stream->seek (0);
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  bool parse (msgpack_read_context &ctx)
  {
    bool success = false;

    uint8_t curr = 0;
    if (this->get (&curr))
    {
      switch (curr)
      {
        case MSGPACK_HEADER_NULL:       // null
        {
          success = ctx.read_null ();
          break;
        }

        case MSGPACK_HEADER_BOOL_FALSE: // false
        {
          success = ctx.read_boolean (false);
          break;
        }

        case MSGPACK_HEADER_BOOL_TRUE:  // true
        {
          success = ctx.read_boolean (true);
          break;
        }

        case MSGPACK_HEADER_FLOAT_32:   // float32
        {
          uint32_t fi = 0;
          if (this->get ((uint8_t *) &fi, 4))
          {
            union { float f; uint32_t i; } mem;
            mem.i = bigendian32 (fi);
            success = ctx.read_float (mem.f);
          }

          break;
        }

        case MSGPACK_HEADER_FLOAT_64:   // float64
        {
          uint64_t fi = 0;          
          if (this->get ((uint8_t *) &fi, 8))
          {
            union { double f; uint64_t i; } mem;
            mem.i = bigendian64 (fi);
            success = ctx.read_float (mem.f);
          }
          break;
        }

        case MSGPACK_HEADER_STRING_8:   // string8
        {
          uint8_t slen = 0;
          success = this->get (&slen);
          const char *str = success ? (const char *) this->push (slen) : NULL;
          success = str ? ctx.read_string (str, slen) : false;
          break;
        }

        case MSGPACK_HEADER_STRING_16:  // string16
        {
          uint16_t len = 0;
          success = this->get ((uint8_t *) &len, 2);
          uint16_t slen = bigendian16 (len);
          KVR_ASSERT (slen <= kvr::SZ_T_MAX);
          const char *str = success ? (const char *) this->push (slen) : NULL;
          success = str ? ctx.read_string (str, slen) : false;
          break;
        }

        case MSGPACK_HEADER_STRING_32:  // string32
        {
          uint32_t len = 0;
          success = this->get ((uint8_t *) &len, 4);
          uint32_t slen = bigendian32 (len);
          KVR_ASSERT (slen <= kvr::SZ_T_MAX);
          const char *str = success ? (const char *) this->push (slen) : NULL;
          success = str ? ctx.read_string (str, slen) : false;
          break;
        }

        case MSGPACK_HEADER_ARRAY_16:   // array16
        {
          uint16_t len = 0;
          success = this->get ((uint8_t *) &len, 2);
          uint16_t alen = bigendian16 (len);
          KVR_ASSERT (alen <= kvr::SZ_T_MAX);
          bool ok = success && ctx.read_array_start (alen);
          for (uint16_t i = 0; ok && (i < alen); ++i)
          {
            ok &= parse (ctx);
          }
#if KVR_DEBUG
          ok &= ctx.read_array_end (alen);
#endif
          success = ok;
          break;
        }

        case MSGPACK_HEADER_ARRAY_32:   // array32
        {
          uint32_t len = 0;
          success = this->get ((uint8_t *) &len, 4);
          uint32_t alen = bigendian32 (len);
          KVR_ASSERT (alen <= kvr::SZ_T_MAX);
          bool ok = success && ctx.read_array_start (alen);
          for (uint32_t i = 0; ok && (i < alen); ++i)
          {
            ok &= parse (ctx);
          }
#if KVR_DEBUG
          ok &= ctx.read_array_end (alen);
#endif
          success = ok;

          break;
        }

        case MSGPACK_HEADER_MAP_16:     // map16
        {
          uint16_t len = 0;
          success = this->get ((uint8_t *) &len, 2);
          uint16_t msz = bigendian16 (len);
          KVR_ASSERT (msz <= kvr::SZ_T_MAX);
          bool ok = success && ctx.read_map_start (msz);
          for (uint16_t i = 0; ok && (i < msz); ++i)
          {
            ok &= parse_key (ctx);
            ok &= parse (ctx);
          }
#if KVR_DEBUG
          ok &= ctx.read_map_end (msz);
#endif
          success = ok;
          break;
        }

        case MSGPACK_HEADER_MAP_32:     // map32
        {
          uint32_t len = 0;
          success = this->get ((uint8_t *) &len, 4);
          uint32_t msz = bigendian32 (len);
          KVR_ASSERT (msz <= kvr::SZ_T_MAX);
          bool ok = success && ctx.read_map_start (msz);
          for (uint32_t i = 0; ok && (i < msz); ++i)
          {
            ok &= parse_key (ctx);
            ok &= parse (ctx);
          }
#if KVR_DEBUG
          ok &= ctx.read_map_end (msz);
#endif
          success = ok;
          break;
        }
        case MSGPACK_HEADER_UNSIGNED_8:
        {
          uint8_t i = 0;
          success = this->get (&i);
          success = success ? ctx.read_integer (i) : false;
          break;
        }

        case MSGPACK_HEADER_UNSIGNED_16:
        {
          uint16_t u16 = 0;
          success = this->get ((uint8_t *) &u16, 2);
          uint16_t i = bigendian16 (u16);
          success = success ? ctx.read_integer (i) : false;
          break;
        }

        case MSGPACK_HEADER_UNSIGNED_32:
        {
          uint32_t u32 = 0;
          success = this->get ((uint8_t *) &u32, 4);
          uint32_t i = bigendian32 (u32);
          success = success ? ctx.read_integer (i) : false;
          break;
        }

        case MSGPACK_HEADER_UNSIGNED_64:
        {
          KVR_ASSERT (false && "not supported");

          uint64_t u64 = 0;
          success = this->get ((uint8_t *) &u64, 8);
          uint64_t i = bigendian64 (u64);
          success = success ? ctx._read_uint64 (i) : false;
          break;
        }

        case MSGPACK_HEADER_SIGNED_8:
        {
          uint8_t u8 = 0;
          success = this->get (&u8);
          int8_t i = (int8_t) u8;
          success = success ? ctx.read_integer (i) : false;
          break;
        }

        case MSGPACK_HEADER_SIGNED_16:
        {
          uint16_t u16 = 0;
          success = this->get ((uint8_t *) &u16, 2);
          uint16_t u = bigendian32 (u16);
          int16_t i = (int16_t) u;
          success = success ? ctx.read_integer (i) : false;
          break;
        }

        case MSGPACK_HEADER_SIGNED_32:
        {
          uint32_t u32 = 0;
          success = this->get ((uint8_t *) &u32, 4);
          uint32_t u = bigendian32 (u32);
          int32_t i = (int32_t) u;
          success = success ? ctx.read_integer (i) : false;

          break;
        }

        case MSGPACK_HEADER_SIGNED_64:
        {
          uint64_t u64 = 0;
          success = this->get ((uint8_t *) &u64, 8);
          uint64_t u = bigendian64 (u64);
          int64_t i = (int64_t) u;
          success = success ? ctx.read_integer (i) : false;
          break;
        }

        default:
        {
          if (curr <= 127) // 7-bit positive int
          {
            int64_t i = (int64_t) curr;
            success = ctx.read_integer (i);
          }
          else if ((curr & MSGPACK_HEADER_SIGNED_5) == 0)
          {
            int64_t i = (int64_t) curr;
            success = ctx.read_integer (i);
          }
          else if ((curr & 0xe0) == MSGPACK_HEADER_STRING_5)
          {
            uint8_t slen = (curr & 0x1f);
            const char *str = (const char *) this->push (slen);
            success = str ? ctx.read_string (str, slen) : false;
          }
          else
          {
            uint8_t hi4 = (curr & 0xf0);
            if (hi4 == MSGPACK_HEADER_MAP_4)
            {
              uint8_t msz = (curr & 0x0f);
              bool ok = ctx.read_map_start (msz);
              for (uint8_t i = 0; ok && (i < msz); ++i)
              {
                ok &= parse_key (ctx);
                ok &= parse (ctx);
              }
#if KVR_DEBUG
              ok &= ctx.read_map_end (msz);
#endif
              success = ok;
            }
            else if (hi4 == MSGPACK_HEADER_ARRAY_4)
            {
              uint8_t alen = (curr & 0x0f);
              bool ok = ctx.read_array_start (alen);
              for (uint8_t i = 0; ok && (i < alen); ++i)
              {
                ok &= parse (ctx);
              }
#if KVR_DEBUG
              ok = ok && ctx.read_array_end (alen);
#endif
              success = ok;
            }
            else
            {
              KVR_ASSERT (false);
              success = false;
            }
          }

          break;
        }
      }
    }

    return success;
  }

private:

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  bool parse_key (msgpack_read_context &ctx)
  {
    bool success = false;

    uint8_t curr = 0;

    if (this->get (&curr))
    {
      switch (curr)
      {
        case MSGPACK_HEADER_STRING_8:   // string8
        {
          uint8_t slen = 0;
          success = this->get (&slen);
          const char *str = success ? (const char *) this->push (slen) : NULL;
          success = str ? ctx.read_key (str, slen) : false;
          break;
        }

        case MSGPACK_HEADER_STRING_16:  // string16
        {
          uint16_t len = 0;
          success = this->get ((uint8_t *) &len, 2);
          uint16_t slen = bigendian16 (len);
          KVR_ASSERT (slen <= kvr::SZ_T_MAX);
          const char *str = success ? (const char *) this->push (slen) : NULL;
          success = str ? ctx.read_key (str, slen) : false;
          break;
        }

        case MSGPACK_HEADER_STRING_32:  // string32
        {
          uint32_t len = 0;
          success = this->get ((uint8_t *) &len, 4);
          uint32_t slen = bigendian32 (len);
          KVR_ASSERT (slen <= kvr::SZ_T_MAX);
          const char *str = success ? (const char *) this->push (slen) : NULL;
          success = str ? ctx.read_key (str, slen) : false;
          break;
        }

        default:
        {
          if ((curr & 0xe0) == MSGPACK_HEADER_STRING_5)
          {
            uint8_t slen = (curr & 0x1f);
            const char *str = (const char *) this->push (slen);
            success = str ? ctx.read_key (str, slen) : false;
          }
          else
          {
            success = false;
          }

          break;
        }
      }
    }

    return success;
  };

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  bool get (uint8_t *byte)
  {
    bool ok = m_stream->get (byte);
    if (!ok) { m_error = PARSE_ERROR_EOS; }
    return ok;
  }

  bool get (uint8_t *bytes, size_t count)
  {
    bool ok = m_stream->get (bytes, count);
    if (!ok) { m_error = PARSE_ERROR_EOS; }
    return ok;
  }

  const uint8_t * push (size_t count)
  {
    const uint8_t *bytes = m_stream->push (count);
    if (!bytes) { m_error = PARSE_ERROR_EOS; }
    return bytes;
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  kvr::istream *m_stream;
  parse_error m_error;
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
  msgpack_write_context (kvr::ostream *ostream) : m_stream (ostream)
  {
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
    put (MSGPACK_HEADER_NULL);
    return true;
  }

  bool write_array (kvr::sz_t size)
  {
    if (size <= 15)
    {
      uint8_t sz = (uint8_t) size;
      put (MSGPACK_HEADER_ARRAY_4 | sz);
    }
    else if (size <= 0xffff)
    {
      put (MSGPACK_HEADER_ARRAY_16);
      uint16_t sz = bigendian16 (size);
      put ((uint8_t *) &sz, 2);
    }
    else if (size <= 0xffffffff)
    {
      put (MSGPACK_HEADER_ARRAY_32);
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
      put (MSGPACK_HEADER_MAP_4 | sz);
    }
    else if (size <= 0xffff)
    {
      put (MSGPACK_HEADER_MAP_16);
      uint16_t sz = bigendian16 (size);
      put ((uint8_t *) &sz, 2);
    }
    else if (size <= 0xffffffff)
    {
      put (MSGPACK_HEADER_MAP_32);
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
      put (MSGPACK_HEADER_STRING_5 | len);
    }
    else if (slen <= 0xff)
    {
      uint8_t len = (uint8_t) slen;
      put (MSGPACK_HEADER_STRING_8);
      put (len);
    }
    else if (slen <= 0xffff)
    {
      put (MSGPACK_HEADER_STRING_16);
      uint16_t len = bigendian16 (slen);
      put ((uint8_t *) &len, 2);
    }
    else if (slen <= 0xffffffff)
    {
      put (MSGPACK_HEADER_STRING_32);
      uint32_t len = bigendian32 (slen);
      put ((uint8_t *) &len, 4);
    }
    else
    {
      return false;
    }

    put ((uint8_t *) str, (size_t) slen);

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
        put (MSGPACK_HEADER_UNSIGNED_8);
        put (i);
      }
      else if (i64 <= 0xffff)
      {
        put (MSGPACK_HEADER_UNSIGNED_16);
        uint16_t i = bigendian16 (i64);
        put ((uint8_t *) &i, 2);
      }
      else if (i64 <= 0xffffffff)
      {
        put (MSGPACK_HEADER_UNSIGNED_32);
        uint32_t i = bigendian32 (i64);
        put ((uint8_t *) &i, 4);
      }
      else // max is int64_t;
      {
        put (MSGPACK_HEADER_SIGNED_64);
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
        put (MSGPACK_HEADER_SIGNED_5 | i);
      }
      else if (i64 >= nint8)
      {
        uint8_t i = (uint8_t) i64;
        put (MSGPACK_HEADER_SIGNED_8);
        put (i);
      }
      else if (i64 >= nint16)
      {
        put (MSGPACK_HEADER_SIGNED_16);
        uint16_t i = bigendian16 (i64);
        put ((uint8_t *) &i, 2);
      }
      else if (i64 >= nint32)
      {
        put (MSGPACK_HEADER_SIGNED_32);
        uint32_t i = bigendian32 (i64);
        put ((uint8_t *) i, 4);
      }
      else
      {
        put (MSGPACK_HEADER_SIGNED_64);
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
      put (MSGPACK_HEADER_FLOAT_32);
      union { float f; uint32_t i; } mem;
      mem.f = (float) f;
      uint32_t fi = bigendian32 (mem.i);
      put ((uint8_t *) &fi, 4);
    }
    else
    {
      put (MSGPACK_HEADER_FLOAT_64);
      union { double f; uint64_t i; } mem;
      mem.f = (double) f;
      uint64_t fi = bigendian64 (mem.i);
      put ((uint8_t *) &fi, 8);
    }

    return true;
  }

  bool write_boolean (bool b)
  {
    put (b ? MSGPACK_HEADER_BOOL_TRUE : MSGPACK_HEADER_BOOL_FALSE);
    return true; 
  }
  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  void put (uint8_t byte)
  {
    while (!m_stream->put (byte))
    {
      size_t newcap = m_stream->capacity () * 2;
      m_stream->resize (newcap);
    }
  }

  void put (uint8_t *bytes, size_t count)
  {
    while (!m_stream->put (bytes, count))
    {
      size_t newcap = m_stream->capacity () * 2;
      m_stream->resize (newcap);
    }
  }
  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  kvr::ostream *m_stream;

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
    kvr::pair p;
    while (c.get (&p))
    {
      kvr::key *k = p.get_key ();
      kvr::value *v = p.get_value ();
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
    KVR_REF_UNUSED (str);

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
  else if (val->is_integer ())
  //////////////////////////////////
  {
    int64_t n = val->get_integer ();

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
  else if (val->is_float ())
  //////////////////////////////////
  {
    const double fmax = std::numeric_limits<float>::max ();
    double n = val->get_float ();
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
// msgpack_writer
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class msgpack_writer
{
public:

  msgpack_writer (const kvr::value *val) : m_root (val)
  {
  }

  bool print (msgpack_write_context &ctx)
  {
    return print_r (m_root, ctx);
  }

private:

  bool print_r (const kvr::value *val, msgpack_write_context &ctx)
  {
    KVR_ASSERT (val);

    bool success = false;

    //////////////////////////////////
    if (val->is_map ())
    //////////////////////////////////
    {
      kvr::sz_t msz = val->size ();
      bool ok = ctx.write_map (msz);
      kvr::value::cursor c = val->fcursor ();
      kvr::pair p;
      while (ok && c.get (&p))
      {
        kvr::key *k = p.get_key ();
        ok &= ctx.write_string (k->get_string (), k->get_length ());

        kvr::value *v = p.get_value ();
        ok &= print_r (v, ctx);
      }

      success = ok;
    }

    //////////////////////////////////
    else if (val->is_array ())
    //////////////////////////////////
    {
      kvr::sz_t alen = val->length ();
      bool ok = ctx.write_array (alen);
      for (kvr::sz_t i = 0; (i < alen) && ok; ++i)
      {
        kvr::value *v = val->element (i);
        ok &= print_r (v, ctx);
      }
      success = ok;
    }

    //////////////////////////////////
    else if (val->is_string ())
    //////////////////////////////////
    {
      kvr::sz_t slen = 0;
      const char *str = val->get_string (&slen);
      success = ctx.write_string (str, slen);
    }

    //////////////////////////////////
    else if (val->is_integer ())
    //////////////////////////////////
    {
      int64_t n = val->get_integer ();
      success = ctx.write_integer (n);
    }

    //////////////////////////////////
    else if (val->is_float ())
    //////////////////////////////////
    {
      double n = val->get_float ();
      success = ctx.write_float (n);
    }

    //////////////////////////////////
    else if (val->is_boolean ())
    //////////////////////////////////
    {
      bool b = val->get_boolean ();
      success = ctx.write_boolean (b);
    }

    //////////////////////////////////
    else if (val->is_null ())
    //////////////////////////////////
    {
      success = ctx.write_null ();
    }

    return success;
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  const kvr::value *m_root;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// kvr_msgpack
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool kvr_msgpack::read (kvr::value *dest, const uint8_t *data, size_t size)
{
  KVR_ASSERT (dest);
  KVR_ASSERT (data);
  KVR_ASSERT ((size > 0));

  kvr::istream stream (data, size);
  msgpack_reader reader (&stream);
  msgpack_read_context ctx (dest);  

  bool success = reader.parse (ctx);
  return success;  
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool kvr_msgpack::write (const kvr::value *src, kvr::ostream *ostr)
{
  KVR_ASSERT (src);
  KVR_ASSERT (ostr);

  msgpack_writer writer (src);
  msgpack_write_context ctx (ostr);

  bool success = writer.print (ctx);
#if defined(_MSC_VER) && KVR_DEBUG && 1
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
