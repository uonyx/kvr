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

#define KVR_EXAMPLE_HAVE_OPENSSL 0

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "../src/kvr.h"
#include <new>
#include <stdio.h>
#if KVR_EXAMPLE_HAVE_OPENSSL
#include <openssl/sha.h>
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
    static const char lut [] = "0123456789abcdef";
    char hi = lut [(byte >> 4)];
    char lo = lut [(byte & 15)];
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

private:

  hex_ostream (const hex_ostream &);
  hex_ostream &operator=(const hex_ostream &);

  kvr::mem_ostream m_os;
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// SHA1 string generator stream
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#if KVR_EXAMPLE_HAVE_OPENSSL
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

private:

  sha1_ostream (const sha1_ostream &);
  sha1_ostream &operator=(const sha1_ostream &);

  char     m_str [64];
  size_t   m_len;
  SHA_CTX  m_ctx;
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// standard input file stream with buffering
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t MAX_BUF_SZ = BUFSIZ>
class file_istream : public kvr::istream
{
public:

  file_istream (const char *filename) : m_fp (0)
  {
    this->open (filename);
  }

  ~file_istream ()
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
      if (m_fp)
      {
        setvbuf (m_fp, m_buf, _IOFBF, MAX_BUF_SZ);
        return true;
      }
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
#ifdef _MSC_VER
    fread_s (byte, 1, sizeof (uint8_t), 1, m_fp);
#else
    fread (byte, sizeof (uint8_t), 1, m_fp);
#endif
    return true;
  }

  bool read (uint8_t *bytes, size_t count)
  {
#ifdef _MSC_VER
    fread_s (bytes, count, sizeof (uint8_t), count, m_fp);
#else
    fread (bytes, sizeof (uint8_t), count, m_fp);
#endif
    return true;
  }

  size_t tell () const
  {
    long p = ftell (m_fp);
    return static_cast<size_t>(p);
  }

  uint8_t peek () const
  {
    int c = fgetc (m_fp);
    ungetc (c, m_fp);
    return (c == EOF) ? 0u : (uint8_t) c;
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
// standard output file stream with buffering
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
      if (m_fp)
      {
        setvbuf (m_fp, m_buf, _IOFBF, MAX_BUF_SZ);
        return true;
      }
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
    fwrite (&byte, sizeof (uint8_t), 1, m_fp);
  }

  void write (uint8_t *bytes, size_t count)
  {
    fwrite (bytes, sizeof (uint8_t), count, m_fp);
  }

  void flush ()
  {
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
// buffered input file stream : same as file_istream but with manual buffering. done for fun OR
//                            : optimization opportunity (for faster memcpy using SIMD maybe?)
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

  size_t tell () const
  {
    if (m_pos >= m_sz)
    {
      long p = ftell (m_fp);
      return static_cast<size_t>(p);
    }

    return m_pos;
  }

  uint8_t peek () const
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
// buffered output file stream : same as file_ostream but with manual buffering. done for fun OR
//                             : optimization opportunity (for faster memcpy using SIMD maybe?)
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

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
