/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Copyright (c) 2015 Ubaka Onyechi
 *
 * kvr is free software distributed under the MIT license.
 * See LICENSE file for details.
 */

 /////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef KVR_INTERNAL_H
#define KVR_INTERNAL_H

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cmath>
#include <limits>

#include "../kvr.h"
#include "rapidjson/internal/itoa.h"
#include "rapidjson/internal/dtoa.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#if KVR_DEBUG
#include <cassert>
#define KVR_ASSERT(X) assert(X)
#define KVR_ASSERT_SAFE(X, R) KVR_ASSERT(X)
#else
#define KVR_ASSERT(X) 
#define KVR_ASSERT_SAFE(X, R) do { if (!(X)) { return (R); } } while (0,0)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#define KVR_REF_UNUSED(X) (void)(X)

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#if (RAPIDJSON_ENDIAN == RAPIDJSON_LITTLEENDIAN)
#define KVR_LITTLE_ENDIAN
#else
#define KVR_BIG_ENDIAN
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (_MSC_VER)
#define kvr_strcpy(dst, dsz, src) strcpy_s (dst, dsz, src)
#define kvr_strncpy(dst, dsz, src, n) strncpy_s (dst, dsz, src, n)
#define kvr_strdup(src, sz) _strdup (src)
#else
#define kvr_strcpy(dst, dsz, src) { strncpy (dst, src, dsz); dst [dsz - 1] = 0; }
#define kvr_strncpy(dst, dsz, src, n) strncpy(dst, src, n)
#define kvr_strdup(src, sz) strndup (src, sz)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

namespace kvr
{
  namespace internal
  {
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    size_t u32toa (uint32_t u32, char dest [11])
    {
      const char* end = kvr_rapidjson::internal::u32toa (u32, dest);
      return (end - dest);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    size_t u64toa (int64_t i64, char dest [21])
    {
      const char* end = kvr_rapidjson::internal::u64toa (i64, dest);
      return (end - dest);
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    size_t i64toa (int64_t i64, char dest [22])
    {
      const char* end = kvr_rapidjson::internal::i64toa (i64, dest);
      return (end - dest);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    uint32_t ndigitsu32 (uint32_t u32)
    {
      uint32_t count = kvr_rapidjson::internal::CountDecimalDigit32 (u32);
      return count;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    uint32_t ndigitsi64 (int64_t i64)
    {
#if 0
      int64_t n = i64;
      uint32_t count = (n < 0) ? 1 : 0; // sign
      do
      {
        ++count;
        n /= 10;
      } while (n);

      return count;
#else
      uint32_t neg = 0;
      uint64_t u64 = static_cast<uint64_t>(i64);
      if (i64 < 0)
      {
        neg = 1;
        u64 = ~u64 + 1;
      }

      if (u64 < 10ULL) return 1 + neg;
      if (u64 < 100ULL) return 2 + neg;
      if (u64 < 1000ULL) return 3 + neg;
      if (u64 < 10000ULL) return 4 + neg;
      if (u64 < 100000ULL) return 5 + neg;
      if (u64 < 1000000ULL) return 6 + neg;
      if (u64 < 10000000ULL) return 7 + neg;
      if (u64 < 100000000ULL) return 8 + neg;
      if (u64 < 1000000000ULL) return 9 + neg;
      if (u64 < 10000000000ULL) return 10 + neg;
      if (u64 < 100000000000ULL) return 11 + neg;
      if (u64 < 1000000000000ULL) return 12 + neg;
      if (u64 < 10000000000000ULL) return 13 + neg;
      if (u64 < 100000000000000ULL) return 14 + neg;
      if (u64 < 1000000000000000ULL) return 15 + neg;
      if (u64 < 10000000000000000ULL) return 16 + neg;
      if (u64 < 100000000000000000ULL) return 17 + neg;
      if (u64 < 1000000000000000000ULL) return 18 + neg;
      if (u64 < 10000000000000000000ULL) return 19 + neg;
      return 20 + neg;
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    uint16_t byteswap16 (uint16_t val)
    {
      uint16_t swap = (uint16_t) ((val >> 8) | (val << 8));
      return swap;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    uint32_t byteswap32 (uint32_t val)
    {
      uint32_t swap = (uint32_t) (((val >> 24) & 0x000000ff) |
                                  ((val >> 8) & 0x0000ff00) |
                                  ((val << 8) & 0x00ff0000) |
                                  ((val << 24) & 0xff000000));
      return swap;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    uint64_t byteswap64 (uint64_t val)
    {
      uint64_t swap = (uint64_t) (((val >> 56) & 0x00000000000000ff) |
                                  ((val >> 40) & 0x000000000000ff00) |
                                  ((val >> 24) & 0x0000000000ff0000) |
                                  ((val >> 8) & 0x00000000ff000000) |
                                  ((val << 8) & 0x000000ff00000000) |
                                  ((val << 24) & 0x0000ff0000000000) |
                                  ((val << 40) & 0x00ff000000000000) |
                                  ((val << 56) & 0xff00000000000000));
      return swap;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    uint32_t strhash (const char *str)
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

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    bool hex_encode (const uint8_t *data, size_t size, kvr::mem_ostream *ostr)
    {
      static const char lut [] = "0123456789abcdef";

      bool success = false;

      if (data && (size > 0))
      {
        size_t tcap = (size * 2) + 1;
        ostr->reserve (tcap);

        for (size_t i = 0; i < size; ++i)
        {
          uint8_t c = data [i];
          char hi = lut [(c >> 4)];
          char lo = lut [(c & 15)];
          ostr->put (hi);
          ostr->put (lo);
        }

        success = true;
      }

      return success;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    const T& min (const T& a, const T& b)
    {
      return (a < b) ? a : b;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    const T& max (const T& a, const T& b)
    {
      return (a > b) ? a : b;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    
    sz_t align_size (sz_t s, sz_t align)
    {
      sz_t a = align - 1;
      sz_t as = (s + a) & ~a;
      return as;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    bool isnan (double f)
    {
      // quick nan check (valid for IEEE fp mode). 
      // TODO: more robust nan check

#if KVR_CPP11
      return std::isnan (f);
#else
#if defined (_MSC_VER)
      return (_isnan (f) != 0);
#else
      return (f != f); //return std::tr1::isnan (f);
#endif
#endif
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    // floating point nan check
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "kvr_msgpack.h"
#include "kvr_json.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
