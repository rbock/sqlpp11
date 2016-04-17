/*
 * Copyright (c) 2013-2016, Roland Bock
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

#ifndef SQLPP_VERBATIM_TABLE_H
#define SQLPP_VERBATIM_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types/no_value.h>

namespace sqlpp
{
  namespace detail
  {
    struct unusable_pseudo_column_t
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "pseudo_column";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
        };
      };
      using _traits = make_traits<no_value_t>;
    };
  }

  struct verbatim_table_t : public table_t<verbatim_table_t, detail::unusable_pseudo_column_t>
  {
    using _nodes = detail::type_vector<>;

    struct _alias_t
    {
      static constexpr const char _literal[] = "verbatim_table";  // FIXME need to use alias for verbatim table
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
    };

    verbatim_table_t(std::string representation) : _representation(representation)
    {
    }

    verbatim_table_t(const verbatim_table_t& rhs) = default;
    verbatim_table_t(verbatim_table_t&& rhs) = default;
    verbatim_table_t& operator=(const verbatim_table_t& rhs) = default;
    verbatim_table_t& operator=(verbatim_table_t&& rhs) = default;
    ~verbatim_table_t() = default;

    std::string _representation;
  };

  template <typename Context>
  struct serializer_t<Context, verbatim_table_t>
  {
    using _serialize_check = consistent_t;
    using T = verbatim_table_t;

    static Context& _(const T& t, Context& context)
    {
      context << t._representation;
      return context;
    }
  };

  inline verbatim_table_t verbatim_table(std::string name)
  {
    return {name};
  }
}

#endif
