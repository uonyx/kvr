/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_H
#define KVR_H

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define KVR_PLATFORM_POSIX
#endif

#if defined (_WIN32) || (defined (_WIN64))
#define KVR_PLATFORM_WINDOWS
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if __cplusplus >= 201103L
#include <unordered_map>
#define std_unordered_map std::unordered_map
#else

#if defined (_MSC_VER)
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#define std_unordered_map std::tr1::unordered_map
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#define KVR_OPTIMIZATION_IMPLICIT_TYPE_CONVERSION_OFF   0
#define KVR_OPTIMIZATION_FAST_MAP_INSERT_ON             0
#define KVR_OPTIMIZATION_FAST_MAP_REMOVE_ON             0 // no safety - be careful

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class kvr
{
public:
  
  typedef uint16_t sz_t;

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  class key;
  class value;

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  
  class pair
  {
  public:

    const char *get_key () const;
    value * get_value ();

  private:

    pair ();
    ~pair ();

    key   *m_k;
    value *m_v;

    friend class kvr;
  };

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  class key
  {
  public:

    const char *get_string () const;

  private:

    key (const char *str);
    ~key ();

    char *m_str;
    sz_t  m_ref;

    friend class kvr;
  };

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  
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
    bool          is_null () const;
    bool          is_string () const;
    bool          is_boolean () const;
    bool          is_number () const;
    bool          is_map () const;
    bool          is_array () const;
    
    // type conversion
    void          conv_null ();
    void          conv_map ();
    void          conv_array ();
    void          conv_string ();
    void          conv_boolean ();
    void          conv_number_i ();
    void          conv_number_f ();
 
    // native variant ops
    void          set_string (const char *str);
    void          set_boolean (bool b);
    void          set_number_i (int64_t n);
    void          set_number_f (double n);
    const char *  get_string () const;
    bool          get_boolean () const;
    int64_t       get_number_i () const;
    double        get_number_f () const;

    // container (array or map) variant ops:     
    sz_t          size () const;

    // array variant ops
    value *       push (int64_t number);
    value *       push (double number);
    value *       push (bool boolean);
    value *       push (const char *str);
    value *       push_map ();
    value *       push_array ();
    value *       push_null ();
    bool          pop ();
    value *       element (sz_t index) const;

    // map variant ops
    pair *        find (const char *key) const;
    pair *        insert (const char *key, int64_t number);
    pair *        insert (const char *key, double number);
    pair *        insert (const char *key, bool boolean);
    pair *        insert (const char *key, const char *str);
    pair *        insert_map (const char *key);
    pair *        insert_array (const char *key);
    pair *        insert_null (const char *key);    
    bool          remove (pair *node);
    cursor        fcursor () const;

    // path search
    value *       search (const char **path, sz_t len);

    // copy
    void          copy (const value *rhs);

    // debug log
    void          dump () const;

  private:

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    union string
    {
      struct dyn_str
      {
        const char *data;
        sz_t    size;
        sz_t    len;
      } dyn;

      struct stt_str
      {
        static const sz_t CAP = ((sizeof (sz_t) * 2) + sizeof (char *));
        char data [CAP];
      } stt;
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
      value **m_ptr;
      sz_t    m_len;
      sz_t    m_cap;

      void    init (sz_t size);
      void    deinit ();
      void    push (value *v);
      value  *pop ();

      static const sz_t SIZE_INCR = 8;
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    struct map
    {
      void  init (sz_t size);
      void  deinit ();
      pair *insert (key *k, value *v);
      bool  remove (pair *p);
      pair *find (const key *k) const;

      pair *m_ptr;
      sz_t  m_size;
      sz_t  m_cap;

      static const sz_t SIZE_INCR = 8;
    };

  public:

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    class cursor
    {
    public:
      pair *get_pair ();
      ~cursor () { m_map = NULL; }

    private:
      cursor (const map *m) : m_map (m), m_index (0) {}
      const map *m_map;
      sz_t m_index;

      friend class value;
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

      data () {}
    };

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    void _dump (size_t lpad, const char *key) const;
    void _conv_map (sz_t size = map::SIZE_INCR);
    void _conv_array (sz_t size = array::SIZE_INCR);

    void clear ();

    bool is_string_dynamic () const;
    bool is_string_static () const;
    bool is_number_integer () const;
    bool is_number_float () const;

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    value (kvr *ctx, uint32_t flags);
    ~value ();

    ///////////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    data      m_data; 
    uint32_t  m_flags;
    kvr     * m_ctx;
    
    friend class kvr;
  };

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  enum encoding_type
  {
    ENCODING_TYPE_JSON,
    ENCODING_TYPE_MSGPACK,
  };

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  static kvr *  init (uint32_t flags = 0);
  static void   deinit (kvr *ctx);

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  value * create_value_null ();
  value * create_value_map ();
  value * create_value_array ();
  value * create_value (int64_t number);
  value * create_value (double number);
  value * create_value (bool boolean);
  value * create_value (const char *str);
  void    destroy_value (value *v);

  value * diff (const value *va, const value *vb);
  value * patch (value *vout, const value *vpatch);
  size_t  serialize (encoding_type encoding, const value *v, char *data, size_t size);
  value * deserialize (encoding_type encoding, const char *data, size_t size);

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

private:

  kvr ();
  kvr (const kvr &);
  ~kvr ();

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  enum value_flags
  {
    VALUE_FLAG_NONE                 = 0,
    VALUE_FLAG_TYPE_NULL            = (1 << 0),
    VALUE_FLAG_TYPE_MAP             = (1 << 1),
    VALUE_FLAG_TYPE_ARRAY           = (1 << 2),
    VALUE_FLAG_TYPE_DYN_STRING      = (1 << 3),
    VALUE_FLAG_TYPE_STT_STRING      = (1 << 4),
    VALUE_FLAG_TYPE_NUMBER_INTEGER  = (1 << 5),
    VALUE_FLAG_TYPE_NUMBER_FLOAT    = (1 << 6),
    VALUE_FLAG_TYPE_BOOLEAN         = (1 << 7),

    VALUE_FLAG_PARENT_CTX           = (1 << 8),
    VALUE_FLAG_PARENT_MAP           = (1 << 9),
    VALUE_FLAG_PARENT_ARRAY         = (1 << 10),
  };

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  struct equal_cstr
  {
    bool operator ()(const char *a, const char *b) const
    {
      return strcmp (a, b) == 0;
    }
  };

  struct hash_djb
  {
    uint32_t operator ()(const char *s) const
    {
      uint32_t hash = 5381;
      char c;

      while ((c = *s++))
      {
        hash = ((hash << 5) + hash) + c;
      }

      return hash;
    }
  };

  typedef std_unordered_map<const char *, key *, hash_djb, equal_cstr> keystore;
  
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  
#if 0
  class allocator // key, value, string
  {
  public:
    bool init () { return true; }
    void deinit () {}
  };

  class pool_alloc
  {
  public:
    enum object_type
    {
      OBJECT_TYPE_KEY,
      OBJECT_TYPE_VALUE,
      OBJECT_TYPE_STRING,
      OBJECT_TYPE_COUNT,
    };

    void *malloc (object_type type);
    void free (void *block);

  private:
    void *m_pools [OBJECT_TYPE_COUNT];

  private:
    class pool
    {
    public:
      pool (size_t blockSize);

    private:
      void *m_ptr;
      size_t m_size;
      // free list
      // used list
    };
  };
#endif

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  key * find_key (const char *str);
  key * create_key (const char *str);
  void  destroy_key (key *k);

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  value * _create_value_null (uint32_t parentType);
  value * _create_value_map (uint32_t parentType);
  value * _create_value_array (uint32_t parentType);
  value * _create_value (uint32_t parentType, int64_t number);
  value * _create_value (uint32_t parentType, double number);
  value * _create_value (uint32_t parentType, bool boolean);
  value * _create_value (uint32_t parentType, const char *str);
  void    _destroy_value (uint32_t parentType, value *v);

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  keystore m_keystore;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

inline bool kvr::value::is_map () const
{
  return (m_flags & VALUE_FLAG_TYPE_MAP) != 0;
}

inline bool kvr::value::is_array () const
{
  return (m_flags & VALUE_FLAG_TYPE_ARRAY) != 0;
}

inline bool kvr::value::is_string () const
{
  return (m_flags & (VALUE_FLAG_TYPE_DYN_STRING | VALUE_FLAG_TYPE_STT_STRING)) != 0;
}

inline bool kvr::value::is_boolean () const
{
  return (m_flags & VALUE_FLAG_TYPE_BOOLEAN) != 0;
}

inline bool kvr::value::is_number () const
{
  return (m_flags & (VALUE_FLAG_TYPE_NUMBER_INTEGER | VALUE_FLAG_TYPE_NUMBER_FLOAT)) != 0;
}

inline bool kvr::value::is_null () const
{
  return (m_flags & VALUE_FLAG_TYPE_NULL) != 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

inline bool kvr::value::is_string_dynamic () const
{
  return (m_flags & VALUE_FLAG_TYPE_DYN_STRING) != 0;
}

inline bool kvr::value::is_string_static () const
{
  return (m_flags & VALUE_FLAG_TYPE_STT_STRING) != 0;
}

inline bool kvr::value::is_number_integer () const
{
  return (m_flags & VALUE_FLAG_TYPE_NUMBER_INTEGER) != 0;
}

inline bool kvr::value::is_number_float () const
{
  return (m_flags & VALUE_FLAG_TYPE_NUMBER_FLOAT) != 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
