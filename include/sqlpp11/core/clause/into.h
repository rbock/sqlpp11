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
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/database/prepared_insert.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // A SINGLE TABLE DATA
  template <typename Table>
  struct into_data_t
  {
    into_data_t(Table table) : _table(table)
    {
    }

    into_data_t(const into_data_t&) = default;
    into_data_t(into_data_t&&) = default;
    into_data_t& operator=(const into_data_t&) = default;
    into_data_t& operator=(into_data_t&&) = default;
    ~into_data_t() = default;

    Table _table;
  };

  // A SINGLE TABLE
  template <typename Table>
  struct into_t
  {
    using _data_t = into_data_t<Table>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = consistent_t;
    };
  };

  template<typename Table>
    struct nodes_of<into_t<Table>>
    {
      using type = detail::type_vector<Table>;
    };

  template<typename Table>
  struct provided_tables_of<into_t<Table>> : public provided_tables_of<Table>
  {
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_into_t, "into() required");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_into_arg_is_table, "argument for into() must be a table");
  template <typename T>
  struct check_into
  {
    using type = static_combined_check_t<static_check_t<is_raw_table<T>::value, assert_into_arg_is_table>>;
  };
  template <typename T>
  using check_into_t = typename check_into<T>::type;

  // NO INTO YET
  struct no_into_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

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

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_into_t, T>;

      using _consistency_check = assert_into_t;

      template <typename Table>
      auto into(Table table) const -> _new_statement_t<check_into_t<Table>, into_t<Table>>
      {
        return _into_impl(check_into_t<Table>{}, table);
      }

    private:
      template <typename Check, typename Table>
      auto _into_impl(Check, Table table) const -> inconsistent<Check>;

      template <typename Table>
      auto _into_impl(consistent_t /*unused*/, Table table) const
          -> _new_statement_t<consistent_t, into_t<Table>>
      {
        static_assert(required_tables_of_t<into_t<Table>>::is_empty(),
                      "argument depends on another table in into()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), into_data_t<Table>{table}};
      }
    };
  };

  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const into_data_t<Table>& t) -> std::string
  {
    return " INTO " + to_sql_string(context, t._table);
  }

  template <typename T>
  auto into(T&& t) -> decltype(statement_t<no_into_t>().into(std::forward<T>(t)))
  {
    return statement_t<no_into_t>().into(std::forward<T>(t));
  }
}  // namespace sqlpp
