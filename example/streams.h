/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Copyright (c) 2015 Ubaka Onyechi
 *
 * kvr is free software distributed under the MIT license.
 * See https://github.com/uonyx/kvr/blob/master/LICENSE for details.
 */

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_EXAMPLE_STREAMS
#define KVR_EXAMPLE_STREAMS

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "../src/kvr.h"
#include <cstdio>
#include <cassert>
#ifdef KVR_EXAMPLE_HAVE_OPENSSL
#include <openssl/sha.h>
#endif
#ifdef KVR_EXAMPLE_HAVE_ZLIB
#include <algorithm>
#include <zlib.h>
#endif
#ifdef KVR_EXAMPLE_HAVE_LZ4
#include "lz4/lz4.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// hexadecimal string output stream : useful for debugging binary output (msgpack, cbor)
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class hex_ostream : public kvr::ostream
{
public:

  hex_ostream (size_t size = 512u) : m_os (size) {}

  void put (uint8_t byte)
  {
    static const uint8_t lut [] = "0123456789abcdef";
    uint8_t hi = lut [(byte >> 4)];
    uint8_t lo = lut [(byte & 15)];
    m_os.put (hi);
    m_os.put (lo);
  }

  void write (uint8_t *bytes, size_t count)
  {
    for (size_t i = 0; i < count; ++i)
    {
      this->put (bytes [i]);
    }
  }

  void flush ()
  {
    m_os.flush ();
  }

  const char *string () const
  {
    return reinterpret_cast<const char *>(m_os.buffer ());
  }

  size_t length () const
  {
    return m_os.tell ();
  }

  void reset ()
  {
    m_os.seek (0);
    m_os.flush ();
  }

private:

  hex_ostream (const hex_ostream &);
  hex_ostream &operator=(const hex_ostream &);

  kvr::mem_ostream m_os;
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// percent-encoding output stream : useful for HTTP GET/POST request data 
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class percent_encoding_ostream : public kvr::ostream
{
public:

  percent_encoding_ostream (size_t size = 512u) : m_os (size) {}

  void put (uint8_t byte)
  {
    char ch = byte;

    if (((ch >= '0') && (ch <= '9')) ||
        ((ch >= 'a') && (ch <= 'z')) ||
        ((ch >= 'A') && (ch <= 'Z')) ||
        ((ch == '-') || (ch == '.') || (ch == '_') || (ch == '~')))
    {
      // unreserved characters
      m_os.put (byte);
    }
    else
    {
      // control characters, reserved characters, unsafe characters
      static const uint8_t lut [] = "0123456789ABCDEF";
      uint8_t hi = lut [(byte >> 4)];
      uint8_t lo = lut [(byte & 15)];
      m_os.put ('%');
      m_os.put (hi);
      m_os.put (lo);
    }
  }

  void write (uint8_t *bytes, size_t count)
  {
    for (size_t i = 0; i < count; ++i)
    {
      this->put (bytes [i]);
    }
  }

  void flush ()
  {
    m_os.flush ();
  }

  const char *string () const
  {
    return reinterpret_cast<const char *>(m_os.buffer ());
  }

  size_t length () const
  {
    return m_os.tell ();
  }

  void reset ()
  {
    m_os.seek (0);
    m_os.flush ();
  }

private:

  percent_encoding_ostream (const percent_encoding_ostream &);
  percent_encoding_ostream &operator=(const percent_encoding_ostream &);

  kvr::mem_ostream m_os;
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// input file stream 
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BUF_SZ = BUFSIZ>
class file_istream : public kvr::istream
{
public:

  file_istream (const char *filename) : m_fp (NULL)
  {
    this->open (filename);
  }

  ~file_istream ()
  {
    if (m_fp) { this->close (); }
  }

  bool open (const char *filename)
  {
    assert (!m_fp);
#ifdef _MSC_VER
    fopen_s (&m_fp, filename, "rb");
#else
    m_fp = fopen (filename, "rb");
#endif
    if (m_fp)
    {
      setvbuf (m_fp, m_buf, _IOFBF, MAX_BUF_SZ);
      return true;
    }

    return false;
  }

  void close ()
  {
    assert (m_fp);
    fclose (m_fp);
    m_fp = NULL;
  }

  bool get (uint8_t *byte)
  {
    assert (m_fp);
#ifdef _MSC_VER
    fread_s (byte, 1, sizeof (uint8_t), 1, m_fp);
#else
    fread (byte, sizeof (uint8_t), 1, m_fp);
#endif
    return true;
  }

  bool read (uint8_t *bytes, size_t count)
  {
    assert (m_fp);
#ifdef _MSC_VER
    fread_s (bytes, count, sizeof (uint8_t), count, m_fp);
#else
    fread (bytes, sizeof (uint8_t), count, m_fp);
#endif
    return true;
  }

  size_t tell ()
  {
    assert (m_fp);
    long p = ftell (m_fp);
    return static_cast<size_t>(p);
  }

  uint8_t peek ()
  {
    assert (m_fp);
    int c = fgetc (m_fp);
    ungetc (c, m_fp);
    return (c == EOF) ? 0u : static_cast<uint8_t>(c);
  }

private:

  file_istream (const file_istream &);
  file_istream &operator=(const file_istream &);

  FILE  * m_fp;
  char    m_buf [MAX_BUF_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// output file stream 
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BUF_SZ = BUFSIZ>
class file_ostream : public kvr::ostream
{
public:

  file_ostream (const char *filename) : m_fp (NULL)
  {
    this->open (filename);
  }

  ~file_ostream ()
  {
    if (m_fp) { this->close (); }
  }

  bool open (const char *filename)
  {
    assert (!m_fp);
#ifdef _MSC_VER
    fopen_s (&m_fp, filename, "wb");
#else
    m_fp = fopen (filename, "wb");
#endif
    if (m_fp)
    {
      setvbuf (m_fp, m_buf, _IOFBF, MAX_BUF_SZ);
      return true;
    }

    return false;
  }

  void close ()
  {
    assert (m_fp);
    fclose (m_fp);
    m_fp = NULL;
  }

  void put (uint8_t byte)
  {
    assert (m_fp);
    fwrite (&byte, sizeof (uint8_t), 1, m_fp);
  }

  void write (uint8_t *bytes, size_t count)
  {
    assert (m_fp);
    fwrite (bytes, sizeof (uint8_t), count, m_fp);
  }

  void flush ()
  {
    assert (m_fp);
    fflush (m_fp);
  }

private:

  file_ostream (const file_ostream &);
  file_ostream &operator=(const file_ostream &);

  FILE  * m_fp;
  char    m_buf [MAX_BUF_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// buffered input file stream : similar to file_istream but with better performance.
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BUF_SZ>
class buffered_file_istream : public kvr::istream
{
public:

  buffered_file_istream (const char *filename) : m_fp (0), m_sz (0), m_pos (0)
  {
    this->open (filename);
  }

  ~buffered_file_istream ()
  {
    this->close ();
  }

  bool open (const char *filename)
  {
    if (!m_fp)
    {
#ifdef _MSC_VER
      fopen_s (&m_fp, filename, "rb");
#else
      m_fp = fopen (filename, "rb");
#endif
      return m_fp ? true : false;
    }

    return false;
  }

  void close ()
  {
    if (m_fp)
    {
      fclose (m_fp);
      m_fp = NULL;
    }
  }

  bool get (uint8_t *byte)
  {
    if (m_pos >= m_sz)
    {
#ifdef _MSC_VER
      m_sz = fread_s (m_buf, MAX_BUF_SZ, sizeof (uint8_t), MAX_BUF_SZ, m_fp);
#else
      m_sz = fread (m_buf, sizeof (uint8_t), MAX_BUF_SZ, m_fp);
#endif
      m_pos = 0;
    }

    *byte = m_buf [m_pos++];
    return true;
  }

  bool read (uint8_t *bytes, size_t count)
  {
    if (count > (m_sz - m_pos))
    {
      // read everything in buffer first
      size_t copy = m_sz - m_pos;
      memcpy (bytes, &m_buf [m_pos], copy);
      m_pos += copy;
      bytes += copy;
      count -= copy;
    }

    if (count >= MAX_BUF_SZ)
    {
#ifdef _MSC_VER
      fread_s (bytes, count, sizeof (uint8_t), count, m_fp);
#else
      fread (bytes, sizeof (uint8_t), count, m_fp);
#endif
    }
    else
    {
      if ((m_pos + count) > m_sz)
      {
#ifdef _MSC_VER
        m_sz = fread_s (m_buf, MAX_BUF_SZ, sizeof (uint8_t), MAX_BUF_SZ, m_fp);
#else
        m_sz = fread (m_buf, sizeof (uint8_t), MAX_BUF_SZ, m_fp);
#endif
        m_pos = 0;
      }

      memcpy (bytes, &m_buf [m_pos], count);
      m_pos += count;
    }

    return true;
  }

  size_t tell ()
  {
    if (m_pos >= m_sz)
    {
      long p = ftell (m_fp);
      return static_cast<size_t>(p);
    }

    return m_pos;
  }

  uint8_t peek ()
  {
    if (m_pos >= m_sz)
    {
      int c = fgetc (m_fp);
      ungetc (c, m_fp);
      return (c == EOF) ? 0u : (uint8_t) c;
    }

    return m_buf [m_pos];
  }

private:

  buffered_file_istream (const buffered_file_istream &);
  buffered_file_istream &operator=(const buffered_file_istream &);

  FILE    * m_fp;
  size_t    m_sz;
  size_t    m_pos;
  uint8_t   m_buf [MAX_BUF_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// buffered output file stream : similar to file_ostream but with better performance.
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BUF_SZ>
class buffered_file_ostream : public kvr::ostream
{
public:

  buffered_file_ostream (const char *filename) : m_fp (NULL), m_pos (0)
  {
    this->open (filename);
  }

  ~buffered_file_ostream ()
  {
    this->close ();
  }

  bool open (const char *filename)
  {
    if (!m_fp)
    {
#ifdef _MSC_VER
      fopen_s (&m_fp, filename, "wb");
#else
      m_fp = fopen (filename, "wb");
#endif
      return m_fp ? true : false;
    }

    return false;
  }

  void close ()
  {
    if (m_fp)
    {
      fclose (m_fp);
      m_fp = NULL;
    }
  }

  void put (uint8_t byte)
  {
    if (m_pos >= MAX_BUF_SZ)
    {
      fwrite (m_buf, sizeof (uint8_t), m_pos, m_fp);
      m_pos = 0;
    }

    m_buf [m_pos++] = byte;
  }

  void write (uint8_t *bytes, size_t count)
  {
    if ((m_pos + count) > MAX_BUF_SZ)
    {
      fwrite (m_buf, sizeof (uint8_t), m_pos, m_fp);
      m_pos = 0;
    }

    if (count >= MAX_BUF_SZ)
    {
      fwrite (bytes, sizeof (uint8_t), count, m_fp);
    }
    else
    {
      memcpy (&m_buf [m_pos], bytes, count);
      m_pos += count;
    }
  }

  void flush ()
  {
    fwrite (m_buf, sizeof (uint8_t), m_pos, m_fp);
    fflush (m_fp);
  }

private:

  buffered_file_ostream (const buffered_file_ostream &);
  buffered_file_ostream &operator=(const buffered_file_ostream &);

  FILE    * m_fp;
  size_t    m_pos;
  uint8_t   m_buf [MAX_BUF_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef KVR_EXAMPLE_HAVE_OPENSSL
// SHA1 hash string stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class sha1_ostream : public kvr::ostream
{
public:

  sha1_ostream () : m_len (0)
  {
    SHA1_Init (&m_ctx);
    m_str [0] = 0;
  }

  void put (uint8_t byte)
  {
    SHA1_Update (&m_ctx, &byte, 1);
  }

  void write (uint8_t *bytes, size_t count)
  {
    SHA1_Update (&m_ctx, bytes, count);
  }

  void flush ()
  {
    const size_t DIGEST_SIZE = 20;
    uint8_t digest [DIGEST_SIZE];
    SHA1_Final (digest, &m_ctx);

    for (size_t i = 0; i < DIGEST_SIZE; ++i)
    {
      static const char lut [] = "0123456789abcdef";
      uint8_t byte = digest [i];
      char hi = lut [(byte >> 4)];
      char lo = lut [(byte & 15)];
      m_str [m_len++] = hi;
      m_str [m_len++] = lo;
    }

    m_str [m_len] = 0;
  }

  const char *string () const
  {
    return m_str;
  }

  size_t length () const
  {
    return m_len;
  }

  void reset ()
  {
    m_str [0] = 0;
    m_len = 0;
    SHA1_Init (&m_ctx);
  }

private:

  sha1_ostream (const sha1_ostream &);
  sha1_ostream &operator=(const sha1_ostream &);

  SHA_CTX  m_ctx;
  size_t   m_len;
  char     m_str [64];
};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef KVR_EXAMPLE_HAVE_ZLIB
// GZIP compression input file stream 
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BLOCK_SZ>
class gzip_file_istream : public kvr::istream
{
#if KVR_CPP11
  static_assert (MAX_BLOCK_SZ < (64 * 1024), "block size can't be more than 64kb");
#endif

public:

  gzip_file_istream (const char *filename) : m_fp (NULL), m_sz (0), m_pos (0), m_tlp (0)
  {
    this->open (filename);
  }

  ~gzip_file_istream ()
  {
    if (m_fp) { this->close (); }
  }

  bool open (const char *filename)
  {
    assert (!m_fp);

    memset (&m_zs, 0, sizeof (m_zs));
    int err = inflateInit2 (&m_zs, (15 + 16));
    if (err != Z_OK)
    {
      return false;
    }

#ifdef _MSC_VER
    fopen_s (&m_fp, filename, "rb");
#else
    m_fp = fopen (filename, "rb");
#endif
    if (!m_fp)
    {
      inflateEnd (&m_zs);
      return false;
    }

    m_pos = 0;
    m_tlp = 0;
    m_sz  = 0;

    return true;
  }

  void close ()
  {
    assert (m_fp);
    fclose (m_fp);
    m_fp = NULL;
    inflateEnd (&m_zs);
  }

  bool get (uint8_t *byte)
  {
    if (m_pos >= m_sz)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
    }

    if (m_sz)
    {
      *byte = m_buf [m_pos++];
      return true;
    }

    return false;
  }

  bool read (uint8_t *bytes, size_t count)
  {
    assert (m_fp);

    // requesting more the remaining buffer size
    size_t left = m_sz - m_pos;
    if (left && (count > left))
    {
      // read everything in buffer first      
      memcpy (bytes, &m_buf [m_pos], left);
      m_pos += left;
      bytes += left;
      count -= left;
    }

    // more than max buffer size
    while (count >= MAX_BLOCK_SZ)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
      
      memcpy (bytes, &m_buf, m_sz);
      m_pos += m_sz;
      bytes += m_sz;
      count -= m_sz;
    }

    if ((m_pos + count) > m_sz)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
    }

    if (m_sz)
    {
      memcpy (bytes, &m_buf [m_pos], count);
      m_pos += count;
      return true;
    }

    return false;
  }

  size_t tell ()
  {
    assert (m_fp);    
    return m_tlp + m_pos;
  }

  uint8_t peek ()
  {
    assert (m_fp);

    if (m_pos >= m_sz)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
    }

    uint8_t ch = (m_sz > 0) ? m_buf [m_pos] : 0u;
    return ch;
  }

private:

  size_t decompress_buf (uint8_t *dst, size_t sz)
  {
    uint8_t in [MAX_BLOCK_SZ];
#ifdef _MSC_VER
    m_sz = fread_s (in, MAX_BLOCK_SZ, sizeof (uint8_t), MAX_BLOCK_SZ, m_fp);
#else
    m_sz = fread (in, sizeof (uint8_t), MAX_BLOCK_SZ, m_fp);
#endif
    if (ferror (m_fp)) 
    { 
      return 0u; 
    }

    m_zs.next_in = in;
    m_zs.avail_in = m_sz;

    m_zs.avail_out = sz;
    m_zs.next_out = dst;

    int ret = inflate (&m_zs, Z_NO_FLUSH);
    if ((ret == Z_STREAM_ERROR) || (ret == Z_NEED_DICT) || (ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR))
    {
      return 0u;
    }

    fseek (m_fp, -((int) m_zs.avail_in), SEEK_CUR);        
    
    size_t have = sz - m_zs.avail_out;
    return have;
  }

  void reset_buf ()
  {
    m_pos = 0;
    m_tlp += m_sz;
  }

  z_stream            m_zs;
  FILE              * m_fp;
  size_t              m_sz;
  size_t              m_pos;
  size_t              m_tlp;
  uint8_t             m_buf [MAX_BLOCK_SZ];  
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// GZIP compression output file stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BLOCK_SZ>
class gzip_file_ostream : public kvr::ostream
{
#if KVR_CPP11
  static_assert (MAX_BLOCK_SZ < (64 * 1024), "block size can't be more than 64kb");
#endif

public:

  gzip_file_ostream (const char *filename) : m_fp (NULL), m_pos (0)
  {
    this->open (filename);
  }

  ~gzip_file_ostream ()
  {
    if (m_fp) { this->close (); }
  }

  bool open (const char *filename)
  {
    assert (!m_fp);

    memset (&m_zs, 0, sizeof (m_zs));
    int err = deflateInit2 (&m_zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15 + 16), 8, Z_DEFAULT_STRATEGY);
    if (err != Z_OK)
    {
      return false;
    }
#ifdef _MSC_VER
    fopen_s (&m_fp, filename, "wb");
#else
    m_fp = fopen (filename, "wb");
#endif
    if (!m_fp)
    {
      deflateEnd (&m_zs);
      return false;
    }

    m_pos = 0;

    return true;
  }

  void close ()
  {
    assert (m_fp);
    fclose (m_fp);
    m_fp = NULL;
    deflateEnd (&m_zs);
  }

  void put (uint8_t byte)
  {
    assert (m_fp);

    if (m_pos >= MAX_BLOCK_SZ)
    {
      this->compress_buf (m_buf, m_pos, Z_NO_FLUSH);
      m_pos = 0;
    }

    m_buf [m_pos++] = byte;
  }

  void write (uint8_t *bytes, size_t count)
  {
    assert (m_fp);

    if ((m_pos + count) > MAX_BLOCK_SZ)
    {
      this->compress_buf (m_buf, m_pos, Z_NO_FLUSH);
      m_pos = 0;
    }

    while (count >= MAX_BLOCK_SZ)
    {
      memcpy (&m_buf [m_pos], bytes, MAX_BLOCK_SZ);
      m_pos += MAX_BLOCK_SZ;

      this->compress_buf (m_buf, m_pos, Z_NO_FLUSH);
      m_pos = 0;

      bytes += MAX_BLOCK_SZ;
      count -= MAX_BLOCK_SZ;
    }

    memcpy (&m_buf [m_pos], bytes, count);
    m_pos += count;
  }

  void flush ()
  {
    assert (m_fp);

    if (m_pos > 0)
    {
      this->compress_buf (m_buf, m_pos, Z_FINISH);
      m_pos = 0;
    }

    fflush (m_fp);
  }

private:

  void compress_buf (const uint8_t *src, size_t sz, int zflush)
  {
    m_zs.next_in = (z_const Bytef *) src;
    m_zs.avail_in = sz;    

    unsigned char out [MAX_BLOCK_SZ];

    do
    {
      m_zs.avail_out = MAX_BLOCK_SZ;
      m_zs.next_out = out;

      int ret = deflate (&m_zs, zflush);
      assert (ret != Z_STREAM_ERROR);
      
      size_t have = MAX_BLOCK_SZ - m_zs.avail_out;
      fwrite (out, 1, have, m_fp);
    } 
    while (m_zs.avail_out == 0);
  }

  z_stream          m_zs;
  FILE            * m_fp;
  size_t            m_pos;
  uint8_t           m_buf [MAX_BLOCK_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// GZIP compression input memory stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BLOCK_SZ>
class gzip_istream : public kvr::istream
{
#if KVR_CPP11
  static_assert (MAX_BLOCK_SZ <= (64 * 1024), "block size can't be more than 64kb");
#endif

public:

  gzip_istream (const uint8_t *buf, size_t sz) : m_is (buf, sz), m_sz (0), m_pos (0), m_tlp (0)
  {
    memset (&m_zs, 0, sizeof (m_zs));
    inflateInit2 (&m_zs, (15 + 16));    
  }

  ~gzip_istream ()
  {
    inflateEnd (&m_zs);
  }

  bool get (uint8_t *byte)
  {
    if (m_pos >= m_sz)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
    }

    if (m_sz)
    {
      *byte = m_buf [m_pos++];
      return true;
    }

    return false;
  }

  bool read (uint8_t *bytes, size_t count)
  {
    // requesting more the remaining buffer size
    size_t left = m_sz - m_pos;
    if (left && (count > left))
    {
      // read everything in buffer first      
      memcpy (bytes, &m_buf [m_pos], left);
      m_pos += left;
      bytes += left;
      count -= left;
    }

    // more than max buffer size
    while (count >= MAX_BLOCK_SZ)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
      
      memcpy (bytes, &m_buf, m_sz);
      m_pos += m_sz;
      bytes += m_sz;
      count -= m_sz;
    }

    if ((m_pos + count) > m_sz)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
    }

    if (m_sz)
    {
      memcpy (bytes, &m_buf [m_pos], count);
      m_pos += count;
      return true;
    }

    return false;
  }

  size_t tell ()
  {
    return m_tlp + m_pos;
  }

  uint8_t peek ()
  {
    if (m_pos >= m_sz)
    {
      this->reset_buf ();
      m_sz = this->decompress_buf (m_buf, MAX_BLOCK_SZ);
    }

    uint8_t ch = (m_sz > 0) ? m_buf [m_pos] : 0u;
    return ch;
  }

private:

  size_t decompress_buf (uint8_t *dst, size_t sz)
  {
    size_t isz = std::min (m_is.size () - m_is.tell (), sz);
    m_zs.next_in = (z_const Bytef *) m_is.push (isz);
    m_zs.avail_in = isz;
    
    m_zs.avail_out = sz;
    m_zs.next_out = dst;

    int ret = inflate (&m_zs, Z_NO_FLUSH);
    if ((ret == Z_STREAM_ERROR) || (ret == Z_NEED_DICT) || (ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR))
    {
      return 0u;
    }
    
    m_is.pop (m_zs.avail_in);
    size_t have = sz - m_zs.avail_out;
    return have;
  }

  void reset_buf ()
  {
    m_pos = 0;
    m_tlp += m_sz;
  }

  z_stream            m_zs;
  kvr::mem_istream    m_is;
  size_t              m_sz;
  size_t              m_pos;
  size_t              m_tlp; 
  uint8_t             m_buf [MAX_BLOCK_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// GZIP compression output memory stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BLOCK_SZ>
class gzip_ostream : public kvr::ostream
{
#if KVR_CPP11
  static_assert (MAX_BLOCK_SZ <= (64 * 1024), "block size can't be more than 64kb");
#endif

public:

  gzip_ostream (size_t size = 1024) : m_os (size), m_pos (0)
  {
    memset (&m_zs, 0, sizeof (m_zs));
    deflateInit2 (&m_zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15 + 16), 8, Z_DEFAULT_STRATEGY);
  }

  ~gzip_ostream ()
  {
    deflateEnd (&m_zs);
  }

  void put (uint8_t byte)
  {
    if (m_pos >= MAX_BLOCK_SZ)
    {
      this->compress_buf (m_buf, m_pos, Z_NO_FLUSH);
      m_pos = 0;
    }

    m_buf [m_pos++] = byte;
  }

  void write (uint8_t *bytes, size_t count)
  {
    if ((m_pos + count) > MAX_BLOCK_SZ)
    {
      this->compress_buf (m_buf, m_pos, Z_NO_FLUSH);
      m_pos = 0;
    }

    while (count >= MAX_BLOCK_SZ)
    {
      memcpy (&m_buf [m_pos], bytes, MAX_BLOCK_SZ);
      m_pos += MAX_BLOCK_SZ;

      this->compress_buf (m_buf, m_pos, Z_NO_FLUSH);
      m_pos = 0;

      bytes += MAX_BLOCK_SZ;
      count -= MAX_BLOCK_SZ;
    }

    memcpy (&m_buf [m_pos], bytes, count);
    m_pos += count;
  }

  void flush ()
  {
    if (m_pos > 0)
    {
      this->compress_buf (m_buf, m_pos, Z_FINISH);
      m_pos = 0;
    }

    m_os.flush ();
  }

  const uint8_t *data () const
  {
    return m_os.buffer ();
  }

  size_t size () const
  {
    return m_os.tell ();
  }

private:

  void compress_buf (const uint8_t *src, size_t sz, int zflush)
  {
    m_zs.next_in = (z_const Bytef *) src;
    m_zs.avail_in = sz;    

    static const size_t csz = compressBound (MAX_BLOCK_SZ);
    do
    {
      uint8_t *out = m_os.push (csz);

      m_zs.avail_out = csz;
      m_zs.next_out = out;

      int ret = deflate (&m_zs, zflush);
      assert (ret != Z_STREAM_ERROR);
      
      m_os.pop (m_zs.avail_out);
    } 
    while (m_zs.avail_out == 0);
  }

  z_stream          m_zs;
  kvr::mem_ostream  m_os;  
  size_t            m_pos;
  uint8_t           m_buf [MAX_BLOCK_SZ];
};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef KVR_EXAMPLE_HAVE_LZ4
// double-buffered LZ4 compression input stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BLOCK_SZ>
class lz4_istream : public kvr::istream
{
#if KVR_CPP11
  static_assert (MAX_BLOCK_SZ <= (64 * 1024), "block size can't be more than 64kb");
#endif

public:

  lz4_istream (const uint8_t *buf, size_t sz) : m_is (buf, sz), m_sz (0), m_pos (0), m_idx (1), m_tlp (0)
  {
    LZ4_setStreamDecode (&m_lz4s, NULL, 0);
  }

  bool get (uint8_t *byte)
  {
    if (m_pos >= m_sz)
    {
      this->swap_buf ();
      m_sz = this->decompress_buf (m_buf [m_idx], MAX_BLOCK_SZ);
    }

    if (m_sz)
    {
      *byte = m_buf [m_idx][m_pos++];
      return true;
    }

    return false;
  }

  bool read (uint8_t *bytes, size_t count)
  {
    // requesting more the remaining buffer size
    size_t left = m_sz - m_pos;
    if (left && (count > left))
    {
      // read everything in buffer first      
      memcpy (bytes, &m_buf [m_idx][m_pos], left);
      m_pos += left;
      bytes += left;
      count -= left;
    }

    // more than max buffer size
    while (count >= MAX_BLOCK_SZ)
    {
      this->swap_buf ();
      m_sz = this->decompress_buf (m_buf [m_idx], MAX_BLOCK_SZ);

      memcpy (bytes, &m_buf [m_idx][m_pos], m_sz);
      
      m_pos += m_sz;
      bytes += m_sz;
      count -= m_sz;
    }

    if ((m_pos + count) > m_sz)
    {
      this->swap_buf ();
      m_sz = this->decompress_buf (m_buf [m_idx], MAX_BLOCK_SZ);
    }

    if (m_sz)
    {
      memcpy (bytes, &m_buf [m_idx][m_pos], count);
      m_pos += count;
      return true;
    }

    return false;
  }

  size_t tell ()
  {
    return m_tlp += m_pos;
  }

  uint8_t peek ()
  {
    if (m_pos >= m_sz)
    {
      this->swap_buf ();      
      m_sz = this->decompress_buf (m_buf [m_idx], MAX_BLOCK_SZ);
    }

    uint8_t ch = (m_sz > 0) ? m_buf [m_idx][m_pos] : 0u;
    return ch;
  }

private:

  size_t decompress_buf (char *dst, size_t sz)
  {
    // get compressed block
    int cbufsz = 0;    
    if (!m_is.read (reinterpret_cast<uint8_t *>(&cbufsz), sizeof (cbufsz)))
    {
      return 0;
    }
    const char *c = (const char *) m_is.push (cbufsz);
    if (!c)
    {
      return 0;
    }

    // decompress to buffer
    const char *cbuf = reinterpret_cast<const char *>(c);
    int dbufsz = LZ4_decompress_safe_continue (&m_lz4s, cbuf, dst, cbufsz, (int) sz);
    if (dbufsz < 0)
    {
      return 0;
    }

    return static_cast<size_t>(dbufsz);
  }

  void swap_buf ()
  {
    m_idx = (m_idx + 1) % 2;
    m_pos = 0;
    m_tlp += m_sz;
  }

  LZ4_streamDecode_t  m_lz4s;
  kvr::mem_istream    m_is;
  size_t              m_sz;
  size_t              m_pos;
  uint8_t             m_idx;
  size_t              m_tlp;
  char                m_buf [2] [MAX_BLOCK_SZ];
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// double-buffered LZ4 compression output stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BLOCK_SZ>
class lz4_ostream : public kvr::ostream
{
#if KVR_CPP11
  static_assert (MAX_BLOCK_SZ <= (64 * 1024), "block size can't be more than 64kb");
#endif

public:

  lz4_ostream (size_t size = 1024) : m_os (size), m_idx (0), m_pos (0)
  {
    LZ4_resetStream (&m_lz4s);
  }

  void put (uint8_t byte)
  {
    if (m_pos >= MAX_BLOCK_SZ)
    {
      this->compress_buf (m_buf [m_idx], m_pos);
      this->swap_buf ();
    }

    m_buf [m_idx][m_pos++] = byte;
  }

  void write (uint8_t *bytes, size_t count)
  {
    if ((m_pos + count) > MAX_BLOCK_SZ)
    {
      this->compress_buf (m_buf [m_idx], m_pos);
      this->swap_buf ();
    }

    while (count >= MAX_BLOCK_SZ)
    {
      memcpy (&m_buf [m_idx] [m_pos], bytes, MAX_BLOCK_SZ);
      m_pos += MAX_BLOCK_SZ;

      this->compress_buf (m_buf [m_idx], m_pos);
      this->swap_buf ();

      bytes += MAX_BLOCK_SZ;
      count -= MAX_BLOCK_SZ;
    }

    memcpy (&m_buf [m_idx][m_pos], bytes, count);
    m_pos += count;
  }

  void flush ()
  {
    if (m_pos > 0)
    {
      this->compress_buf (m_buf [m_idx], m_pos);
      this->swap_buf ();
    }

    m_os.flush ();
  }

  const uint8_t *data () const
  {
    return m_os.buffer ();
  }

  size_t size () const
  {
    return m_os.tell ();
  }

private:

  void compress_buf (const char *src, size_t sz)
  {
    const size_t cbufsz = LZ4_COMPRESSBOUND (MAX_BLOCK_SZ);
    uint8_t *c = m_os.push (sizeof (int) + cbufsz);
    if (!c)
    {
      return;
    }

    int *csz = reinterpret_cast<int *>(c);
    char *cbuf = reinterpret_cast<char *>(c + sizeof (int));

    *csz = LZ4_compress_fast_continue (&m_lz4s, src, cbuf, (int) sz, cbufsz, 1);
    if (*csz > 0)
    {
      m_os.pop (cbufsz - *csz);
    }
    else
    {
      m_os.pop (sizeof (int) + cbufsz);
    }
  }

  void swap_buf ()
  {
    m_idx = (m_idx + 1) % 2;
    m_pos = 0;
  }

  LZ4_stream_t      m_lz4s;
  kvr::mem_ostream  m_os;
  size_t            m_pos;
  uint8_t           m_idx;
  char              m_buf [2] [MAX_BLOCK_SZ];
};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
