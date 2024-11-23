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
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  // A SINGLE TABLE DATA
  template <typename Table>
  struct single_table_data_t
  {
    single_table_data_t(Table table) : _table(table)
    {
    }

    single_table_data_t(const single_table_data_t&) = default;
    single_table_data_t(single_table_data_t&&) = default;
    single_table_data_t& operator=(const single_table_data_t&) = default;
    single_table_data_t& operator=(single_table_data_t&&) = default;
    ~single_table_data_t() = default;

    Table _table;
  };

  // A SINGLE TABLE
  template <typename Table>
  struct single_table_t
  {
    using _data_t = single_table_data_t<Table>;

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

  template <typename Table>
  struct nodes_of<single_table_t<Table>>
  {
    using type = detail::type_vector<Table>;
  };

  template<typename Table>
  struct provided_tables_of<single_table_t<Table>> : public provided_tables_of<Table>
  {
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_table_arg_is_table_t, "argument for update() must be a table");
  template <typename Table>
  struct check_update_table
  {
    using type = static_combined_check_t<static_check_t<is_raw_table<Table>::value, assert_update_table_arg_is_table_t>>;
  };
  template <typename Table>
  using check_update_table_t = typename check_update_table<Table>::type;

  // NO TABLE YET
  struct no_single_table_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;

    // Data
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
      using _new_statement_t = new_statement_t<Check, Policies, no_single_table_t, T>;

      using _consistency_check = consistent_t;

      template <typename Table>
      auto single_table(Table table) const -> _new_statement_t<check_update_table_t<Table>, single_table_t<Table>>
      {
        return _single_table_impl(check_update_table_t<Table>{}, std::move(table));
      }

    private:
      template <typename Check, typename Table>
      auto _single_table_impl(Check, Table table) const -> inconsistent<Check>;

      template <typename Table>
      auto _single_table_impl(consistent_t /*unused*/, Table table) const
          -> _new_statement_t<consistent_t, single_table_t<Table>>
      {
        static_assert(required_tables_of_t<single_table_t<Table>>::is_empty(),
                      "argument depends on another table in single_table()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), single_table_data_t<Table>{table}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Table>
  auto to_sql_string(Context& context, const single_table_data_t<Table>& t) -> std::string
  {
    return to_sql_string(context, t._table);
  }

  template <typename T>
  auto single_table(T t) -> decltype(statement_t<no_single_table_t>().single_table(t))
  {
    return statement_t<no_single_table_t>().single_table(std::move(t));
  }
}  // namespace sqlpp
