/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
* Copyright (c) 2015 Ubaka Onyechi
*
* kvr is free software distributed under the MIT license.
* See https://raw.githubusercontent.com/uonyx/kvr/master/LICENSE for details.
*/

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "streams.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void example_encode_stream ()
{
  // create a kvr context
  kvr::ctx *ctx = kvr::ctx::create ();

  // create value
  kvr::value *val = ctx->create_value ();

  // transcode from a JSON file to a MSGPACK file
  {
    file_istream<BUFSIZ> inputfile ("data/ARN-x.json");
    if (val->decode (kvr::CODEC_JSON, inputfile))
    {
      printf ("%s\n", "FILE: Decoded to \"ARN-x.json\"");

      file_ostream<BUFSIZ> outputfile ("data/ARN-x.msgpack");
      if (val->encode (kvr::CODEC_MSGPACK, &outputfile))
      {
        printf ("%s\n", "FILE: Encoded from \"ARN-x.msgpack\"");
      }
    }
  }

  // encode to hexadecimal string of CBOR-encoded value
  {
    hex_ostream cborhex;
    if (val->encode (kvr::CODEC_CBOR, &cborhex))
    {
      const char *hexstr = cborhex.string ();
      size_t hexstrlen = cborhex.length ();
      printf ("CBOR-HEX: %s [%zu]\n", hexstr, hexstrlen);
    }
  }

#if KVR_EXAMPLE_HAVE_OPENSSL
  // encode to SHA1 string of JSON-encoded value
  {
    sha1_ostream jsonsha1;
    if (val->encode (kvr::CODEC_JSON, &jsonsha1))
    {
      const char *sha1str = jsonsha1.string ();
      size_t sha1strlen = jsonsha1.length ();
      printf ("JSON-SHA1: %s [%zu]\n", sha1str, sha1strlen);
    }
  }
#endif

#if KVR_EXAMPLE_HAVE_ZLIB
  // encode JSON-encoded value to GZIP file
  {
    gzip_file_ostream<4096> gzfileos ("data/ARN-x.json.gz");
    if (val->encode (kvr::CODEC_JSON, &gzfileos))
    {
      printf ("%s\n", "JSON-GZIP FILE: Encoded to \"ARN-x.json.gz\"");
      gzfileos.close ();

      gzip_file_istream<4096> gzfileis ("data/ARN-x.json.gz");
      if (val->decode (kvr::CODEC_JSON, gzfileis))
      {
        printf ("%s\n", "JSON-GZIP FILE: Encoded from \"ARN-x.json.gz\"");
      }
    }
  }

  // encode JSON-encoded value to GZIP memory stream
  {
    gzip_ostream<4096> gzos;
    if (val->encode (kvr::CODEC_MSGPACK, &gzos))
    {
      size_t rsize = val->encode_bound (kvr::CODEC_MSGPACK);
      size_t csize = gzos.size ();
      printf ("MSGPACK-GZIP: Raw Size (Estimate): %zu, Compressed Size: %zu\n", rsize, csize);

      // verify decode
      {
        kvr::value *val2 = ctx->create_value ();
        gzip_istream<4096> gzis (gzos.data (), gzos.size ());
        if (val2->decode (kvr::CODEC_MSGPACK, gzis))
        {
          uint32_t hash1 = val->hash ();
          uint32_t hash2 = val2->hash ();
          printf ("MSGPACK-GZIP: Verify [%s]\n", (hash1 == hash2) ? "YES" : "NO");
        }
        ctx->destroy_value (val2);
      }
    }
  }
#endif

#if KVR_EXAMPLE_HAVE_LZ4
  // encode MSGPACK-encoded value to LZ4 memory stream
  {
    lz4_ostream<4096> lz4os;
    if (val->encode (kvr::CODEC_MSGPACK, &lz4os))
    {
      size_t rsize = val->encode_bound (kvr::CODEC_MSGPACK);
      size_t csize = lz4os.size ();
      printf ("MSGPACK-LZ4: Raw Size (Estimate): %zu, Compressed Size: %zu \n", rsize, csize);

      // verify decode
      {
        kvr::value *val2 = ctx->create_value ();
        lz4_istream<4096> lz4is (lz4os.data (), lz4os.size ());
        if (val2->decode (kvr::CODEC_MSGPACK, lz4is))
        {
          uint32_t hash1 = val->hash ();
          uint32_t hash2 = val2->hash ();
          printf ("MSGPACK-LZ4: Verify [%s]\n", (hash1 == hash2) ? "YES" : "NO");
        }
        ctx->destroy_value (val2);
      }
    }
  }
#endif

  // clean up
  ctx->destroy_value (val);

  // destroy context
  kvr::ctx::destroy (ctx);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int main ()
{
  example_encode_stream ();

  return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
