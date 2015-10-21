//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "streams.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void sample_encode_stream ()
{
  // create a kvr context
  kvr::ctx *ctx = kvr::ctx::create ();

  // create a map and transcode from a json file to a msgpack file
  // using app-level memory buffered file streams
  kvr::value *val0 = ctx->create_value ();
  {
    buffered_file_istream<1024> inputfile ("data/ARN-x.json");
    
    if (val0->decode (kvr::CODEC_JSON, inputfile))
    {
      printf ("%s\n", "decoded ARN-x.json");

      buffered_file_ostream<1024> outputfile ("data/ARN-x.msgpack");

      if (val0->encode (kvr::CODEC_MSGPACK, &outputfile))
      {
        printf ("%s\n", "encoded ARN-x.msgpack");
      }

      outputfile.close ();
    }

    inputfile.close ();
  }

  // let's transcode again but this time the reverse (msgpack back to json)
  // using standard file streams
  kvr::value *val1 = ctx->create_value ();
  {
    file_istream inputfile ("data/ARN-x.msgpack");

    if (val1->decode (kvr::CODEC_MSGPACK, inputfile))
    {
      printf ("%s\n", "decoded ARN-x.msgpack");

      file_ostream outputfile ("data/ARN-x.2.json");

      if (val1->encode (kvr::CODEC_JSON, &outputfile))
      {
        printf ("%s\n", "encoded ARN-x.2.json");
      }

      outputfile.close ();
    }

    inputfile.close ();
  }

  // clean up
  ctx->destroy_value (val0);
  ctx->destroy_value (val1);

  // destroy context
  kvr::ctx::destroy (ctx);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int main ()
{
  sample_encode_stream ();

  return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
