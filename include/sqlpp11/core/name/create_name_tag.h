#pragma once

/*
 * Copyright (c) 2013, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/core/name/char_sequence.h>
#include <sqlpp11/core/type_traits.h>

#define SQLPP_NAME_TAG_GUTS(SQL_NAME, CPP_NAME)   \
  static constexpr const char name[] = #SQL_NAME; \
  template <typename T>                           \
  struct _member_t                                \
  {                                               \
    T CPP_NAME = {};                              \
    T& operator()()                               \
    {                                             \
      return CPP_NAME;                            \
    }                                             \
    const T& operator()() const                   \
    {                                             \
      return CPP_NAME;                            \
    }                                             \
  }

#define SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(SQL_NAME, CPP_NAME) \
  struct _sqlpp_name_tag                                          \
  {                                                               \
    SQLPP_NAME_TAG_GUTS(SQL_NAME, CPP_NAME);               \
  }

#define SQLPP_CREATE_NAME_TAG(NAME)                    \
  struct NAME##_t : public ::sqlpp::name_tag_base      \
  {                                                    \
    SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(NAME, NAME); \
  };                                                   \
  constexpr auto NAME = NAME##_t                       \
  {                                                    \
  }

#define SQLPP_QUOTED_NAME_TAG_GUTS(SQL_NAME, CPP_NAME) \
  struct _quoted_name_t                                \
  {                                                    \
    bool requires_quotes = true;                       \
    static constexpr const char name[] = #SQL_NAME;    \
  };                                                   \
  static constexpr _quoted_name name;                  \
  template <typename T>                                \
  struct _member_t                                     \
  {                                                    \
    T CPP_NAME = {};                                   \
    T& operator()()                                    \
    {                                                  \
      return CPP_NAME;                                 \
    }                                                  \
    const T& operator()() const                        \
    {                                                  \
      return CPP_NAME;                                 \
    }                                                  \
  }

#warning: Need to add tests with quoted names
#define SQLPP_CREATE_QUOTED_NAME_TAG_FOR_SQL_AND_CPP(SQL_NAME, CPP_NAME) \
  struct _sqlpp_name_tag                                                 \
  {                                                                      \
    SQLPP_NAME_TAG_GUTS(SQL_NAME, CPP_NAME);                             \
  }

#define SQLPP_CREATE_QUOTED_NAME_TAG(NAME)             \
  struct NAME##_t : public ::sqlpp::name_tag_base      \
  {                                                    \
    SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(NAME, NAME); \
  };                                                   \
  constexpr auto NAME = NAME##_t                       \
  {                                                    \
  }

namespace sqlpp
{
  namespace alias
  {
    SQLPP_CREATE_NAME_TAG(a);
    SQLPP_CREATE_NAME_TAG(b);
    SQLPP_CREATE_NAME_TAG(c);
    SQLPP_CREATE_NAME_TAG(d);
    SQLPP_CREATE_NAME_TAG(e);
    SQLPP_CREATE_NAME_TAG(f);
    SQLPP_CREATE_NAME_TAG(g);
    SQLPP_CREATE_NAME_TAG(h);
    SQLPP_CREATE_NAME_TAG(i);
    SQLPP_CREATE_NAME_TAG(j);
    SQLPP_CREATE_NAME_TAG(k);
    SQLPP_CREATE_NAME_TAG(l);
    SQLPP_CREATE_NAME_TAG(m);
    SQLPP_CREATE_NAME_TAG(n);
    SQLPP_CREATE_NAME_TAG(o);
    SQLPP_CREATE_NAME_TAG(p);
    SQLPP_CREATE_NAME_TAG(q);
    SQLPP_CREATE_NAME_TAG(r);
    SQLPP_CREATE_NAME_TAG(s);
    SQLPP_CREATE_NAME_TAG(t);
    SQLPP_CREATE_NAME_TAG(u);
    SQLPP_CREATE_NAME_TAG(v);
    SQLPP_CREATE_NAME_TAG(w);
    SQLPP_CREATE_NAME_TAG(x);
    SQLPP_CREATE_NAME_TAG(y);
    SQLPP_CREATE_NAME_TAG(z);
    SQLPP_CREATE_NAME_TAG(left);
    SQLPP_CREATE_NAME_TAG(right);
  }  // namespace alias
}  // namespace sqlpp
