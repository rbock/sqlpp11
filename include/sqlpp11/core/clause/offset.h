#pragma once

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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename Offset>
  struct offset_t
  {
    offset_t(Offset value) : _value(value)
    {
    }

    offset_t(const offset_t&) = default;
    offset_t(offset_t&&) = default;
    offset_t& operator=(const offset_t&) = default;
    offset_t& operator=(offset_t&&) = default;
    ~offset_t() = default;

    Offset _value;
  };

  template <typename Offset>
  struct is_clause<offset_t<Offset>> : public std::true_type
  {
  };

  template<typename Statement, typename Offset>
    struct consistency_check<Statement, offset_t<Offset>> { using type = consistent_t; };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_offset_is_integral,
                               "argument for offset() must be an integral expressions");
  template <typename T>
  struct check_offset
  {
    using type =
        static_combined_check_t<static_check_t<is_integral<T>::value or is_unsigned_integral<T>::value, assert_offset_is_integral>>;
  };
  template <typename T>
  using check_offset_t = typename check_offset<remove_dynamic_t<T>>::type;

  struct no_offset_t
  {
  };

  template <typename Statement>
  struct clause_base<no_offset_t, Statement> : public clause_data<no_offset_t, Statement>
  {
    using clause_data<no_offset_t, Statement>::clause_data;

#warning : reactivate check_offset_t
    template <typename Arg>
    auto offset(Arg arg) const -> decltype(new_statement(*this, offset_t<Arg>{std::move(arg)}))
    {
      return new_statement(*this, offset_t<Arg>{std::move(arg)});
    }
  };

  template<typename Statement>
    struct consistency_check<Statement, no_offset_t> { using type = consistent_t; };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const no_offset_t&) -> std::string
  {
    return "";
  }

  template <typename Context, typename Offset>
  auto to_sql_string(Context& context, const offset_t<Offset>& t) -> std::string
  {
    return  " OFFSET " +  operand_to_sql_string(context, t._value);
  }

  template <typename Context, typename Offset>
  auto to_sql_string(Context& context, const offset_t<dynamic_t<Offset>>& t) -> std::string
  {
    if (not t._value._condition)
    {
      return "";
    }
    return  " OFFSET " + operand_to_sql_string(context, t._value._expr);
  }

  template <typename T>
  auto offset(T&& t) -> decltype(statement_t<no_offset_t>().offset(std::forward<T>(t)))
  {
    return statement_t<no_offset_t>().offset(std::forward<T>(t));
  }

}  // namespace sqlpp
