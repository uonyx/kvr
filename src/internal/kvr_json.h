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

#ifndef KVR_JSON_H
#define KVR_JSON_H

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define KVR_JSON_ALLOW_COMMENTS 0

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace kvr
{
    namespace internal
    {
        namespace json
        {
            // optimized output memory stream interface wrapper
            struct ostream_memory
            {
                ostream_memory (kvr::mem_ostream *mem_ostream) : m_stream (mem_ostream) {}
                void  Put (char ch) { m_stream->put (ch); }
                char *Push (size_t count) { return (char *) m_stream->push (count); }
                char *Pop (size_t count) { return (char *) m_stream->pop (count); }
                void  Flush () { m_stream->flush (); }
                
                kvr::mem_ostream *m_stream;
            };
            
            // custom output stream interface wrapper
            struct ostream_custom
            {
                ostream_custom (kvr::ostream *mem_ostream) : m_stream (mem_ostream) {}
                void Put (char ch) { m_stream->put (ch); }
                void Flush () { m_stream->flush (); }
                
                kvr::ostream *m_stream;
            };
            
            // custom input stream interface wrapper
            struct istream_custom
            {
                typedef char Ch;
                istream_custom (kvr::istream *mem_istream) : m_stream (mem_istream) {}
                char    Peek () { return (char) m_stream->peek (); }
                char    Take () { uint8_t byte = 0;  m_stream->get (&byte); return (char) byte; }
                size_t  Tell () { return m_stream->tell (); }
                char *  PutBegin () { return NULL; }
                size_t  PutEnd (char *) { return 0u; }
                void    Put (char) { KVR_ASSERT (false); } // ?!
                
                kvr::istream *m_stream;
            };
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#if KVR_DEBUG
#define KVR_JSON_DEBUG_PARSE_FLAGS (kvr_rapidjson::kParseValidateEncodingFlag)
#else
#define KVR_JSON_DEBUG_PARSE_FLAGS (0)
#endif

#if KVR_JSON_ALLOW_COMMENTS
#define KVR_JSON_STRICT_PARSE_FLAGS (kvr_rapidjson::kParseCommentsFlag)
#else
#define KVR_JSON_STRICT_PARSE_FLAGS (0)
#endif

#if KVR_FLAG_ENCODE_COMPACT_FP_PRECISION
#define KVR_JSON_BASE_PARSE_FLAGS (kvr_rapidjson::kParseStopWhenDoneFlag)
#else
#define KVR_JSON_BASE_PARSE_FLAGS (kvr_rapidjson::kParseStopWhenDoneFlag | kvr_rapidjson::kParseFullPrecisionFlag)
#endif

#define KVR_JSON_PARSE_FLAGS (KVR_JSON_BASE_PARSE_FLAGS | KVR_JSON_STRICT_PARSE_FLAGS | KVR_JSON_DEBUG_PARSE_FLAGS)

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace kvr
{
    namespace internal
    {
        namespace json
        {
            struct read_ctx
            {
                ////////////////////////////////////////////////////////////
                
                read_ctx (kvr::value *value) : m_root (value), m_temp (NULL), m_depth (0)
                {
                    memset (m_stack, 0, sizeof (m_stack));
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Null ()
                {
                    KVR_ASSERT_SAFE (m_depth != 0, false);
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT (node);
                    KVR_ASSERT (node->is_map () || node->is_array ());
                    
                    if (node->is_map ())
                    {
                        KVR_ASSERT (m_temp && m_temp->is_null ());
                        m_temp = NULL;
                    }
                    else if (node->is_array ())
                    {
                        node->push_null ();
                    }
                    else
                    {
                        return false;
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Bool (bool b)
                {
                    KVR_ASSERT_SAFE (m_depth != 0, false);
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT (node);
                    KVR_ASSERT (node->is_map () || node->is_array ());
                    
                    if (node->is_map ())
                    {
                        KVR_ASSERT (m_temp && m_temp->is_null ());
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_boolean ();
#endif
                        m_temp->set_boolean (b);
                        m_temp = NULL;
                    }
                    else if (node->is_array ())
                    {
                        node->push (b);
                    }
                    else
                    {
                        return false;
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Int (int i)
                {
                    return Int64 ((int64_t) i);
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Uint (unsigned u)
                {
                    return Int64 ((int64_t) u);
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Int64 (int64_t i)
                {
                    KVR_ASSERT_SAFE (m_depth != 0, false);
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT (node);
                    KVR_ASSERT (node->is_map () || node->is_array ());
                    
                    if (node->is_map ())
                    {
                        KVR_ASSERT (m_temp && m_temp->is_null ());
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_integer ();
#endif
                        m_temp->set_integer (i);
                        m_temp = NULL;
                    }
                    else if (node->is_array ())
                    {
                        node->push (i);
                    }
                    else
                    {
                        return false;
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Uint64 (uint64_t u)
                {
                    KVR_ASSERT (m_depth != 0);
                    KVR_ASSERT (false && "not supported");
                    KVR_REF_UNUSED (u);
                    return false;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Double (double d)
                {
                    KVR_ASSERT_SAFE (m_depth != 0, false);
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
                    }
                    else if (node->is_array ())
                    {
                        node->push (d);
                    }
                    else
                    {
                        return false;
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool String (const char *str, kvr_rapidjson::SizeType length, bool copy)
                {
                    KVR_ASSERT_SAFE (m_depth != 0, false);
                    KVR_REF_UNUSED (copy);
                    
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT (node);
                    KVR_ASSERT (node->is_map () || node->is_array ());
                    
                    if (node->is_map ())
                    {
                        KVR_ASSERT (m_temp && m_temp->is_null ());
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        m_temp = m_temp->as_string ();
#endif
                        m_temp->set_string (str, (kvr::sz_t) length);
                        m_temp = NULL;
                    }
                    else if (node->is_array ())
                    {
                        kvr::value *vstr = node->push_null (); KVR_ASSERT (vstr);
#if KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION
                        vstr = vstr->as_string ();
#endif
                        vstr->set_string (str, (kvr::sz_t) length);
                    }
                    else
                    {
                        return false;
                    }
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool StartObject ()
                {
                    kvr::value *node = NULL;
                    
                    if (m_depth > 0)
                    {
                        node = m_stack [m_depth - 1];
                        KVR_ASSERT_SAFE (node && (node->is_map () || node->is_array ()), false);
                        
                        if (node->is_map ())
                        {
                            KVR_ASSERT (m_temp && m_temp->is_null ());
                            node = m_temp->as_map();
                            m_temp = NULL;
                        }
                        else if (node->is_array ())
                        {
                            node = node->push_map ();
                        }
                    }
                    else
                    {
                        node = m_root->as_map ();
                    }
                    
                    KVR_ASSERT (m_depth < KVR_CONSTANT_MAX_TREE_DEPTH);
                    m_stack [m_depth++] = node;
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool Key (const char *str, kvr_rapidjson::SizeType length, bool copy)
                {
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT_SAFE (node && node->is_map (), false);
                    
                    KVR_REF_UNUSED (length);
                    KVR_REF_UNUSED (copy);
                    
                    KVR_ASSERT (!m_temp);
                    m_temp = node->insert_null (str);
                    return (m_temp != NULL);
                }
                
                ////////////////////////////////////////////////////////////
                
                bool EndObject (kvr_rapidjson::SizeType memberCount)
                {
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT_SAFE (node && node->is_map (), false);
                    KVR_ASSERT (node->size () == (kvr::sz_t) memberCount);
                    KVR_REF_UNUSED (memberCount);
                    
                    m_stack [--m_depth] = NULL;
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool StartArray ()
                {
                    kvr::value *node = NULL;
                    
                    if (m_depth > 0)
                    {
                        node = m_stack [m_depth - 1];
                        KVR_ASSERT_SAFE (node && (node->is_map () || node->is_array ()), false);
                        
                        if (node->is_map ())
                        {
                            KVR_ASSERT (m_temp && m_temp->is_null ());
                            node = m_temp->as_array();
                            m_temp = NULL;
                        }
                        else if (node->is_array ())
                        {
                            node = node->push_array ();
                        }
                    }
                    else
                    {
                        node = m_root->as_array ();
                    }
                    
                    KVR_ASSERT (m_depth < KVR_CONSTANT_MAX_TREE_DEPTH);
                    m_stack [m_depth++] = node;
                    
                    return true;
                }
                
                ////////////////////////////////////////////////////////////
                
                bool EndArray (kvr_rapidjson::SizeType elementCount)
                {
                    kvr::value *node = m_stack [m_depth - 1];
                    KVR_ASSERT_SAFE (node && node->is_array (), false);
                    KVR_ASSERT (node->length () == (kvr::sz_t) elementCount);
                    KVR_REF_UNUSED (elementCount);
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
            
            template<typename ostr>
            struct writer
            {
                writer (ostr &os) : m_wrt (os) {}
                
                bool print (const kvr::value *val)
                {
                    KVR_ASSERT (val);
                    
                    bool success = false;
                    
                    if (val->is_map ())
                    {
                        bool ok = m_wrt.StartObject ();
                        kvr::value::cursor c (val);
                        kvr::pair p;
                        while (ok && c.get (&p))
                        {
                            kvr::key *k = p.get_key ();
                            ok = m_wrt.Key (k->get_string (), k->get_length ());
                            
                            kvr::value *v = p.get_value ();
                            ok = ok && print (v);
                        }
                        success = ok && m_wrt.EndObject ();
                    }
                    
                    else if (val->is_array ())
                    {
                        bool ok = m_wrt.StartArray ();
                        for (kvr::sz_t i = 0, c = val->length (); (i < c) && ok; ++i)
                        {
                            kvr::value *v = val->element (i);
                            ok = print (v);
                        }
                        success = ok && m_wrt.EndArray ();
                    }
                    
                    else if (val->is_string ())
                    {
                        kvr::sz_t slen = 0;
                        const char *str = val->get_string (&slen);
                        success = m_wrt.String (str, slen);
                    }
                    
                    else if (val->is_integer ())
                    {
                        int64_t n = val->get_integer ();
                        success = m_wrt.Int64 (n);
                    }
                    
                    else if (val->is_float ())
                    {
                        double n = val->get_float ();
                        success = m_wrt.Double (n);
                    }
                    
                    else if (val->is_boolean ())
                    {
                        bool b = val->get_boolean ();
                        success = m_wrt.Bool (b);
                    }
                    
                    else if (val->is_null ())
                    {
                        success = m_wrt.Null ();
                    }
                    
                    return success;
                }
                
                kvr_rapidjson::Writer<ostr> m_wrt;
            };
            
            /////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            
            bool read (kvr::value *dest, kvr::istream &istr)
            {
                KVR_ASSERT (dest);
                
                read_ctx rctx (dest);
                istream_custom ss (&istr);
                
                kvr_rapidjson::Reader reader;
                kvr_rapidjson::ParseResult ok = reader.Parse<KVR_JSON_PARSE_FLAGS> (ss, rctx);
#if KVR_DEBUG        
                if (ok.IsError ()) { std::fprintf (stderr, "JSON parse error: %s (%zu)", kvr_rapidjson::GetParseError_En (ok.Code ()), ok.Offset ()); }
#endif
                return ok && (rctx.m_depth == 0);
            }
            
            ////////////////////////////////////////////////////////////
            
            bool write (const kvr::value *src, kvr::ostream *ostr)
            {
                KVR_ASSERT (src);
                KVR_ASSERT (ostr);
                
                ostream_custom wostr (ostr);
                writer<ostream_custom> wrt (wostr);
                return wrt.print (src);
            }
            
            ////////////////////////////////////////////////////////////
            
            bool read (kvr::value *dest, kvr::mem_istream &istr)
            {
                KVR_ASSERT (dest);
                
                const char *str = (const char *) istr.buffer ();
                KVR_ASSERT (str);
                
#if KVR_DEBUG
                size_t len = istr.size ();
                KVR_REF_UNUSED (len);
                KVR_ASSERT (len > 0);
#endif
                read_ctx rctx (dest);
                kvr_rapidjson::StringStream ss (str);
                kvr_rapidjson::Reader reader;
                kvr_rapidjson::ParseResult ok = reader.Parse<KVR_JSON_PARSE_FLAGS> (ss, rctx);
#if KVR_DEBUG        
                if (ok.IsError ()) { std::fprintf (stderr, "JSON parse error: %s (%zu)", kvr_rapidjson::GetParseError_En (ok.Code ()), ok.Offset ()); }
#endif
                return ok && (rctx.m_depth == 0);
            }
            
            ////////////////////////////////////////////////////////////
            
            bool write (const kvr::value *src, kvr::mem_ostream *ostr)
            {
                KVR_ASSERT (src);
                KVR_ASSERT (ostr);
                
                ostream_memory wostr (ostr);
                writer<ostream_memory> wrt (wostr);
                return wrt.print (src);
            }
            
            ////////////////////////////////////////////////////////////
            
            size_t write_approx_size (const kvr::value *val)
            {
                size_t size = 0;
                
                if (val->is_map ())
                {
                    size += 2; // brackets
                    kvr::value::cursor c (val);
                    kvr::pair p;
                    while (c.get (&p))
                    {
                        kvr::key *k = p.get_key ();
                        kvr::value *v = p.get_value ();
                        
                        size += k->get_length () + 2; // + quotes
                        size += write_approx_size (v);
                        size += 2; // colon and comma
                    }
                }
                
                else if (val->is_array ())
                {
                    size += 2; // brackets
                    for (kvr::sz_t i = 0, c = val->length (); i < c; ++i)
                    {
                        kvr::value *v = val->element (i);
                        size += kvr::internal::ndigitsu32 (i);
                        size += write_approx_size (v);
                        size += 1; // comma
                    }
                }
                
                else if (val->is_string ())
                {
                    kvr::sz_t slen = 0;
                    const char *str = val->get_string (&slen);
                    KVR_REF_UNUSED (str);
                    size += (slen + 2); // + quotes
                }
                
                else if (val->is_integer ())
                {
                    int64_t n = val->get_integer ();
                    size += kvr::internal::ndigitsi64 (n);
                }
                
                else if (val->is_float ())
                {
                    //size += 13; // average (guess)
                    size += 25; // 25 is max
                }
                
                else if (val->is_boolean ())
                {
                    bool b = val->get_boolean ();
                    size += b ? 4 : 5;
                }
                
                else if (val->is_null ())
                {
                    size += 4;
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
