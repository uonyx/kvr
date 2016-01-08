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

#ifndef KVR_H
#define KVR_H

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_DEBUG
#if (defined (_MSC_VER) && defined (_DEBUG)) || !defined (NDEBUG)
#define KVR_DEBUG 1
#else
#define KVR_DEBUG 0
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_64
#if defined (__LP64__) || defined (_WIN64) || defined (__x86_64__) || defined (__ppc64__) || defined(__EMSCRIPTEN__)
#define KVR_64 1
#else
#define KVR_64 0
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_CPP11
#if (__cplusplus >= 201103L) || (defined (_MSC_VER) && _MSC_VER >= 1600)
#define KVR_CPP11 1
#else
#define KVR_CPP11 0
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cstring>
#if KVR_CPP11 
#include <cstdint>
#else
//#include <tr1/cstdint>
#ifdef _MSC_VER
#include "internal/rapidjson/msinttypes/stdint.h"
#else
#include <stdint.h>
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////           

// explicitly perform type conversions on values?
#define KVR_FLAG_DISABLE_IMPLICIT_TYPE_CONVERSION   0
// allow duplicate keys and speed up insertions in map values?
#define KVR_FLAG_ALLOW_DUPLICATE_MAP_KEYS           0
// use compact floating point precision during serialization?
#define KVR_FLAG_ENCODE_COMPACT_FP_PRECISION        0
// relax strict json format parsing (allowing comments etc)?
#define KVR_FLAG_DECODE_RELAXED_JSON                0

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// maximum allowed tree depth
#define KVR_CONSTANT_MAX_TREE_DEPTH                     (64u)
// epsilon for comparing floating point equality for diffs
#define KVR_CONSTANT_DIFF_FP_EQ_EPSILON                 (1.0e-7)
// memory (re)allocation element size for map & array
#define KVR_CONSTANT_COMMON_BLOCK_SZ                    (8u)

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#if (KVR_CONSTANT_COMMON_BLOCK_SZ & (KVR_CONSTANT_COMMON_BLOCK_SZ - 1))
#error "#define KVR_CONSTANT_COMMON_BLOCK_SZ must be a power of 2"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace kvr
{
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

#if KVR_64
  typedef uint32_t sz_t;
#if KVR_DEBUG
  static const uint64_t SZ_T_MAX = 0xffffffff;
#endif
#else
  typedef uint16_t sz_t;
#if KVR_DEBUG
  static const uint64_t SZ_T_MAX = 0xffff;
#endif
#endif

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  enum codec_t
  {
    CODEC_JSON,
    CODEC_MSGPACK,
    CODEC_CBOR,
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class allocator
  {
  public:
    virtual void * allocate (size_t sz) = 0;
    virtual void   deallocate (void *p, size_t sz) = 0;

  protected:
    ~allocator () {}
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class ostream
  {
  public:
    virtual void put (uint8_t byte) = 0;
    virtual void write (uint8_t *bytes, size_t count) = 0;
    virtual void flush () = 0;

  protected:
    ~ostream () {}
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class istream
  {
  public:
    virtual bool    get (uint8_t *byte) = 0;
    virtual bool    read (uint8_t *bytes, size_t count) = 0;
    virtual size_t  tell () = 0;
    virtual uint8_t peek () = 0;

  protected:
    ~istream () {}
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class ctx;
  class obuffer;
  class pair;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class key
  {
  public:

    const char *  get_string () const;
    sz_t          get_length () const;

  private:
    
    char    * m_str;
    sz_t      m_len;
    sz_t      m_ref;
    uint32_t  m_hash;

    friend class ctx;
    friend class value;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class value
  {
  public:

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    class cursor;

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    // type checking    
    bool          is_map () const;
    bool          is_array () const;
    bool          is_string () const;
    bool          is_boolean () const;
    bool          is_integer () const;
    bool          is_float () const;
    bool          is_null () const;

    // type conversion    
    value *       conv_map (sz_t sz = 8);
    value *       conv_array (sz_t sz = 8);
    value *       conv_string ();
    value *       conv_boolean ();
    value *       conv_integer ();
    value *       conv_float ();
    value *       conv_null ();

    // string variant operations
    void          set_string (const char *str, sz_t len);
    void          set_string (const char *str);
    const char *  get_string () const;
    const char *  get_string (sz_t *len) const;

    // integer variant operations
    void          set_integer (int64_t n);
    int64_t       get_integer () const;

    // floating-point variant operations
    void          set_float (double n);
    double        get_float () const;

    // boolean variant operations
    void          set_boolean (bool b);
    bool          get_boolean () const;

    // array variant operations
    value *       push (int32_t n);
    value *       push (int64_t n);
    value *       push (double n);
    value *       push (bool b);
    value *       push (const char *str);
    value *       push_map ();
    value *       push_array ();
    value *       push_null ();
    bool          pop ();
    bool          pop (sz_t index);
    value *       element (sz_t index) const;
    sz_t          length () const;

    // map variant operations
    value *       insert (const char *key, int32_t n);
    value *       insert (const char *key, int64_t n);
    value *       insert (const char *key, double n);
    value *       insert (const char *key, bool b);
    value *       insert (const char *key, const char *str);
    value *       insert_map (const char *key);
    value *       insert_array (const char *key);
    value *       insert_null (const char *key);
    value *       find (const char *key) const;
    void          remove (const char *key);
    sz_t          size () const;

    // path search (map or array)
    value *       search (const char *pathexpr) const;
    value *       search (const char **path, sz_t pathsz) const;

    // copy/merge
    value *       copy (const value *rhs);
    value *       merge (const value *rhs);

    // diff/patch
    value *       diff (const value *original, const value *modified);
    value *       patch (const value *diff);

    // hash code
    uint32_t      hash (uint32_t seed = 0) const;

    // serialization (buffer)
    bool          encode (codec_t codec, obuffer *obuf);
    bool          decode (codec_t codec, const uint8_t *data, size_t size);

    // serialization (stream)
    bool          encode (codec_t codec, ostream *ostr);
    bool          decode (codec_t codec, istream &istr);

    // serialization (estimate buffer size)
    size_t        encode_bound (codec_t codec) const;
    
    // debug stderr output
    void          dump () const;

  private:

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    union string
    {
      struct dyn_str
      {
        static const sz_t PAD = (KVR_CONSTANT_COMMON_BLOCK_SZ - 1);
        char *  m_data;
        sz_t    m_size;
        sz_t    m_len;

        const char *get () const;
        sz_t length () const;
        void set (const char *str, sz_t len, allocator *a);
        void cleanup (allocator *a);
      } m_dyn;

      struct stt_str
      {
        static const sz_t CAP = ((sizeof (sz_t) * 2) + sizeof (char *)); // sizeof dyn_str
        char m_data [CAP];

        const char *get () const;
        sz_t length () const;
        void set (const char *str, sz_t len);
      } m_stt;
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    union number
    {
      int64_t   i;
      double    f;
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    struct array
    {
      static const sz_t CAP_INCR = KVR_CONSTANT_COMMON_BLOCK_SZ;

      void    init (sz_t size, allocator *a);
      void    deinit (allocator *a);
      void    push (value *v, allocator *a);
      value * pop ();
      value * pop (sz_t index);
      value * elem (sz_t index) const;

      value **m_ptr;
      sz_t    m_len;
      sz_t    m_cap;
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    struct map
    {
      static const sz_t CAP_INCR = KVR_CONSTANT_COMMON_BLOCK_SZ;

      struct node
      {
        key   *k;
        value *v;
        node () : k (NULL), v (NULL) {}
      };

      void    init (sz_t size, allocator *a);
      void    deinit (allocator *a);
      node *  insert (key *k, value *v, allocator *a);
      void    remove (node *n);
      node *  find (const key *k) const;
      sz_t    size_l () const;
      sz_t    size_c () const;
      sz_t    _cap () const; // experimental capacity

      node *  m_ptr;
      sz_t    m_len;
      sz_t    m_cap;
    };

  public:

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    class cursor
    {
    public:

      bool get (pair *p);
      explicit cursor (const value *map);

    private:

      const map::node * _get ();
      const value * m_map;
      sz_t          m_index;
    };

  private:

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    union data
    {
      number    n;
      map       m;
      array     a;
      string    s;
      bool      b;
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    enum _flags
    {
      FLAG_NONE                 = 0,
      FLAG_TYPE_NULL            = (1 << 0),
      FLAG_TYPE_MAP             = (1 << 1),
      FLAG_TYPE_ARRAY           = (1 << 2),
      FLAG_TYPE_STRING_DYNAMIC  = (1 << 3),
      FLAG_TYPE_STRING_STATIC   = (1 << 4),
      FLAG_TYPE_NUMBER_INTEGER  = (1 << 5),
      FLAG_TYPE_NUMBER_FLOAT    = (1 << 6),
      FLAG_TYPE_BOOLEAN         = (1 << 7),
      FLAG_PARENT_CTX           = (1 << 8),
      FLAG_PARENT_MAP           = (1 << 9),
      FLAG_PARENT_ARRAY         = (1 << 10),
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    bool    _is_number () const;
    bool    _is_string_dynamic () const;
    bool    _is_string_static () const;

    void    _string_set (const char *str, sz_t len);
    void    _string_move (char *str, sz_t size);

    value * _search_path_expr (const char *expr, const char **lastkey = NULL,
                               value **lastparent = NULL) const;
    value * _search_key (const char *key) const;

    uint8_t _type () const;
    bool    _type_equiv (const value *other) const;

    void    _destruct ();
    void    _clear ();
    void    _dump (size_t lpad, const char *key) const;

    void    _diff_set_rem (value *set, value *rem, const value *og, const value *md,
                       const char **path, const sz_t pathsz, sz_t pathcnt);
    void    _diff_add (value *add, const value *og, const value *md,
                       const char **path, const sz_t pathsz, sz_t pathcnt);

    void    _patch_set (const value *set);
    void    _patch_add (const value *add);
    void    _patch_rem (const value *rem);

    void    _insert_kv (key *k, value *v);
    void    _push_v (value *v);

    value * _conv_map (sz_t cap);
    value * _conv_array (sz_t cap);

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    value (ctx *ctx, uint32_t flags);

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    data      m_data;
    uint32_t  m_flags;
    ctx     * m_ctx;

    friend class ctx;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class pair
  {
  public:

    key   * get_key () const;
    value * get_value ();

    pair ();

  private:

    key   * m_k;
    value * m_v;

    friend class value;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class ctx
  {
  public:

    static ctx * create (size_t ks_min_size, size_t vs_min_size, allocator *allocator = NULL);
    static ctx * create (allocator *allocator = NULL);    
    static void  destroy (ctx *ctx);

    value * create_value ();
    void    destroy_value (value *v);
    size_t  get_key_count ();
    size_t  get_value_count ();
    void    dump (int id = 0) const;

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

  private:

    struct key_store
    {
      void    init (size_t cap, uint32_t hfseed, allocator *a);
      void    deinit (allocator *a);
      void    resize (allocator *a);
      key *   insert (const char *str, allocator *a);
      key *   insert (char *str, sz_t len, allocator *a);
      key *   find (const char *str) const;
      void    erase (key *k, allocator *a);
      void    erase (const char *str, allocator *a);
      size_t  used () const;
      float   load_factor () const;
      void    dump () const;

      key **    m_keys;
      size_t    m_size;
      size_t    m_used;
      uint32_t  m_seed;
    };

    struct val_store
    {
      void    init (size_t cap, allocator *a);
      void    deinit (allocator *a);
      void    push_back (value *v, allocator *a);
      void    remove (value *v);
      size_t  used () const;
      value * at (size_t index);
      void    clear ();
      void    dump () const;

      value **  m_data;
      size_t    m_size;
      size_t    m_used;
    };
    
    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    value *   _create_value_null (uint32_t parentType);
    value *   _create_value_map (uint32_t parentType);
    value *   _create_value_array (uint32_t parentType);
    value *   _create_value_integer (uint32_t parentType, int64_t number);
    value *   _create_value_float (uint32_t parentType, double number);
    value *   _create_value_boolean (uint32_t parentType, bool boolean);
    value *   _create_value_string (uint32_t parentType, const char *str, sz_t len);
    value *   _create_value (uint32_t parentType);
    bool      _destroy_value (uint32_t parentType, value *v);

    key *     _find_key (const char *str);
    key *     _create_key (const char *str);    
    key *     _create_key (char *str, sz_t len);    
    void      _destroy_key (key *k);

    char *    _create_path_expr (const char **path, sz_t pathsz, sz_t *exprsz) const;
    void      _destroy_path_expr (char *expr, sz_t exprsz);

    uint32_t  _get_rand ();
    
    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

  private:

    ctx (size_t ks_size, size_t vs_size, allocator *a);
    ctx (const ctx &);
    ~ctx ();

    allocator * m_allocator;
    key_store   m_kstore;
    val_store   m_vstore;

    friend class value;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  // optimized memory output stream
  class mem_ostream
  {
  public:

    mem_ostream (uint8_t *buf, size_t sz, allocator *alloc = NULL);
    mem_ostream (size_t sz, allocator *alloc = NULL);
    ~mem_ostream ();

    void            put (uint8_t byte);
    void            write (uint8_t *bytes, size_t count);
    void            flush ();
    uint8_t *       push (size_t count);
    uint8_t *       pop (size_t count);
    size_t          tell () const;
    void            seek (size_t pos);        
    const uint8_t * buffer () const;
    size_t          size () const;
    void            reserve (size_t sz);

  private:

    mem_ostream (const mem_ostream &);
    mem_ostream &operator=(const mem_ostream &);

    enum buf_type
    {
      BUF_EXTERNAL,
      BUF_INTERNAL,
    };

    static const size_t MIN_BUF_SZ;

    uint8_t *   m_buf;
    size_t      m_sz;
    size_t      m_pos;
    buf_type    m_btype;
    allocator * m_alloc;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  // optimized memory input stream
  class mem_istream
  {
  public:

    mem_istream (const uint8_t *buf, size_t sz);

    const uint8_t * buffer () const;
    size_t          size () const;
    size_t          tell ();
    uint8_t         peek ();
    void            seek (size_t pos);
    bool            get (uint8_t *byte);
    bool            read (uint8_t *bytes, size_t count);
    const uint8_t * push (size_t count);
    const uint8_t * pop (size_t count);

  private:

    mem_istream (const mem_istream &);
    mem_istream &operator=(const mem_istream &);

    const uint8_t * m_buf;
    size_t          m_sz;
    size_t          m_pos;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  class obuffer
  {
  public:
    // outputs to internal buffer with minimum size of 256 bytes
    obuffer (size_t size = 256u, allocator *alloc = NULL);
    // outputs to external 'data' buffer. if size is insufficient, 
    // an internal buffer will be created 
    obuffer (uint8_t *data, size_t size, allocator *alloc = NULL);    

    const uint8_t * get_data () const;
    size_t          get_size () const;

  private:

    mem_ostream m_stream;
    friend class value;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "kvr.h.inl"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
