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

#include "../src/kvr.h"
#include <cstdio>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//
// SimpleVariant is a skeleton-wrapper class that demonstrates 
// how kvr can be used to create a higher-level variant system
//
// SimpleVariant can be extended with more features like; better
// type deduction for operator[] indexing; better error-handling
// and exception handling; more serialization options; better 
// performance; and more.

class SimpleVariant
{
public:

  SimpleVariant (int32_t i) : m_val (NULL)
  {
    if (m_ctx)
    {
      m_val = m_ctx->create_value ()->conv_integer ();
      m_val->set_integer (i);
    }
  }

  SimpleVariant (int64_t i) : m_val (NULL)
  {
    if (m_ctx)
    {
      m_val = m_ctx->create_value ()->conv_integer ();
      m_val->set_integer (i);
    }
  }

  SimpleVariant (double f) : m_val (NULL)
  {
    if (m_ctx)
    {
      m_val = m_ctx->create_value ()->conv_float ();
      m_val->set_float (f);
    }
  }

  SimpleVariant (bool b) : m_val (NULL)
  {
    if (m_ctx)
    {
      m_val = m_ctx->create_value ()->conv_boolean ();
      m_val->set_boolean (b);
    }
  }

  SimpleVariant (const std::string &str) : m_val (NULL)
  {
    if (m_ctx)
    {
      m_val = m_ctx->create_value ()->conv_string ();
      m_val->set_string (str.c_str (), static_cast<kvr::sz_t>(str.length ()));
    }
  }

  SimpleVariant (const SimpleVariant &var) : m_val (NULL)
  {
    m_val = m_ctx->create_value ();
    m_val->copy (var.m_val);
  }

  SimpleVariant () : m_val (NULL)
  {
    if (m_ctx)
    {
      m_val = m_ctx->create_value ()->conv_null ();
    }
  }

  ~SimpleVariant ()
  {
    if (m_ctx && m_val)
    {
      m_ctx->destroy_value (m_val);
    }
  }

  SimpleVariant &operator= (const SimpleVariant &var)
  {
    m_val->copy (var.m_val);
    return *this;
  }

  SimpleVariant operator [] (const std::string &key)
  {
    kvr::value *v = NULL;

    if (m_val->is_map ())
    {
      v = m_val->find (key.c_str ());
    }
    else
    {
      m_val->conv_map ();
    }

    if (!v)
    {
      v = m_val->insert_null (key.c_str ());
    }

    return SimpleVariant (v);
  }

  SimpleVariant operator [] (size_t index)
  {
    kvr::value *v = NULL;

    if (m_val->is_array ())
    {
      v = m_val->element (static_cast<kvr::sz_t>(index));
    }
    else
    {
      m_val->conv_array ();
    }

    if (!v)
    {
      v = m_val->push_null ();
    }

    return SimpleVariant (v);
  }

  operator std::string () const
  {
    if (m_val && m_val->is_string ())
    {
      kvr::sz_t strLen = 0;
      const char *str = m_val->get_string (&strLen);
      return std::string (str, strLen);
    }
    return std::string ();
  }

  operator double () const
  {
    if (m_val)
    {
      if (m_val->is_float ())
      {
        return m_val->get_float ();
      }
      else if (m_val->is_integer ())
      {
        return static_cast<double> (m_val->get_integer ());
      }
    }
    return 0.0;
  }

  operator int64_t () const
  {
    if (m_val)
    {
      if (m_val->is_float ())
      {
        return static_cast<int64_t> (m_val->get_float ());
      }
      else if (m_val->is_integer ())
      {
        return m_val->get_integer ();
      }
    }
    return 0;
  }

  operator bool () const
  {
    if (m_val && (m_val->is_boolean ()))
    {
      return m_val->get_boolean ();
    }
    return false;
  }

  std::string toJSON ()
  {
    kvr::obuffer buf;
    if (m_val && m_val->encode (kvr::CODEC_JSON, &buf))
    {
      return std::string (reinterpret_cast<const char *>(buf.get_data ()), buf.get_size ());
    }
    return std::string ();
  }

private:

  SimpleVariant (kvr::value *v)
  {
    m_val = v;
  }

public:

  static bool Init ()
  {
    if (!m_ctx)
    {
      m_ctx = kvr::ctx::create ();
      return true;
    }
    return false;
  }

  static void Deinit ()
  {
    if (m_ctx)
    {
      kvr::ctx::destroy (m_ctx);
      m_ctx = NULL;
    }
  }

  static kvr::ctx * m_ctx;

  kvr::value * m_val;
};

kvr::ctx * SimpleVariant::m_ctx = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void example_simple_variant ()
{
  // quirky intialise of variant system (as the result of kvr::context dependency)
  SimpleVariant::Init ();

  // declare 3 vars
  SimpleVariant var0 (true);
  SimpleVariant var1 (3.142);
  SimpleVariant var2 = 10;
  SimpleVariant var3 (var1);

  // copy operation
  var3 = var2;

  // convert var2 to an array
  var2 [0] = 5;
  var2 [1] = 15;

  // convert var3 to a map
  // explict std::string wrapper used to avoid type collision (this can be improved using templates)
  var3 [std::string ("papa")] = var1;
  var3 [std::string ("wawa")] = 67;
  var3 [std::string ("jack")] = std::string ("nimble");

  // copy var2 array to child of var3 object and serialise to JSON
  var3 [std::string ("array")] = var2;
  std::string json = var3.toJSON ();
  printf ("JSON: %s\n", json.c_str ());

  // get var from var3 map
  SimpleVariant var4 = var3 [std::string ("wawa")];

  // get raw values from vars
  bool truth = var0;
  double pi = var1;
  int sixtySeven = (int) var4;

  printf ("truth      = %s\n", truth ? "true" : "false");
  printf ("pi         = %.2f\n", pi);
  printf ("sixtySeven = %d\n", sixtySeven);

  // quirky deintialise
  SimpleVariant::Deinit ();
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int main ()
{
  example_simple_variant ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
