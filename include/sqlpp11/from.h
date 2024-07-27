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

#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/table_ref.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // FROM DATA
  template <typename Table>
  struct from_data_t
  {
    Table _table;
  };

  // FROM
  template <typename Table>
  struct from_t
  {
    using _traits = make_traits<no_value_t, tag::is_from>;

    using _data_t = from_data_t<Table>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      // FIXME: We might want to check if we have too many tables define in the FROM
      using _consistency_check = consistent_t;
    };
  };

  template<typename Table>
  struct nodes_of<from_t<Table>>
  {
    using type = detail::type_vector<Table>;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_from_not_pre_join_t,
      "from() argument is a pre join, please use an explicit on() condition or unconditionally()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_from_table_t, "from() argument has to be a table or join expression");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_from_dependency_free_t, "at least one table depends on another table in from()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_from_no_duplicates_t, "at least one duplicate table name detected in from()");

  template <typename Table>
  struct check_from
  {
    using type = static_combined_check_t<
        static_check_t<not is_pre_join_t<Table>::value, assert_from_not_pre_join_t>,
        static_check_t<is_table<Table>::value, assert_from_table_t>,
        static_check_t<required_tables_of_t<Table>::size::value == 0, assert_from_dependency_free_t>,
        static_check_t<provided_tables_of<Table>::size::value ==
                           detail::make_name_of_set_t<provided_tables_of<Table>>::size::value,
                       assert_from_no_duplicates_t>>;
  };

  template <typename Table>
  using check_from_t = typename check_from<Table>::type;

  template <typename Table>
  using check_from_static_t = check_from_t<Table>;

  struct no_from_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;

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
      using _new_statement_t = new_statement_t<Check, Policies, no_from_t, T>;

      using _consistency_check = consistent_t;

      template <typename Table>
      auto from(Table table) const -> _new_statement_t<check_from_static_t<Table>, from_t<from_table_t<Table>>>
      {
        using Check = check_from_static_t<Table>;
        return _from_impl(Check{}, table);
      }

    private:
      template <typename Check, typename Table>
      auto _from_impl(Check, Table table) const -> inconsistent<Check>;

      template <typename Table>
      auto _from_impl(consistent_t /*unused*/, Table table) const
          -> _new_statement_t<consistent_t, from_t<from_table_t<Table>>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                from_data_t<from_table_t<Table>>{from_table(table)}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Table>
  Context& serialize(Context& context, const from_data_t<Table>& t)
  {
    context << " FROM ";
    serialize(context, t._table);
    return context;
  }

  template <typename T>
  auto from(T&& t) -> decltype(statement_t<no_from_t>().from(std::forward<T>(t)))
  {
    return statement_t<no_from_t>().from(std::forward<T>(t));
  }

}  // namespace sqlpp
