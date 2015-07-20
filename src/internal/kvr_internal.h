/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_INTERNAL_H
#define KVR_INTERNAL_H

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "../kvr.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include <assert.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#if KVR_DEBUG && 1
#define KVR_ASSERT(X) assert(X)
#define KVR_ASSERT_SAFE(X, R) KVR_ASSERT(X)
#else
#define KVR_ASSERT(X) 
#define KVR_ASSERT_SAFE(X, R) do { if (!X) { return (R); } } while (0)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (_MSC_VER)
#define kvr_strcpy(dst, dsz, src) strcpy_s (dst, dsz, src)
#define kvr_strncpy(dst, src, n) strncpy_s (dst, src, n)
#define kvr_strdup(src, sz) _strdup (src)
#else
#define kvr_strcpy(dst, dsz, src) { strncpy (dst, src, dsz); dst [dsz - 1] = 0; }
#define kvr_strncpy(dst, src, n) strncpy (dst, src, n)
#define kvr_strdup(src, sz) strndup (src, sz)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class kvr_internal
{
private:

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  struct json_read_context
  {
    json_read_context (kvr::value *value) : m_root (value), m_pair (NULL), m_depth (0)
    {
      memset (m_stack, 0, sizeof (m_stack));
    }
    
    bool Null ()
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

    bool Bool (bool b)
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

    bool Int (int i)
    {
      return Int64 ((int64_t) i);
    }

    bool Uint (unsigned u)
    {
      return Int64 ((int64_t) u);
    }

    bool Int64 (int64_t i)
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

    bool Uint64 (uint64_t u)
    {
      KVR_ASSERT (m_depth != 0);
      KVR_ASSERT (false && "not supported");
      return false;
    }

    bool Double (double d)
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
    
    bool String (const char *str, kvr_rapidjson::SizeType length, bool copy)
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
        pv->_set_string (str, (kvr::sz_t) length);
        m_pair = NULL;
        success = true;
      }
      else if (node->is_array ())
      {
        kvr::value *vstr = node->push_null (); KVR_ASSERT (vstr);
        vstr->conv_string ();
        vstr->_set_string (str, (kvr::sz_t) length);
        success = true;
      }

      return success;
    }
    
    bool StartObject ()
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
    
    bool Key (const char *str, kvr_rapidjson::SizeType length, bool copy)
    {
      kvr::value *node = m_stack [m_depth - 1];
      KVR_ASSERT_SAFE (node && node->is_map (), false);

      KVR_ASSERT (!m_pair);
      m_pair = node->insert_null (str);
      return (m_pair != NULL);
    }
    
    bool EndObject (kvr_rapidjson::SizeType memberCount)
    {
      kvr::value *node = m_stack [m_depth - 1];
      KVR_ASSERT_SAFE (node && node->is_map (), false);
      m_stack [--m_depth] = NULL;
      return true;
    }
    
    bool StartArray ()
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

    bool EndArray (kvr_rapidjson::SizeType elementCount)
    {
      kvr::value *node = m_stack [m_depth - 1];
      KVR_ASSERT_SAFE (node && node->is_array (), false);
      KVR_ASSERT (node->size () == (kvr::sz_t) elementCount);
      m_stack [--m_depth] = NULL;
      return true;
    }

    kvr::value  * m_stack [KVR_CONSTANT_MAX_TREE_DEPTH];
    kvr::value  * m_root;
    kvr::pair   * m_pair;
    kvr::sz_t     m_depth;
  };

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  struct json_write_context
  {
    json_write_context (char *data, size_t capacity) : m_data (data), m_pos (0), m_cap (capacity)
    {
      KVR_ASSERT (data);
      KVR_ASSERT (capacity > 0); 
    }

    ~json_write_context ()
    {
      KVR_ASSERT (m_data);
      m_data [m_pos] = 0;
    }

    void Put (char ch)
    {
      KVR_ASSERT (m_data);
      KVR_ASSERT (m_pos < m_cap);
      m_data [m_pos++] = ch;
    }

    char* Push (size_t count)
    {
      KVR_ASSERT (m_data);
      KVR_ASSERT (m_pos < m_cap);
      char *start = &m_data [m_pos];
      m_pos += count;
      return start;
    }

    void Pop (size_t count)
    {
      m_pos -= count;
    }

    void Flush () 
    {
    }

    const char *GetString () const
    {
      m_data [m_pos] = 0;
      return m_data;
    }

    size_t GetLength () const
    {
      return m_pos;
    }

    char  * m_data;    
    size_t  m_pos;
    size_t  m_cap;
  };

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static size_t json_write_approx_size (const kvr::value *val)
  {
    size_t size = 0;

    //////////////////////////////////
    if (val->is_map ())
    //////////////////////////////////
    {
      size += 2; // brackets
      kvr::value::cursor c = val->fcursor ();
      kvr::pair  *p = c.get ();
      while (p)
      {
        kvr::key *k = p->get_key ();
        kvr::value *v = p->get_value ();

        size += k->get_length () + 2; // + quotes
        size += kvr_internal::json_write_approx_size (v);
        size += 2; // colon and comma

        p = c.get ();
      }
    }

    //////////////////////////////////
    else if (val->is_array ())
    //////////////////////////////////
    {
      size += 2; // brackets
      for (kvr::sz_t i = 0, c = val->size (); i < c; ++i)
      {
        kvr::value *v = val->element (i);        
        size += kvr_internal::u32digits (i);
        size += kvr_internal::json_write_approx_size (v);
        size += 1; // comma
      }
    }

    //////////////////////////////////
    else if (val->is_string ())
    //////////////////////////////////
    {
      const char *str = val->get_string ();
      size += (val->_get_string_size () + 2); // + quotes
    }

    //////////////////////////////////
    else if (val->is_number_i ())
    //////////////////////////////////
    {
      int64_t n = val->get_number_i ();
      size += kvr_internal::i64digits (n); 
    }

    //////////////////////////////////
    else if (val->is_number_f ())
    //////////////////////////////////
    {
      double n = val->get_number_f ();
      size += 9; // average (guess)
    }

    //////////////////////////////////
    else if (val->is_boolean ())
    //////////////////////////////////
    {
      bool b = val->get_boolean ();
      size += b ? 4 : 5;
    }

    //////////////////////////////////
    else if (val->is_null ())
    //////////////////////////////////
    {
      size += 4;
    }

    return size;
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static bool json_write_stream (const kvr::value *val, kvr_rapidjson::Writer<json_write_context> &writer)
  {
    bool success = false;

    //////////////////////////////////
    if (val->is_map ())
    //////////////////////////////////
    {
      bool ok = writer.StartObject ();

      kvr::value::cursor c = val->fcursor ();
      kvr::pair *p = c.get ();
      while (p && ok)
      {
        kvr::key *k = p->get_key ();
        writer.Key (k->get_string (), k->get_length ());

        kvr::value *v = p->get_value ();
        ok = kvr_internal::json_write_stream (v, writer);

        p = c.get ();
      }

      success = ok && writer.EndObject ();
    }

    //////////////////////////////////
    else if (val->is_array ())
    //////////////////////////////////
    {
      bool ok = writer.StartArray ();

      for (kvr::sz_t i = 0, c = val->size (); (i < c) && ok; ++i)
      {
        kvr::value *v = val->element (i);        
        ok = kvr_internal::json_write_stream (v, writer);
      }

      success = ok && writer.EndArray ();
    }

    //////////////////////////////////
    else if (val->is_string ())
    //////////////////////////////////
    {
      const char *str = val->get_string ();      
      success = writer.String (str, strlen (str));
    }

    //////////////////////////////////
    else if (val->is_number_i ())
    //////////////////////////////////
    {
      int64_t n = val->get_number_i ();
      success = writer.Int64 (n);
    }

    //////////////////////////////////
    else if (val->is_number_f ())
    //////////////////////////////////
    {
      double n = val->get_number_f ();
      success = writer.Double (n);
    }

    //////////////////////////////////
    else if (val->is_boolean ())
    //////////////////////////////////
    {
      bool b = val->get_boolean ();
      success = writer.Bool (b);
    }

    //////////////////////////////////
    else if (val->is_null ())
    //////////////////////////////////
    {
      success = writer.Null ();
    }

    return success;
  }

public:

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static bool json_read (kvr::value *dest, const char *json, size_t len)
  {
    KVR_ASSERT (dest);
    KVR_ASSERT (json);
    
    bool success = true;

    do
    {
      if (!(dest && json))
      {
        success = false;
        break;
      }

      json_read_context rctx (dest);
      kvr_rapidjson::StringStream ss (json);
      kvr_rapidjson::Reader reader;

      if (!reader.Parse (ss, rctx))
      {
        dest->conv_null ();
        success = false;
        break;
      }

      if (rctx.m_depth != 0)
      {
        dest->conv_null ();
        success = false;
        break;
      }
    } 
    while (0);

    return success;
  };

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static bool json_write (const kvr::value *src, char **json, size_t *len)
  {
    KVR_ASSERT (src);
    KVR_ASSERT (json);
    KVR_ASSERT (len);

    bool success = true;

    do
    {
      size_t approxBufSize = json_write_approx_size (src);
      if (approxBufSize == 0)
      {
        success = false;
        break;
      }

      approxBufSize = (approxBufSize + 31U) & ~31U;
      char *data = new char [approxBufSize];
      json_write_context wctx (data, approxBufSize);
      kvr_rapidjson::Writer<json_write_context> writer (wctx);

      if (!kvr_internal::json_write_stream (src, writer))
      {
        delete [] data;
        success = false;
        break;
      }

      KVR_ASSERT (data == wctx.GetString ());
      *json = data;
      *len = wctx.GetLength ();

    } while (0);

    return success;
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  
  static bool msgpack_write_stream (const kvr::value *src, kvr::strbuffer *strbuf)
  {
    KVR_ASSERT (src);

    bool success = false;

    const kvr::value *val = src; 

    //////////////////////////////////
    if (val->is_map ())
    //////////////////////////////////
    {
      bool ok = true; //writer.StartObject ();

      kvr::value::cursor c = val->fcursor ();
      kvr::pair *p = c.get ();
      while (p && ok)
      {
        kvr::key *k = p->get_key ();
        //writer.Key (k->get_string (), k->get_length ());

        kvr::value *v = p->get_value ();
        ok = false; //kvr_internal::json_write_stream (v, writer);

        p = c.get ();
      }

      success = ok; //&& writer.EndObject ();
    }

    //////////////////////////////////
    else if (val->is_array ())
    //////////////////////////////////
    {
      bool ok = false; //writer.StartArray ();

      for (kvr::sz_t i = 0, c = val->size (); (i < c) && ok; ++i)
      {
        kvr::value *v = val->element (i);        
        ok = false; //kvr_internal::json_write_stream (v, writer);
      }

      success = ok; //&& writer.EndArray ();
    }

    //////////////////////////////////
    else if (val->is_string ())
    //////////////////////////////////
    {
      const char *str = val->get_string ();      
      success = true; //writer.String (str, strlen (str));
    }

    //////////////////////////////////
    else if (val->is_number_i ())
    //////////////////////////////////
    {
      int64_t n = val->get_number_i ();
      success = true; //writer.Int64 (n);
    }

    //////////////////////////////////
    else if (val->is_number_f ())
    //////////////////////////////////
    {
      double n = val->get_number_f ();
      success = true; //writer.Double (n);
    }

    //////////////////////////////////
    else if (val->is_boolean ())
    //////////////////////////////////
    {
      bool b = val->get_boolean ();
      printf ("%s", b ? "c3": "c2");
      success = true; //writer.Bool (b);
    }

    //////////////////////////////////
    else if (val->is_null ())
    //////////////////////////////////
    {
      printf("%s", "c0");
      success = true; //writer.Null ();
    }

    return success;
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static size_t u32toa (uint32_t u32, char dest [10])
  {
    const char* end = kvr_rapidjson::internal::u32toa (u32, dest);
    return (end - dest);
  }

  static size_t i64toa (int64_t i64, char dest [21])
  {
    const char* end = kvr_rapidjson::internal::i64toa (i64, dest);
    return (end - dest);
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static uint32_t u32digits (uint32_t u32)
  {
    uint32_t count = kvr_rapidjson::internal::CountDecimalDigit32 (u32);
    return count;
  }

  static uint32_t i64digits (int64_t i64)
  {
    int64_t n = i64;
    uint32_t count = (n < 0) ? 1 : 0; // sign
    do
    {
      count++;
      n /= 10;
    } while (n);

    return count;
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  static uint32_t strhash (const char *str)
  {
    KVR_ASSERT (str);
    // djb hash function (fast)

    uint32_t hash = 5381;
    char c;

    while ((c = *str++))
    {
      hash = ((hash << 5) + hash) + c;
    }

    return hash;
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
