/*
 * Copyright (c) 2013-2015, Roland Bock
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

#ifndef SQLPP_VERBATIM_H
#define SQLPP_VERBATIM_H

#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/serialize.h>

namespace sqlpp
{
  template <typename ValueType>  // Csaba Csoma suggests: unsafe_sql instead of verbatim
  struct verbatim_t : public expression_operators<verbatim_t<ValueType>, ValueType>,
                      public alias_operators<verbatim_t<ValueType>>
  {
    using _traits = make_traits<ValueType, tag::is_expression>;
    using _nodes = detail::type_vector<>;
    using _can_be_null =
        std::true_type;  // since we do not know what's going on inside the verbatim, we assume it can be null

    verbatim_t(std::string verbatim) : _verbatim(verbatim)
    {
    }
    verbatim_t(const verbatim_t&) = default;
    verbatim_t(verbatim_t&&) = default;
    verbatim_t& operator=(const verbatim_t&) = default;
    verbatim_t& operator=(verbatim_t&&) = default;
    ~verbatim_t() = default;

    std::string _verbatim;
  };

  template <typename Context, typename ValueType>
  struct serializer_t<Context, verbatim_t<ValueType>>
  {
    using _serialize_check = consistent_t;
    using T = verbatim_t<ValueType>;

    static Context& _(const T& t, Context& context)
    {
      context << t._verbatim;
      return context;
    }
  };

  template <typename ValueType, typename StringType>
  auto verbatim(StringType s) -> verbatim_t<ValueType>
  {
    return {s};
  }

  inline auto verbatim(std::string s) -> verbatim_t<no_value_t>
  {
    return {s};
  }
}

#endif
