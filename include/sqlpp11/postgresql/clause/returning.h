#pragma once

/**
 * Copyright © 2014-2018, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp11/postgresql/clause/returning_column_list.h>
#include <sqlpp11/core/clause/select_column_list.h>
#include <sqlpp11/core/query/statement.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_returning)

  namespace postgresql
  {
    struct return_name_t
    {
    };

    struct returning_t : public statement_name_t<return_name_t, tag::is_returning>
    {
    };

    using blank_returning_t = statement_t<returning_t, no_returning_column_list_t>;

    inline blank_returning_t returning()
    {
      return {};
    }

    struct no_returning_t
    {
      using _data_t = no_data_t;

      // Base template to be inherited by the statement
      template <typename Policies>
      struct _base_t
      {
        _base_t() = default;
        _base_t(_data_t data) : _data{std::move(data)}
        {
        }

        _data_t _data;

        template <typename T>
        static auto _get_member(T t) -> decltype(t.no_returning)
        {
          return t.no_returning;
        }

        template <typename T>
        using _new_statement_t = new_statement_t<consistent_t, Policies, no_returning_t, T>;

        using _consistency_check = consistent_t;

#warning: Why do we have this at all? Isn't this the same as no_returning_column_list?
        template <typename... Columns, typename = sqlpp::enable_if_t<select_columns_have_values<Columns...>::value>>
        auto returning(Columns... columns) const -> _new_statement_t<make_returning_column_list_t<Columns...>>
        {
          SQLPP_STATIC_ASSERT(sizeof...(Columns), "at least one return column required");
          SQLPP_STATIC_ASSERT(select_columns_have_names<Columns...>::value, "each return column must have a name");

          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  typename make_returning_column_list_t<Columns...>::_data_t{
                      std::tuple_cat(sqlpp::detail::tupelize(std::move(columns))...)}};
        }
      };
    };
  }  // namespace postgresql
}  // namespace sqlpp
