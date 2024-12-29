#pragma once

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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>

#warning: move into namespace postgresql
namespace sqlpp
{
  // USING
  template <typename... Tables>
  struct using_t
  {
    using_t(Tables... tables) : _tables(tables...)
    {
    }

    using_t(const using_t&) = default;
    using_t(using_t&&) = default;
    using_t& operator=(const using_t&) = default;
    using_t& operator=(using_t&&) = default;
    ~using_t() = default;

    std::tuple<Tables...> _tables;
  };

  template <typename Statement, typename... Tables>
  struct consistency_check<Statement, using_t<Tables...>>
  {
    using type = consistent_t;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_using_args_are_tables_t, "arguments for using() must be tables");
  template <typename... Tables>
  struct check_using
  {
    using type = static_combined_check_t<
        static_check_t<logic::all<is_table<Tables>::value...>::value, assert_using_args_are_tables_t>>;
  };
  template <typename... Tables>
  using check_using_t = typename check_using<Tables...>::type;

  // NO USING YET
  struct no_using_t
  {
  };

  template <typename Statement>
  struct clause_base<no_using_t, Statement> : public clause_data<no_using_t, Statement>
  {
    using clause_data<no_using_t, Statement>::clause_data;

      template <typename... Tables>
      auto using_(Tables... tables) const -> decltype(new_statement(*this, using_t<Tables...>{std::move(tables)...}))
      {
#warning: reactivate check_using
        static_assert(not detail::has_duplicates<Tables...>::value,
                      "at least one duplicate argument detected in using()");
        static_assert(sizeof...(Tables), "at least one table required in using()");

        return new_statement(*this, using_t<Tables...>{std::move(tables)...});
      }

  };

  template <typename Statement>
  struct consistency_check<Statement, no_using_t>
  {
    using type = consistent_t;
  };

  // Serialization
  template <typename Context>
  auto to_sql_string(Context& , const no_using_t& ) -> std::string
  {
    return "";
  }

  template <typename Context, typename... Tables>
  auto to_sql_string(Context& context, const using_t<Tables...>& t) -> std::string
  {
    return " USING " + tuple_to_sql_string(context, t._tables, tuple_operand{", "});
  }
#warning: Move this to postgresql as neither mysql nor sqlite support it, I think?
}  // namespace sqlpp
