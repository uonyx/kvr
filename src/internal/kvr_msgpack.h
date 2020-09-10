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

#ifndef KVR_MSGPACK_H
#define KVR_MSGPACK_H

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define KVR_MSGPACK_READ_KEY_SPECIALIZATION     0
#define KVR_MSGPACK_WRITE_COMPACT_FP_OVERRIDE   0

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace kvr
{
    namespace internal
    {
        namespace msgpack
        {
            /////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            
            // Specification: https://github.com/msgpack/msgpack/blob/master/spec.md
            
            /////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            
            // jump table
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
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_boolean ();
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
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_integer ();
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
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_float ();
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
                        
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_string ();
#endif
                        m_temp->set_string (str, length);
                        m_temp = NULL;
                        success = true;
                    }
                    else if (node->is_array ())
                    {
                        kvr::value *vstr = node->push_null (); KVR_ASSERT (vstr);
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        vstr = vstr->as_string ();
#endif
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
                            KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
                            node = m_temp->as_map (size);
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
                        node = m_root->as_map (size);
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
#if KVR_MSGPACK_READ_KEY_SPECIALIZATION
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
                            KVR_ASSERT_SAFE (m_temp && m_temp->is_null (), false);
                            node = m_temp->as_array (length);
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
                        node = m_root->as_array (length);
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
                        switch (curr)
                        {
                            case MSGPACK_HEADER_NULL:         { success = parse_null (ctx); break; }
                            case MSGPACK_HEADER_BOOL_FALSE:   { success = parse_bool (ctx, false); break; }
                            case MSGPACK_HEADER_BOOL_TRUE:    { success = parse_bool (ctx, true); break; }
                            case MSGPACK_HEADER_FLOAT_32:     { success = parse_float32 (is, ctx); break; }
                            case MSGPACK_HEADER_FLOAT_64:     { success = parse_float64 (is, ctx); break; }
                            case MSGPACK_HEADER_STRING_8:     { success = parse_string8 (is, ctx); break; }
                            case MSGPACK_HEADER_STRING_16:    { success = parse_string16 (is, ctx); break; }
                            case MSGPACK_HEADER_STRING_32:    { success = parse_string32 (is, ctx); break; }
                            case MSGPACK_HEADER_ARRAY_16:     { success = parse_array16 (is, ctx); break; }
                            case MSGPACK_HEADER_ARRAY_32:     { success = parse_array32 (is, ctx); break; }
                            case MSGPACK_HEADER_MAP_16:       { success = parse_map16 (is, ctx); break; }
                            case MSGPACK_HEADER_MAP_32:       { success = parse_map32 (is, ctx); break; }
                            case MSGPACK_HEADER_UNSIGNED_8:   { success = parse_unsigned8 (is, ctx); break; }
                            case MSGPACK_HEADER_UNSIGNED_16:  { success = parse_unsigned16 (is, ctx); break; }
                            case MSGPACK_HEADER_UNSIGNED_32:  { success = parse_unsigned32 (is, ctx); break; }
                            case MSGPACK_HEADER_UNSIGNED_64:  { success = parse_unsigned64 (is, ctx); break; }
                            case MSGPACK_HEADER_SIGNED_8:     { success = parse_signed8 (is, ctx); break; }
                            case MSGPACK_HEADER_SIGNED_16:    { success = parse_signed16 (is, ctx); break; }
                            case MSGPACK_HEADER_SIGNED_32:    { success = parse_signed32 (is, ctx); break; }
                            case MSGPACK_HEADER_SIGNED_64:    { success = parse_signed64 (is, ctx); break; }
                            default:
                            {
                                if (curr <= 127)                                    { success = parse_unsigned7 (ctx, curr); }
                                else if ((curr & 0xe0) == MSGPACK_HEADER_SIGNED_5)  { success = parse_signed5 (ctx, curr); }
                                else if ((curr & 0xe0) == MSGPACK_HEADER_STRING_5)  { success = parse_string5 (is, ctx, curr); }
                                else
                                {
                                    uint8_t hi4 = (curr & 0xf0);
                                    if (hi4 == MSGPACK_HEADER_MAP_4)                  { success = parse_map4 (is, ctx, curr); }
                                    else if (hi4 == MSGPACK_HEADER_ARRAY_4)           { success = parse_array4 (is, ctx, curr); }
                                }
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
                        switch (curr)
                        {
                            case MSGPACK_HEADER_STRING_8:   { success = parse_key8 (is, ctx); break; }
                            case MSGPACK_HEADER_STRING_16:  { success = parse_key16 (is, ctx); break; }
                            case MSGPACK_HEADER_STRING_32:  { success = parse_key32 (is, ctx); break; }
                            default:
                            {
                                if ((curr & 0xe0) == MSGPACK_HEADER_STRING_5) { success = parse_key5 (is, ctx, curr); }
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
                
                bool parse_array4 (istr *is, read_ctx &ctx, uint8_t data)
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
                
                bool parse_map4 (istr *is, read_ctx &ctx, uint8_t data)
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
                    //KVR_ASSERT (slen < 32);
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
                
                bool parse_unsigned7 (read_ctx &ctx, uint8_t data)
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
#if 1
                    KVR_ASSERT (false && "not supported");
                    KVR_REF_UNUSED (is);
                    KVR_REF_UNUSED (ctx);
                    return false;
#else
                    uint64_t u64 = 0;
                    if (is->get ((uint8_t *) &u64, 8))
                    {
                        uint64_t i = kvr_bigendian64 (u64);
                        return ctx._read_uint64 (i);
                    }
                    return false;
#endif
                }
                
                ////////////////////////////////////////////////////////////
                
                bool parse_signed5 (read_ctx &ctx, uint8_t data)
                {
                    int8_t i = (int8_t) data;
                    return ctx.read_integer (i);
                }
                
                ////////////////////////////////////////////////////////////
                
                bool parse_signed8 (istr *is, read_ctx &ctx)
                {
                    uint8_t u8 = 0;
                    if (is->get (&u8))
                    {
                        int8_t i = (int8_t) u8;
                        return ctx.read_integer (i);
                    }
                    return false;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool parse_signed16 (istr *is, read_ctx &ctx)
                {
                    uint16_t u16 = 0;
                    if (is->read ((uint8_t *) &u16, 2))
                    {
                        uint16_t u = kvr_bigendian16 (u16);
                        int16_t i = (int16_t) u;
                        return ctx.read_integer (i);
                    }
                    return false;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool parse_signed32 (istr *is, read_ctx &ctx)
                {
                    uint32_t u32 = 0;
                    if (is->read ((uint8_t *) &u32, 4))
                    {
                        uint32_t u = kvr_bigendian32 (u32);
                        int32_t i = (int32_t) u;
                        return ctx.read_integer (i);
                    }
                    return false;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool parse_signed64 (istr *is, read_ctx &ctx)
                {
                    uint64_t u64 = 0;
                    if (is->read ((uint8_t *) &u64, 8))
                    {
                        uint64_t u = kvr_bigendian64 (u64);
                        int64_t i = (int64_t) u;
                        return ctx.read_integer (i);
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
#if KVR_MSGPACK_READ_KEY_SPECIALIZATION
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
                    m_os->put (MSGPACK_HEADER_NULL);
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool write_array (kvr::sz_t size)
                {
                    if (size <= 15)
                    {
                        uint8_t sz = static_cast<uint8_t>(size);
                        m_os->put (MSGPACK_HEADER_ARRAY_4 | sz);
                    }
                    else if (size <= 0xffff)
                    {
                        uint16_t sz = kvr_bigendian16 (size);
                        m_os->put (MSGPACK_HEADER_ARRAY_16);
                        m_os->write ((uint8_t *) &sz, 2);
                    }
                    else if (size <= 0xffffffff)
                    {
                        uint32_t sz = kvr_bigendian32 (size);
                        m_os->put (MSGPACK_HEADER_ARRAY_32);
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
                    if (size <= 15)
                    {
                        uint8_t sz = static_cast<uint8_t>(size);
                        m_os->put (MSGPACK_HEADER_MAP_4 | sz);
                    }
                    else if (size <= 0xffff)
                    {
                        uint16_t sz = kvr_bigendian16 (size);
                        m_os->put (MSGPACK_HEADER_MAP_16);
                        m_os->write ((uint8_t *) &sz, 2);
                    }
                    else if (size <= 0xffffffff)
                    {
                        uint32_t sz = kvr_bigendian32 (size);
                        m_os->put (MSGPACK_HEADER_MAP_32);
                        m_os->write ((uint8_t *) &sz, 4);
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool write_string (const char *str, kvr::sz_t slen)
                {
                    if (slen <= 31)
                    {
                        uint8_t len = static_cast<uint8_t>(slen);
                        m_os->put (MSGPACK_HEADER_STRING_5 | len);
                    }
                    else if (slen <= 0xff)
                    {
                        uint8_t len = static_cast<uint8_t>(slen);
                        m_os->put (MSGPACK_HEADER_STRING_8);
                        m_os->put (len);
                    }
                    else if (slen <= 0xffff)
                    {
                        uint16_t len = kvr_bigendian16 (slen);
                        m_os->put (MSGPACK_HEADER_STRING_16);
                        m_os->write ((uint8_t *) &len, 2);
                    }
                    else if (slen <= 0xffffffff)
                    {
                        uint32_t len = kvr_bigendian32 (slen);
                        m_os->put (MSGPACK_HEADER_STRING_32);
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
                        if (i64 <= 127)
                        {
                            uint8_t i = static_cast<uint8_t>(i64);
                            m_os->put (i);
                        }
                        else if (i64 <= 0xff)
                        {
                            uint8_t i = static_cast<uint8_t>(i64);
                            m_os->put (MSGPACK_HEADER_UNSIGNED_8);
                            m_os->put (i);
                        }
                        else if (i64 <= 0xffff)
                        {
                            uint16_t i = kvr_bigendian16 (i64);
                            m_os->put (MSGPACK_HEADER_UNSIGNED_16);
                            m_os->write ((uint8_t *) &i, 2);
                        }
                        else if (i64 <= 0xffffffff)
                        {
                            uint32_t i = kvr_bigendian32 (i64);
                            m_os->put (MSGPACK_HEADER_UNSIGNED_32);
                            m_os->write ((uint8_t *) &i, 4);
                        }
                        else // max is int64_t;
                        {
                            uint64_t i = kvr_bigendian64 (i64);
                            m_os->put (MSGPACK_HEADER_SIGNED_64);
                            m_os->write ((uint8_t *) &i, 8);
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
                            uint8_t i = static_cast<uint8_t>(i64);
                            m_os->put (MSGPACK_HEADER_SIGNED_5 | i);
                        }
                        else if (i64 >= nint8)
                        {
                            uint8_t i = static_cast<uint8_t>(i64);
                            m_os->put (MSGPACK_HEADER_SIGNED_8);
                            m_os->put (i);
                        }
                        else if (i64 >= nint16)
                        {
                            uint16_t i = kvr_bigendian16 (i64);
                            m_os->put (MSGPACK_HEADER_SIGNED_16);
                            m_os->write ((uint8_t *) &i, 2);
                        }
                        else if (i64 >= nint32)
                        {
                            uint32_t i = kvr_bigendian32 (i64);
                            m_os->put (MSGPACK_HEADER_SIGNED_32);
                            m_os->write ((uint8_t *) &i, 4);
                        }
                        else
                        {
                            uint64_t i = kvr_bigendian64 (i64);
                            m_os->put (MSGPACK_HEADER_SIGNED_64);
                            m_os->write ((uint8_t *) &i, 8);
                        }
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool write_float (double f)
                {
#if KVR_FLAG_ENCODE_COMPACT_FP_PRECISION || KVR_MSGPACK_WRITE_COMPACT_FP_OVERRIDE
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
                        m_os->put (MSGPACK_HEADER_FLOAT_32);
                        m_os->write ((uint8_t *) &f32, 4);
                        return true;
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
                    m_os->put (MSGPACK_HEADER_FLOAT_64);
                    m_os->write ((uint8_t *) &f64, 8);
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool write_boolean (bool b)
                {
                    m_os->put (b ? MSGPACK_HEADER_BOOL_TRUE : MSGPACK_HEADER_BOOL_FALSE);
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
                    
                    kvr::value::cursor c (val);
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
                
                else if (val->is_array ())
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
                
                else if (val->is_string ())
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
                
                else if (val->is_integer ())
                {
                    int64_t n = val->get_integer ();
                    
                    if (n >= 0) // unsigned
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
                
                else if (val->is_float ())
                {
#if KVR_FLAG_ENCODE_COMPACT_FP_PRECISION || KVR_MSGPACK_WRITE_COMPACT_FP_OVERRIDE
                    const double fmin = std::numeric_limits<float>::min ();
                    const double fmax = std::numeric_limits<float>::max ();
                    
                    double n = val->get_float ();
                    if ((n >= fmin) && (n <= fmax))
                    {
                        size += 5;
                    }
                    else
#endif
                    {
                        size += 9;
                    }
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
