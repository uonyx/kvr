///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Copyright (c) 2015 Ubaka Onyechi
 *
 * kvr is free software distributed under the MIT license.
 * See https://raw.githubusercontent.com/uonyx/kvr/master/LICENSE for details.
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_CBOR
#define KVR_CBOR

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define KVR_CBOR_READ_KEY_SPECIALIZATION    0
#define KVR_CBOR_WRITE_COMPACT_FP_OVERRIDE  0

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace kvr
{
  namespace internal
  {
    namespace cbor
    {
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      // Specification: https://tools.ietf.org/html/rfc7049

      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      static const uint8_t CBOR_MAJOR_TYPE_0 = 0x00;  // unsigned integer
      static const uint8_t CBOR_MAJOR_TYPE_1 = 0x20;  // negative integer
      static const uint8_t CBOR_MAJOR_TYPE_2 = 0x40;  // byte string - unused
      static const uint8_t CBOR_MAJOR_TYPE_3 = 0x60;  // text string
      static const uint8_t CBOR_MAJOR_TYPE_4 = 0x80;  // array
      static const uint8_t CBOR_MAJOR_TYPE_5 = 0xa0;  // map
      static const uint8_t CBOR_MAJOR_TYPE_6 = 0xc0;  // semantic tagging - unused
      static const uint8_t CBOR_MAJOR_TYPE_7 = 0xe0;  // floating point and no content simple data types

      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      
      static const uint8_t CBOR_VALUE_TYPE_UINT8    = 24;
      static const uint8_t CBOR_VALUE_TYPE_UINT16   = 25;
      static const uint8_t CBOR_VALUE_TYPE_UINT32   = 26;
      static const uint8_t CBOR_VALUE_TYPE_UINT64   = 27;
      static const uint8_t CBOR_VALUE_TYPE_FALSE    = 20;
      static const uint8_t CBOR_VALUE_TYPE_TRUE     = 21;
      static const uint8_t CBOR_VALUE_TYPE_NULL     = 22;
      static const uint8_t CBOR_VALUE_TYPE_FLOAT16  = 25;
      static const uint8_t CBOR_VALUE_TYPE_FLOAT32  = 26;
      static const uint8_t CBOR_VALUE_TYPE_FLOAT64  = 27;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      struct read_ctx
      {
        ////////////////////////////////////////////////////////////

        read_ctx (kvr::value *value) : m_root (value), m_temp (NULL), m_depth (0)
        {
          memset (m_stack, 0, sizeof (m_stack));
        }

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

        bool _read_uint64 (uint64_t u)
        {
          KVR_ASSERT (m_depth != 0);
          KVR_ASSERT (false && "not supported");
          KVR_REF_UNUSED (u);
          return false;
        }

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

        bool read_key (const char *str, kvr::sz_t length)
        {
          KVR_ASSERT (str);
          kvr::value *node = m_stack [m_depth - 1];
          KVR_ASSERT_SAFE (node && node->is_map (), false);

          KVR_ASSERT (!m_temp);
    #if KVR_CBOR_READ_KEY_SPECIALIZATION
          char key [256];
          KVR_ASSERT (length < 256);
          kvr_strncpy (key, 256, str, length);
          m_temp = node->insert_null (key);
    #else
          KVR_REF_UNUSED (length);
          m_temp = node->insert_null (str);
    #endif
          return (m_temp != NULL);
        }

        ////////////////////////////////////////////////////////////

        bool read_map_end (kvr::sz_t size)
        {
          kvr::value *node = m_stack [m_depth - 1];
          KVR_ASSERT_SAFE (node && node->is_map (), false);
          KVR_ASSERT (node->size () == size);
          KVR_REF_UNUSED (size);
          m_stack [--m_depth] = NULL;
          return true;
        }

        ////////////////////////////////////////////////////////////

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

        ////////////////////////////////////////////////////////////

        bool read_array_end (kvr::sz_t length)
        {
          kvr::value *node = m_stack [m_depth - 1];
          KVR_ASSERT_SAFE (node && node->is_array (), false);
          KVR_ASSERT (node->length () == length);
          KVR_REF_UNUSED (length);
          m_stack [--m_depth] = NULL;
          return true;
        }

        ////////////////////////////////////////////////////////////

        kvr::value  * m_stack [KVR_CONSTANT_MAX_TREE_DEPTH];
        kvr::value  * m_root;
        kvr::value  * m_temp;
        kvr::sz_t     m_depth;
      };

      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      template<typename istr>
      struct reader
      {
      public:

        reader () : m_ss (256) {}

        bool parse (istr *is, read_ctx &ctx)
        {
          KVR_ASSERT (is);

          bool success = false;

          uint8_t curr = 0;
          if (is->get (&curr))
          {
            uint8_t major_type = (curr & 0xe0);
            uint8_t value_type = (curr & 0x1f);
            
            switch (major_type)
            {
              case CBOR_MAJOR_TYPE_0: // unsigned integer
              {
                if (value_type < CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_unsigned5 (ctx, value_type);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_unsigned8 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT16)
                {
                  success = parse_unsigned16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT32)
                {
                  success = parse_unsigned32 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT64)
                {
                  success = parse_unsigned64 (is, ctx);
                }

                break;
              }

              case CBOR_MAJOR_TYPE_1: // negative integer
              { 
                if (value_type < CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_negint5 (ctx, value_type);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_negint8 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT16)
                {
                  success = parse_negint16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT32)
                {
                  success = parse_negint32 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT64)
                {
                  success = parse_negint64 (is, ctx);
                }

                break; 
              }

              case CBOR_MAJOR_TYPE_2: // byte string
              { 
                KVR_ASSERT (false && "unsupported major type (2): byte string");
                // maybe skip bytes? no. client stream interface can't cope atm.
                success = false; 
                break; 
              } 

              case CBOR_MAJOR_TYPE_3: // text string
              { 
                if (value_type < CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_string5 (is, ctx, value_type);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_string8 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT16)
                {
                  success = parse_string16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT32)
                {
                  success = parse_string32 (is, ctx);
                }

                break; 
              }

              case CBOR_MAJOR_TYPE_4: // array
              { 
                if (value_type < CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_array5 (is, ctx, value_type);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_array8 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT16)
                {
                  success = parse_array16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT32)
                {
                  success = parse_array32 (is, ctx);
                }

                break; 
              }

              case CBOR_MAJOR_TYPE_5: // map
              { 
                if (value_type < CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_map5 (is, ctx, value_type);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_map8 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT16)
                {
                  success = parse_map16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT32)
                {
                  success = parse_map32 (is, ctx);
                }

                break; 
              }

              case CBOR_MAJOR_TYPE_6: // semantic tagging
              {
                KVR_ASSERT (false && "unsupported major type (6): semantic tagging");
                // maybe skip bytes? no. client stream interface can't cope atm.
                success = false;
                break;
              }

              case CBOR_MAJOR_TYPE_7: // float, bool, null
              { 
                if (value_type == CBOR_VALUE_TYPE_FLOAT64)
                {
                  success = parse_float64 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_FLOAT32)
                {
                  success = parse_float32 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_FLOAT16)
                {
                  success = parse_float16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_TRUE)
                {
                  success = parse_bool (ctx, true);
                }
                else if (value_type == CBOR_VALUE_TYPE_FALSE)
                {
                  success = parse_bool (ctx, false);
                }
                else if (value_type == CBOR_VALUE_TYPE_NULL)
                {
                  success = parse_null (ctx);
                }

                break; 
              }

              default:
              {
                success = false;
                break;
              }
            }
          }

          return success;
        }

      private:

        ////////////////////////////////////////////////////////////

        bool parse_key (istr *is, read_ctx &ctx)
        {
          KVR_ASSERT (is);

          bool success = false;

          uint8_t curr = 0;
          if (is->get (&curr))
          {
            uint8_t major_type = (curr & 0xe0);
            uint8_t value_type = (curr & 0x1f);

            switch (major_type)
            {
              case CBOR_MAJOR_TYPE_3: // text string
              {
                if (value_type < CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_key5 (is, ctx, value_type);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT8)
                {
                  success = parse_key8 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT16)
                {
                  success = parse_key16 (is, ctx);
                }
                else if (value_type == CBOR_VALUE_TYPE_UINT32)
                {
                  success = parse_key32 (is, ctx);
                }

                break;
              }

              default:
              {
                success = false;
                break;
              }
            }
          }

          return success;
        }

        ////////////////////////////////////////////////////////////

        bool parse_key5 (istr *is, read_ctx &ctx, uint8_t data)
        {
          uint8_t slen = (data & 0x1f);
          //char str [32];
          m_ss.seek (0);
          uint8_t *str = m_ss.push (slen + 1);
          KVR_ASSERT (str);
          if (is->read (str, slen))
          {
            str [slen] = 0;
            return ctx.read_key (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_key8 (istr *is, read_ctx &ctx)
        {
          uint8_t slen = 0;
          if (is->get (&slen))
          {
            m_ss.seek (0);
            uint8_t *str = m_ss.push (slen + 1);
            KVR_ASSERT (str);
            if (is->read (str, slen))
            {
              str [slen] = 0;
              return ctx.read_key (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
            }
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_key16 (istr *is, read_ctx &ctx)
        {
          uint16_t len = 0;
          if (is->read ((uint8_t *) &len, 2))
          {
            m_ss.seek (0);
            uint16_t slen = kvr_bigendian16 (len);
            uint8_t *str = m_ss.push (slen + 1);
            KVR_ASSERT (str);
            if (is->read (str, slen))
            {
              str [slen] = 0;
              return ctx.read_key (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
            }
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_key32 (istr *is, read_ctx &ctx)
        {
          uint32_t len = 0;
          if (is->read ((uint8_t *) &len, 4))
          {
            m_ss.seek (0);
            uint32_t slen = kvr_bigendian32 (len);
            uint8_t *str = m_ss.push (slen + 1);
            KVR_ASSERT (str);
            if (is->read (str, slen))
            {
              str [slen] = 0;
              return ctx.read_key (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
            }
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_null (read_ctx &ctx)
        {
          return ctx.read_null ();
        }

        ////////////////////////////////////////////////////////////

        bool parse_bool (read_ctx &ctx, bool val)
        {
          return ctx.read_boolean (val);
        }

        ////////////////////////////////////////////////////////////

        bool parse_float16 (istr *is, read_ctx &ctx)
        {
          uint16_t fi = 0;
          if (is->read ((uint8_t *) &fi, 2))
          {
            uint16_t hf = kvr_bigendian16 (fi);
            float f = 0.0f;
            if (kvr::internal::fp_half_to_single (hf, &f))
            {
              return ctx.read_float (f);
            }
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_float32 (istr *is, read_ctx &ctx)
        {
          uint32_t u = 0;
          if (is->read ((uint8_t *) &u, 4))
          {
#if KVR_INTERNAL_FLAG_DEBUG_TYPE_PUNNING_ON
            union { float f; uint32_t u; } mem;
            mem.u = kvr_bigendian32 (u);
            return ctx.read_float (mem.f);
#else
            float f = 0;
            uint32_t u32 = kvr_bigendian32 (u);
            KVR_ASSERT (sizeof (f) == sizeof (u32));
            memcpy (&f, &u32, sizeof (u32));
            return ctx.read_float (f);
#endif
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_float64 (istr *is, read_ctx &ctx)
        {
          uint64_t u = 0;
          if (is->read ((uint8_t *) &u, 8))
          {
#if KVR_INTERNAL_FLAG_DEBUG_TYPE_PUNNING_ON
            union { double f; uint64_t u; } mem;
            //union { double f; uint64_t u; } mem = { kvr_bigendian64 (u) };
            mem.u = kvr_bigendian64 (u);
            return ctx.read_float (mem.f);
#else
            double f = 0;
            uint64_t u64 = kvr_bigendian64 (u);
            KVR_ASSERT (sizeof (f) == sizeof (u64));
            memcpy (&f, &u64, sizeof (u64));
            return ctx.read_float (f);
#endif
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_array5 (istr *is, read_ctx &ctx, uint8_t data)
        {
          uint8_t alen = (data & 0x0f);
          bool ok = ctx.read_array_start (alen);
          for (uint8_t i = 0; ok && (i < alen); ++i)
          {
            ok &= parse (is, ctx);
          }
#if KVR_DEBUG
          ok = ok && ctx.read_array_end (alen);
#endif
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_array8 (istr *is, read_ctx &ctx)
        {
          bool ok = false;
          uint8_t alen = 0;
          if (is->get (&alen))
          {
            ok = ctx.read_array_start (alen);
            for (uint8_t i = 0; ok && (i < alen); ++i)
            {
              ok &= parse (is, ctx);
            }
#if KVR_DEBUG
            ok &= ctx.read_array_end (alen);
#endif
          }
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_array16 (istr *is, read_ctx &ctx)
        {
          bool ok = false;
          uint16_t len = 0;
          if (is->read ((uint8_t *) &len, 2))
          {
            uint16_t alen = kvr_bigendian16 (len);
            ok = ctx.read_array_start (static_cast<kvr::sz_t>(alen));
            for (uint16_t i = 0; ok && (i < alen); ++i)
            {
              ok &= parse (is, ctx);
            }
#if KVR_DEBUG
            ok &= ctx.read_array_end (static_cast<kvr::sz_t>(alen));
#endif
          }
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_array32 (istr *is, read_ctx &ctx)
        {
          bool ok = false;
          uint32_t len = 0;
          if (is->read ((uint8_t *) &len, 4))
          {
            uint32_t alen = kvr_bigendian32 (len);
            ok = ctx.read_array_start (static_cast<kvr::sz_t>(alen));
            for (uint32_t i = 0; ok && (i < alen); ++i)
            {
              ok &= parse (is, ctx);
            }
#if KVR_DEBUG
            ok &= ctx.read_array_end (static_cast<kvr::sz_t>(alen));
#endif
          }
          return ok;
        }
        
        ////////////////////////////////////////////////////////////

        bool parse_map5 (istr *is, read_ctx &ctx, uint8_t data)
        {
          uint8_t msz = (data & 0x0f);
          bool ok = ctx.read_map_start (msz);
          for (uint8_t i = 0; ok && (i < msz); ++i)
          {
            ok &= parse_key (is, ctx);
            ok &= parse (is, ctx);
          }
#if KVR_DEBUG
          ok &= ctx.read_map_end (msz);
#endif
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_map8 (istr *is, read_ctx &ctx)
        {
          bool ok = false;
          uint8_t msz = 0;
          if (is->get (&msz))
          {
            ok = ctx.read_map_start (msz);
            for (uint8_t i = 0; ok && (i < msz); ++i)
            {
              ok &= parse_key (is, ctx);
              ok &= parse (is, ctx);
            }
#if KVR_DEBUG
            ok &= ctx.read_map_end (msz);
#endif
          }
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_map16 (istr *is, read_ctx &ctx)
        {
          bool ok = false;
          uint16_t len = 0;
          if (is->read ((uint8_t *) &len, 2))
          {
            uint16_t msz = kvr_bigendian16 (len);
            ok = ctx.read_map_start (static_cast<kvr::sz_t>(msz));
            for (uint16_t i = 0; ok && (i < msz); ++i)
            {
              ok &= parse_key (is, ctx);
              ok &= parse (is, ctx);
            }
#if KVR_DEBUG
            ok &= ctx.read_map_end (static_cast<kvr::sz_t>(msz));
#endif
          }
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_map32 (istr *is, read_ctx &ctx)
        {
          bool ok = false;
          uint32_t len = 0;
          if (is->read ((uint8_t *) &len, 4))
          {
            uint32_t msz = kvr_bigendian32 (len);
            ok = ctx.read_map_start (static_cast<kvr::sz_t>(msz));
            for (uint32_t i = 0; ok && (i < msz); ++i)
            {
              ok &= parse_key (is, ctx);
              ok &= parse (is, ctx);
            }
#if KVR_DEBUG
            ok &= ctx.read_map_end (static_cast<kvr::sz_t>(msz));
#endif
          }
          return ok;
        }

        ////////////////////////////////////////////////////////////

        bool parse_string5 (istr *is, read_ctx &ctx, uint8_t data)
        {
          uint8_t slen = (data & 0x1f);
          //char str [32];
          m_ss.seek (0);
          uint8_t *str = m_ss.push (slen + 1);
          KVR_ASSERT (str);
          if (is->read (str, slen))
          {
            str [slen] = 0;
            return ctx.read_string (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
          }
          return false;      
        }

        ////////////////////////////////////////////////////////////

        bool parse_string8 (istr *is, read_ctx &ctx)
        {
          uint8_t slen = 0;      
          if (is->get (&slen))
          {
            m_ss.seek (0);
            uint8_t *str = m_ss.push (slen + 1);
            KVR_ASSERT (str);
            if (is->read (str, slen))
            {
              str [slen] = 0;
              return ctx.read_string (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
            }
          }      
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_string16 (istr *is, read_ctx &ctx)
        {
          uint16_t len = 0;
          if (is->read ((uint8_t *) &len, 2))
          {
            m_ss.seek (0);
            uint16_t slen = kvr_bigendian16 (len);
            uint8_t *str = m_ss.push (slen + 1);
            KVR_ASSERT (str);
            if (is->read (str, slen))
            {
              str [slen] = 0;
              return ctx.read_string (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
            }
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_string32 (istr *is, read_ctx &ctx)
        {
          uint32_t len = 0;
          if (is->read ((uint8_t *) &len, 4))
          {
            m_ss.seek (0);
            uint32_t slen = kvr_bigendian32 (len);
            uint8_t *str = m_ss.push (slen + 1);
            KVR_ASSERT (str);
            if (is->read (str, slen))
            {
              str [slen] = 0;
              return ctx.read_string (reinterpret_cast<const char *>(str), static_cast<kvr::sz_t>(slen));
            }
          }
          return false;
        }
    
        ////////////////////////////////////////////////////////////

        bool parse_unsigned5 (read_ctx &ctx, uint8_t data)
        {
          return ctx.read_integer (data);
        }

        ////////////////////////////////////////////////////////////

        bool parse_unsigned8 (istr *is, read_ctx &ctx)
        {
          uint8_t i = 0;
          if (is->get (&i))
          {
            return ctx.read_integer (i);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_unsigned16 (istr *is, read_ctx &ctx)
        {
          uint16_t u16 = 0;
          if (is->read ((uint8_t *) &u16, 2))
          {
            uint16_t i = kvr_bigendian16 (u16);
            return ctx.read_integer (i);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_unsigned32 (istr *is, read_ctx &ctx)
        {
          uint32_t u32 = 0;
          if (is->read ((uint8_t *) &u32, 4))
          {
            uint32_t i = kvr_bigendian32 (u32);
            return ctx.read_integer (i);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_unsigned64 (istr *is, read_ctx &ctx)
        {
          uint64_t u64 = 0;
          if (is->read ((uint8_t *) &u64, 8))
          {
            uint64_t i = kvr_bigendian64 (u64);
            const uint64_t imax = std::numeric_limits<int64_t>::max ();
            if (i < imax)
            {
              return ctx.read_integer ((int64_t) i);
            }
            else
            {
              KVR_ASSERT (false && "not supported");
            }
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_negint5 (read_ctx &ctx, uint8_t data)
        {
          int64_t ni = -1 - (int64_t) data;
          return ctx.read_integer (ni);
        }

        ////////////////////////////////////////////////////////////

        bool parse_negint8 (istr *is, read_ctx &ctx)
        {
          uint8_t u8 = 0;
          if (is->get (&u8))
          {
            int64_t ni = -1 - (int64_t) u8;
            return ctx.read_integer (ni);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_negint16 (istr *is, read_ctx &ctx)
        {
          uint16_t u16 = 0;
          if (is->read ((uint8_t *) &u16, 2))
          {
            uint16_t u = kvr_bigendian16 (u16);
            int64_t ni = -1 - (int64_t) u;
            return ctx.read_integer (ni);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_negint32 (istr *is, read_ctx &ctx)
        {
          uint32_t u32 = 0;
          if (is->read ((uint8_t *) &u32, 4))
          {
            uint32_t u = kvr_bigendian32 (u32);
            int64_t ni = -1 - (int64_t) u;
            return ctx.read_integer (ni);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        bool parse_negint64 (istr *is, read_ctx &ctx)
        {
          uint64_t u64 = 0;
          if (is->read ((uint8_t *) &u64, 8))
          {
            uint64_t u = kvr_bigendian64 (u64);
            int64_t ni = -1 - (int64_t) u;
            return ctx.read_integer (ni);
          }
          return false;
        }

        ////////////////////////////////////////////////////////////

        kvr::mem_ostream m_ss;
      };

      template<>
      bool reader<kvr::mem_istream>::parse_string5 (kvr::mem_istream *is, read_ctx &ctx, uint8_t data)
      {
        uint8_t slen = (data & 0x1f);
        const char *str = (const char *) is->push (slen);
        return str ? ctx.read_string (str, slen) : false;
      }

      template<>
      bool reader<kvr::mem_istream>::parse_string8 (kvr::mem_istream *is, read_ctx &ctx)
      {
        uint8_t slen = 0;
        if (is->get (&slen))
        {
          const char *str = (const char *) is->push (slen);
          return str ? ctx.read_string (str, slen) : false;
        }
        return false;
      }

      template<>
      bool reader<kvr::mem_istream>::parse_string16 (kvr::mem_istream *is, read_ctx &ctx)
      {
        uint16_t len = 0;
        if (is->read ((uint8_t *) &len, 2))
        {
          uint16_t slen = kvr_bigendian16 (len);
          const char *str = (const char *) is->push (slen);
          return str ? ctx.read_string (str, slen) : false;
        }
        return false;
      }

      template<>
      bool reader<kvr::mem_istream>::parse_string32 (kvr::mem_istream *is, read_ctx &ctx)
      {    
        uint32_t len = 0;
        if (is->read ((uint8_t *) &len, 4))
        {
          uint32_t slen = kvr_bigendian32 (len);
          const char *str = (const char *) is->push (slen);
          return str ? ctx.read_key (str, static_cast<kvr::sz_t>(slen)) : false;
        }
        return false;
      }
    #if KVR_CBOR_READ_KEY_SPECIALIZATION
      template<>
      bool reader<kvr::mem_istream>::parse_key5 (kvr::mem_istream *is, read_ctx &ctx, uint8_t data)
      {
        uint8_t slen = (data & 0x1f);
        const char *str = (const char *) is->push (slen);
        return str ? ctx.read_key (str, slen) : false;
      }

      template<>
      bool reader<kvr::mem_istream>::parse_key8 (kvr::mem_istream *is, read_ctx &ctx)
      {
        uint8_t slen = 0;
        if (is->get (&slen))
        {
          const char *str = (const char *) is->push (slen);
          return str ? ctx.read_key (str, slen) : false;
        }
        return false;
      }

      template<>
      bool reader<kvr::mem_istream>::parse_key16 (kvr::mem_istream *is, read_ctx &ctx)
      {
        uint16_t len = 0;
        if (is->read ((uint8_t *) &len, 2))
        {
          uint16_t slen = kvr_bigendian16 (len);
          const char *str = (const char *) is->push (slen);
          return str ? ctx.read_key (str, slen) : false;
        }
        return false;
      }

      template<>
      bool reader<kvr::mem_istream>::parse_key32 (kvr::mem_istream *is, read_ctx &ctx)
      {
        uint32_t len = 0;
        if (is->read ((uint8_t *) &len, 4))
        {
          uint32_t slen = kvr_bigendian32 (len);
          const char *str = (const char *) is->push (slen);
          return str ? ctx.read_key (str, slen) : false;
        }
        return false;
      }
    #endif
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      template<typename ostr>
      struct write_ctx
      {
        ////////////////////////////////////////////////////////////

        write_ctx (ostr *os) : m_os (os)
        {
          KVR_ASSERT (os);
        }

        ////////////////////////////////////////////////////////////

        bool write_null ()
        {
          m_os->put (CBOR_MAJOR_TYPE_7 | CBOR_VALUE_TYPE_NULL);
          return true;
        }

        ////////////////////////////////////////////////////////////

        bool write_array (kvr::sz_t size)
        {
          if (size < CBOR_VALUE_TYPE_UINT8)
          {
            uint8_t sz = (uint8_t) size;
            m_os->put (CBOR_MAJOR_TYPE_4 | sz);
          }
          else if (size < 0xff)
          {
            uint8_t sz = (uint8_t) size;
            m_os->put (CBOR_MAJOR_TYPE_4 | CBOR_VALUE_TYPE_UINT8);
            m_os->put (sz);
          }
          else if (size <= 0xffff)
          {
            uint16_t sz = kvr_bigendian16 (size);
            m_os->put (CBOR_MAJOR_TYPE_4 | CBOR_VALUE_TYPE_UINT16);
            m_os->write ((uint8_t *) &sz, 2);
          }
          else if (size <= 0xffffffff)
          {
            uint32_t sz = kvr_bigendian32 (size);
            m_os->put (CBOR_MAJOR_TYPE_4 | CBOR_VALUE_TYPE_UINT32);
            m_os->write ((uint8_t *) &sz, 4);
          }
          else
          {
            return false;
          }

          return true;
        }

        ////////////////////////////////////////////////////////////

        bool write_map (kvr::sz_t size)
        {
          if (size < CBOR_VALUE_TYPE_UINT8)
          {
            uint8_t sz = (uint8_t) size;
            m_os->put (CBOR_MAJOR_TYPE_5 | sz);
          }
          else if (size < 0xff)
          {
            uint8_t sz = (uint8_t) size;
            m_os->put (CBOR_MAJOR_TYPE_5 | CBOR_VALUE_TYPE_UINT8);
            m_os->put (sz);
          }
          else if (size <= 0xffff)
          {
            uint16_t sz = kvr_bigendian16 (size);
            m_os->put (CBOR_MAJOR_TYPE_5 | CBOR_VALUE_TYPE_UINT16);
            m_os->write ((uint8_t *) &sz, 2);
          }
          else if (size <= 0xffffffff)
          {
            uint32_t sz = kvr_bigendian32 (size);
            m_os->put (CBOR_MAJOR_TYPE_5 | CBOR_VALUE_TYPE_UINT32);
            m_os->write ((uint8_t *) &sz, 4);
          }
          else
          {
            return false;
          }

          return true;
        }

        ////////////////////////////////////////////////////////////

        bool write_string (const char *str, kvr::sz_t slen)
        {
          if (slen < CBOR_VALUE_TYPE_UINT8)
          {
            uint8_t len = (uint8_t) slen;
            m_os->put (CBOR_MAJOR_TYPE_3 | len);
          }
          else if (slen < 0xff)
          {
            uint8_t len = (uint8_t) slen;
            m_os->put (CBOR_MAJOR_TYPE_3 | CBOR_VALUE_TYPE_UINT8);
            m_os->put (len);
          }
          else if (slen <= 0xffff)
          {
            uint16_t len = kvr_bigendian16 (slen);
            m_os->put (CBOR_MAJOR_TYPE_3 | CBOR_VALUE_TYPE_UINT16);
            m_os->write ((uint8_t *) &len, 2);
          }
          else if (slen <= 0xffffffff)
          {
            uint32_t len = kvr_bigendian32 (slen);
            m_os->put (CBOR_MAJOR_TYPE_3 | CBOR_VALUE_TYPE_UINT32);
            m_os->write ((uint8_t *) &len, 4);
          }
          else
          {
            return false;
          }

          m_os->write ((uint8_t *) str, (size_t) slen);

          return true;
        }

        ////////////////////////////////////////////////////////////

        bool write_integer (int64_t i64)
        {
          if (i64 >= 0) // unsigned
          {
            const uint64_t ui64 = (uint64_t) i64;

            if (ui64 < CBOR_VALUE_TYPE_UINT8)
            {
              uint8_t i = static_cast<uint8_t>(ui64);
              m_os->put (CBOR_MAJOR_TYPE_0 | i);
            }
            else if (ui64 <= 0xff)
            {
              uint8_t i = static_cast<uint8_t>(ui64);
              m_os->put (CBOR_MAJOR_TYPE_0 | CBOR_VALUE_TYPE_UINT8);
              m_os->put (i);
            }
            else if (ui64 <= 0xffff)
            {
              uint16_t i = kvr_bigendian16 (ui64);
              m_os->put (CBOR_MAJOR_TYPE_0 | CBOR_VALUE_TYPE_UINT16);
              m_os->write ((uint8_t *) &i, 2);
            }
            else if (ui64 <= 0xffffffff)
            {
              uint32_t i = kvr_bigendian32 (ui64);
              m_os->put (CBOR_MAJOR_TYPE_0 | CBOR_VALUE_TYPE_UINT32);
              m_os->write ((uint8_t *) &i, 4);
            }
            else // max is int64_t;
            {
              uint64_t i = kvr_bigendian64 (i64);
              m_os->put (CBOR_MAJOR_TYPE_0 | CBOR_VALUE_TYPE_UINT64);
              m_os->write ((uint8_t *) &i, 8);
            }
          }
          else // signed
          {
            const int64_t ni64 = -1 - i64;

            if (ni64 < CBOR_VALUE_TYPE_UINT8)
            {
              uint8_t i = static_cast<uint8_t>(ni64);
              m_os->put (CBOR_MAJOR_TYPE_1 | i);
            }
            else if (ni64 <= 0xff)
            {
              uint8_t i = static_cast<uint8_t>(ni64);
              m_os->put (CBOR_MAJOR_TYPE_1 | CBOR_VALUE_TYPE_UINT8);
              m_os->put (i);
            }
            else if (ni64 <= 0xffff)
            {
              uint16_t i = kvr_bigendian16 (ni64);
              m_os->put (CBOR_MAJOR_TYPE_1 | CBOR_VALUE_TYPE_UINT16);              
              m_os->write ((uint8_t *) &i, 2);
            }
            else if (ni64 <= 0xffffffff)
            {
              uint32_t i = kvr_bigendian32 (ni64);
              m_os->put (CBOR_MAJOR_TYPE_1 | CBOR_VALUE_TYPE_UINT32);              
              m_os->write ((uint8_t *) &i, 4);
            }
            else
            {
              uint64_t i = kvr_bigendian64 (ni64);
              m_os->put (CBOR_MAJOR_TYPE_1 | CBOR_VALUE_TYPE_UINT64);              
              m_os->write ((uint8_t *) &i, 8);
            }
          }

          return true;
        }

        ////////////////////////////////////////////////////////////

        bool write_float (double f)
        {
#if KVR_OPTIMIZATION_COMPACT_CODEC_FP_PRECISION_ON || KVR_CBOR_WRITE_COMPACT_FP_OVERRIDE
          uint16_t hf = 0;
          if (kvr::internal::fp_single_to_half (f, &hf))
          {
            uint16_t fi = kvr_bigendian16 (hf);
            m_os->put (CBOR_MAJOR_TYPE_7 | CBOR_VALUE_TYPE_FLOAT16);
            m_os->write ((uint8_t *) &fi, 2);
            return true;
          }
          else
          {
            const double fmin = std::numeric_limits<float>::min ();
            const double fmax = std::numeric_limits<float>::max ();
            if ((f >= fmin) && (f <= fmax))
            {
#if KVR_INTERNAL_FLAG_DEBUG_TYPE_PUNNING_ON
              union { float f; uint32_t u; } mem;
              mem.f = (float) f;
              uint32_t f32 = kvr_bigendian32 (mem.u);
#else
              float ff = (float) f;
              uint32_t u = 0;
              memcpy (&u, &ff, sizeof (ff));
              uint32_t f32 = kvr_bigendian32 (u);
#endif
              m_os->put (CBOR_MAJOR_TYPE_7 | CBOR_VALUE_TYPE_FLOAT32);
              m_os->write ((uint8_t *) &f32, 4);
              return true;
            }
          }
#endif
          
#if KVR_INTERNAL_FLAG_DEBUG_TYPE_PUNNING_ON
          union { double f; uint64_t u; } mem;
          mem.f = f;
          uint64_t f64 = kvr_bigendian64 (mem.u);
#else
          uint64_t u = 0;
          memcpy (&u, &f, sizeof (f));
          uint64_t f64 = kvr_bigendian64 (u);
#endif
          m_os->put (CBOR_MAJOR_TYPE_7 | CBOR_VALUE_TYPE_FLOAT64);
          m_os->write ((uint8_t *) &f64, 8);
          return true;
        }

        ////////////////////////////////////////////////////////////

        bool write_boolean (bool b)
        {
          m_os->put (CBOR_MAJOR_TYPE_7 | (b ? CBOR_VALUE_TYPE_TRUE : CBOR_VALUE_TYPE_FALSE));
          return true;
        }

        ////////////////////////////////////////////////////////////

        ostr *m_os;
      };

      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      template<typename ostr>
      struct writer
      {
        bool print (const kvr::value *val, write_ctx<ostr> &ctx)
        {
          KVR_ASSERT (val);

          bool success = false;

          if (val->is_map ())
          {
            kvr::sz_t msz = val->size ();
            bool ok = ctx.write_map (msz);
            kvr::value::cursor c (val);
            kvr::pair p;
            while (ok && c.get (&p))
            {
              kvr::key *k = p.get_key ();
              ok &= ctx.write_string (k->get_string (), k->get_length ());

              kvr::value *v = p.get_value ();          
              ok &= print (v, ctx);
            }
            success = ok;
          }

          else if (val->is_array ())
          {
            kvr::sz_t alen = val->length ();
            bool ok = ctx.write_array (alen);
            for (kvr::sz_t i = 0; (i < alen) && ok; ++i)
            {
              kvr::value *v = val->element (i);
              ok &= print (v, ctx);
            }
            success = ok;
          }

          else if (val->is_string ())
          {
            kvr::sz_t slen = 0;
            const char *str = val->get_string (&slen);
            success = ctx.write_string (str, slen);
          }

          else if (val->is_integer ())
          {
            int64_t n = val->get_integer ();
            success = ctx.write_integer (n);
          }

          else if (val->is_float ())
          {
            double n = val->get_float ();
            success = ctx.write_float (n);
          }

          else if (val->is_boolean ())
          {
            bool b = val->get_boolean ();
            success = ctx.write_boolean (b);
          }

          else if (val->is_null ())
          {
            success = ctx.write_null ();
          }

          return success;
        }
      };

      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////////////////////

      bool read (kvr::value *dest, kvr::istream &istr)
      {
        KVR_ASSERT (dest);

        reader<kvr::istream> reader;
        read_ctx ctx (dest);
        return reader.parse (&istr, ctx);
      }

      ////////////////////////////////////////////////////////////

      bool write (const kvr::value *src, kvr::ostream *ostr)
      {
        KVR_ASSERT (src);
        KVR_ASSERT (ostr);
    
        write_ctx<kvr::ostream> ctx (ostr);
        writer<kvr::ostream> wrt;
        
        if (wrt.print (src, ctx))
        {
          ostr->flush ();
          return true;
        }

        return false;
      }

      ////////////////////////////////////////////////////////////

      bool read (kvr::value *dest, kvr::mem_istream &istr)
      {
        KVR_ASSERT (dest);

        reader<kvr::mem_istream> reader;
        read_ctx ctx (dest);
        return reader.parse (&istr, ctx);
      }

      ////////////////////////////////////////////////////////////

      bool write (const kvr::value *src, kvr::mem_ostream *ostr)
      {
        KVR_ASSERT (src);
        KVR_ASSERT (ostr);

        write_ctx<kvr::mem_ostream> ctx (ostr);
        writer<kvr::mem_ostream> wrt;

        if (wrt.print (src, ctx))
        {
          ostr->flush ();
          return true;
        }

        return false;
      }

      ////////////////////////////////////////////////////////////

      size_t write_approx_size (const kvr::value *val)
      {
        size_t size = 0;

        if (val->is_map ())
        {
          kvr::sz_t msz = val->size ();

          if (msz < CBOR_VALUE_TYPE_UINT8)
          {
            size += 1;
          }
          else if (msz < 0xff)
          {
            size += 2;
          }
          else if (msz <= 0xffff)
          {
            size += 3;
          }
          else
          {
            size += 5;
          }

          kvr::value::cursor c (val);
          kvr::pair p;
          while (c.get (&p))
          {
            kvr::key *k = p.get_key ();
            kvr::value *v = p.get_value ();
            kvr::sz_t klen = k->get_length ();

            if (klen < CBOR_VALUE_TYPE_UINT8)
            {
              size += 1;
            }
            else if (klen < 0xff)
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

        else if (val->is_array ())
        {
          kvr::sz_t alen = val->length ();

          if (alen < CBOR_VALUE_TYPE_UINT8)
          {
            size += 1;
          }
          else if (alen < 0xff)
          {
            size += 2;
          }
          else if (alen <= 0xffff)
          {
            size += 3;
          }
          else
          {
            size += 5;
          }

          for (kvr::sz_t i = 0, c = val->length (); i < c; ++i)
          {
            kvr::value *v = val->element (i);
            size += write_approx_size (v);
          }
        }

        else if (val->is_string ())
        {
          kvr::sz_t slen = 0;
          const char *str = val->get_string (&slen);
          KVR_REF_UNUSED (str);

          if (slen < CBOR_VALUE_TYPE_UINT8)
          {
            size += 1;
          }
          else if (slen < 0xff)
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

        else if (val->is_integer ())
        {
          int64_t n = val->get_integer ();
          int64_t n64 = (n >= 0) ? n : (-1 - n);

          if (n64 < CBOR_VALUE_TYPE_UINT8)
          {
            size += 1;
          }
          else if (n64 < 0xff)
          {
            size += 2;
          }
          else if (n64 <= 0xffff)
          {
            size += 3;
          }
          else if (n64 <= 0xffffffff)
          {
            size += 5;
          }
          else
          {
            size += 9;
          }
        }

        else if (val->is_float ())
        {
#if KVR_OPTIMIZATION_COMPACT_CODEC_FP_PRECISION_ON || KVR_CBOR_WRITE_COMPACT_FP_OVERRIDE
          double f = val->get_float ();
          uint16_t hf = 0;
          if (kvr::internal::fp_single_to_half (f, &hf))
          {
            size += 3;
          }
          else
          {
            const double fmin = std::numeric_limits<float>::min ();
            const double fmax = std::numeric_limits<float>::max ();

            if ((f >= fmin) && (f <= fmax))
            {
              size += 5;
            }
            else
            {
              size += 9;
            }
          }
#else
          size += 9;
#endif
        }

        else if (val->is_boolean ())
        {
          size += 1;
        }

        else if (val->is_null ())
        {
          size += 1;
        }

        return size;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
