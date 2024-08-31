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

#include <sqlpp11/core/operator/assign_expression.h>
#include <sqlpp11/core/basic/column_fwd.h>
#include <sqlpp11/core/clause/insert_value.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/portable_static_assert.h>
#include <sqlpp11/core/clause/simple_column.h>
#include <sqlpp11/core/query/statement.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename... Columns>
    struct have_all_required_columns
    {
      static constexpr bool value = false;
    };

    template <typename Table, typename ColumnSpec, typename... Columns>
    struct have_all_required_columns<column_t<Table, ColumnSpec>, Columns...>
    {
      using First = column_t<Table, ColumnSpec>;
      using _table = typename First::_table;
      using required_columns = typename _table::_required_insert_columns;
      using set_columns = detail::make_type_set_t<First, Columns...>;
      static constexpr bool value = detail::is_subset_of<required_columns, set_columns>::value;
    };
  }  // namespace detail

  struct insert_default_values_data_t
  {
  };

  // COLUMN AND VALUE LIST
  struct insert_default_values_t
  {
    using _traits = make_traits<no_value_t>;

    // Data
    using _data_t = insert_default_values_data_t;

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

  template <typename... Assignments>
  struct insert_list_data_t
  {
    insert_list_data_t(std::tuple<Assignments...> assignments)
        : _assignments(assignments), _columns(columns_from_tuple(assignments)), _values(values_from_tuple(assignments))
    {
    }

    insert_list_data_t(const insert_list_data_t&) = default;
    insert_list_data_t(insert_list_data_t&&) = default;
    insert_list_data_t& operator=(const insert_list_data_t&) = default;
    insert_list_data_t& operator=(insert_list_data_t&&) = default;
    ~insert_list_data_t() = default;

    std::tuple<Assignments...> _assignments;  // FIXME: Need to replace _columns and _values by _assignments
                                              // (connector-container requires assignments)
    std::tuple<simple_column_t<lhs_t<Assignments>>...> _columns;
    std::tuple<rhs_t<Assignments>...> _values;

  private:
    template <size_t... Indexes>
    auto columns_from_tuple(::sqlpp::index_sequence<Indexes...>, std::tuple<Assignments...> assignments)
        -> decltype(_columns)
    {
      (void)assignments;
      return decltype(_columns)(std::get<Indexes>(assignments)._l...);
    }

    auto columns_from_tuple(std::tuple<Assignments...> assignments) -> decltype(_columns)
    {
      const auto seq = ::sqlpp::make_index_sequence<sizeof...(Assignments)>{};
      return columns_from_tuple(seq, assignments);
    }

    template <size_t... Indexes>
    auto values_from_tuple(::sqlpp::index_sequence<Indexes...>, std::tuple<Assignments...> assignments)
        -> decltype(_values)
    {
      (void)assignments;
      return decltype(_values)(std::get<Indexes>(assignments)._r...);
    }

    auto values_from_tuple(std::tuple<Assignments...> assignments) -> decltype(_values)
    {
      const auto seq = ::sqlpp::make_index_sequence<sizeof...(Assignments)>{};
      return values_from_tuple(seq, assignments);
    }
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_assignments_t, "at least one argument is not an assignment in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_no_duplicates_t, "at least one duplicate column detected in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_single_table_t,
                               "set() arguments contain assignments from more than one table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_static_set_count_args_t,
                               "at least one assignment expression required in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_static_set_all_required_t,
                               "at least one required column is missing in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_dynamic_set_statement_dynamic_t,
                               "dynamic_set must not be called in a static statement");

  template <typename... Assignments>
  using check_insert_set_t = static_combined_check_t<
      static_check_t<logic::all<is_assignment<Assignments>::value...>::value,
                     assert_insert_set_assignments_t>,
      static_check_t<not detail::has_duplicates<typename lhs<Assignments>::type...>::value,
                     assert_insert_set_no_duplicates_t>,
      static_check_t<sizeof...(Assignments) == 0 or detail::type_vector_cat_t<required_tables_of_t<
                                                        typename lhs<Assignments>::type>...>::are_same(),
                     assert_insert_set_single_table_t>>;

  // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
  //  template <typename... Assignments>
  //  using check_insert_static_set_t =
  //      static_combined_check_t<check_insert_set_t<Assignments...>,
  //                              static_check_t<sizeof...(Assignments) != 0, assert_insert_static_set_count_args_t>,
  //                              static_check_t<detail::have_all_required_columns<lhs_t<Assignments>...>::value,
  //                                             assert_insert_static_set_all_required_t>>;
  template <typename... Assignments>
  struct check_insert_static_set
  {
    using type = static_combined_check_t<
        check_insert_set_t<Assignments...>,
        static_check_t<sizeof...(Assignments) != 0, assert_insert_static_set_count_args_t>,
        static_check_t<detail::have_all_required_columns<typename lhs<Assignments>::type...>::value,
                       assert_insert_static_set_all_required_t>>;
  };

  template <typename... Assignments>
  using check_insert_static_set_t = typename check_insert_static_set<Assignments...>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_insert_assignments_t,
      "at least one insert assignment requires a table which is otherwise not known in the statement");

  template <typename... Assignments>
  struct insert_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_insert_list>;

    template <template <typename...> class Target>
    using copy_assignments_t = Target<Assignments...>;  // FIXME: Nice idea to copy variadic template arguments?
    template <template <typename...> class Target, template <typename> class Wrap>
    using copy_wrapped_assignments_t = Target<Wrap<Assignments>...>;

    // Data
    using _data_t = insert_list_data_t<Assignments...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<insert_list_t>,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_insert_assignments_t>::type;
    };
  };

#warning: write tests for nodes.
  template <typename... Assignments>
  struct nodes_of<insert_list_t<Assignments...>>
  {
    using type = detail::type_vector<Assignments...>;
  };

  template <typename... Columns>
  struct column_list_data_t
  {
    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
    column_list_data_t(Columns... cols) : _columns(simple_column_t<Columns>(cols)...)
    {
    }

    column_list_data_t(const column_list_data_t&) = default;
    column_list_data_t(column_list_data_t&&) = default;
    column_list_data_t& operator=(const column_list_data_t&) = default;
    column_list_data_t& operator=(column_list_data_t&&) = default;
    ~column_list_data_t() = default;

    using _value_tuple_t = std::tuple<insert_value_t<Columns>...>;
    std::tuple<simple_column_t<Columns>...> _columns;
    std::vector<_value_tuple_t> _insert_values;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_no_unknown_tables_in_column_list_t,
                               "at least one column requires a table which is otherwise not known in the statement");

  template <typename... Columns>
  struct column_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_column_list>;

    using _value_tuple_t = typename column_list_data_t<Columns...>::_value_tuple_t;

    // Data
    using _data_t = column_list_data_t<Columns...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      template <typename... Assignments>
      void add_values(Assignments... assignments)
      {
        static_assert(logic::all<is_assignment<Assignments>::value...>::value,
                      "add_values() arguments have to be assignments");
        using _arg_value_tuple = std::tuple<insert_value_t<lhs_t<Assignments>>...>;
        using _args_correct = std::is_same<_arg_value_tuple, _value_tuple_t>;
        static_assert(_args_correct::value, "add_values() arguments do not match columns() arguments");

        using ok = logic::all<logic::all<is_assignment<Assignments>::value...>::value, _args_correct::value>;

        _add_impl(ok(), assignments...);  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename... Assignments>
      void _add_impl(const std::true_type& /*unused*/, Assignments... assignments)
      {
        _data._insert_values.emplace_back(insert_value_t<lhs_t<Assignments>>{assignments._r}...);
      }

      template <typename... Assignments>
      void _add_impl(const std::false_type& /*unused*/, Assignments... /*unused*/);

    public:
      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<column_list_t>,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_column_list_t>::type;
    };
  };

#warning: write tests for nodes.
  template <typename... Columns>
  struct nodes_of<column_list_t<Columns...>>
  {
    using type = detail::type_vector<Columns...>;
  };


  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_values_t, "insert values required, e.g. set(...) or default_values()");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_columns_are_columns, "arguments for columns() must be table columns");
  template <typename... Columns>
  struct check_insert_columns
  {
    using type = static_combined_check_t<
        static_check_t<logic::all<is_column_t<Columns>::value...>::value, assert_insert_columns_are_columns>>;
  };
  template <typename... Columns>
  using check_insert_columns_t = typename check_insert_columns<Columns...>::type;

  // NO INSERT COLUMNS/VALUES YET
  struct no_insert_value_list_t
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
      using _new_statement_t = new_statement_t<Check, Policies, no_insert_value_list_t, T>;

      using _consistency_check = assert_insert_values_t;

      auto default_values() const -> _new_statement_t<consistent_t, insert_default_values_t>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), insert_default_values_data_t{}};
      }

      template <typename... Columns>
      auto columns(Columns... cols) const
          -> _new_statement_t<check_insert_columns_t<Columns...>, column_list_t<Columns...>>
      {
        static_assert(sizeof...(Columns), "at least one column required in columns()");

        return _columns_impl(check_insert_columns_t<Columns...>{}, cols...);
      }

      template <typename... Assignments>
      auto set(Assignments... assignments) const
          -> _new_statement_t<check_insert_static_set_t<Assignments...>, insert_list_t<Assignments...>>
      {
        using Check = check_insert_static_set_t<Assignments...>;
        return _set_impl(Check{}, std::make_tuple(assignments...));
      }

      template <typename... Assignments>
      auto set(std::tuple<Assignments...> assignments) const
          -> _new_statement_t<check_insert_static_set_t<Assignments...>, insert_list_t<Assignments...>>
      {
        using Check = check_insert_static_set_t<Assignments...>;
        return _set_impl(Check{}, assignments);
      }

    private:
      template <typename Check, typename... Columns>
      auto _columns_impl(Check, Columns... cols) const -> inconsistent<Check>;

      template <typename... Columns>
      auto _columns_impl(consistent_t /*unused*/, Columns... cols) const
          -> _new_statement_t<consistent_t, column_list_t<Columns...>>
      {
        static_assert(detail::are_unique<Columns...>::value,
                      "at least one duplicate argument detected in columns()");
        static_assert(detail::type_vector_cat_t<required_tables_of_t<Columns>...>::are_same(),
                      "columns() contains columns from several tables");
        static_assert(detail::have_all_required_columns<Columns...>::value,
                      "At least one required column is missing in columns()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), column_list_data_t<Columns...>{cols...}};
      }

      template <typename Check, typename... Assignments>
      auto _set_impl(Check, Assignments... assignments) const -> inconsistent<Check>;

      template <typename... Assignments>
      auto _set_impl(consistent_t /*unused*/, std::tuple<Assignments...> assignments) const
          -> _new_statement_t<consistent_t, insert_list_t<Assignments...>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                insert_list_data_t<Assignments...>{assignments}};
      }
    };
  };

  // Interpreters
  template <typename Context>
  auto to_sql_string(Context& , const insert_default_values_data_t&) -> std::string
  {
    return " DEFAULT VALUES";
  }


  template <typename Context, typename... Columns>
  auto to_sql_string(Context& context, const column_list_data_t<Columns...>& t) -> std::string
  {
    auto result = std::string{" ("};
    result += tuple_to_sql_string(context, t._columns, tuple_operand{", "});
    result += ")";
    bool first = true;
    for (const auto& row : t._insert_values)
    {
      if (first)
      {
        result += " VALUES ";
        first = false;
      }
      else
      {
        result += ',';
      }
      result += '(';
      result += tuple_to_sql_string(context, row, tuple_operand{", "});
      result += ')';
    }

    return result;
  }

  template <typename Context, typename... Assignments>
  auto to_sql_string(Context& context, const insert_list_data_t<Assignments...>& t) -> std::string
  {
    auto result = std::string{" ("};
    result += tuple_to_sql_string(context, t._columns, tuple_operand{", "});
    result += ") VALUES(";
    result += tuple_to_sql_string(context, t._values, tuple_operand{", "});
    result += ")";
    return result;
  }

  template <typename... Assignments>
  auto insert_set(Assignments... assignments)
      -> decltype(statement_t<no_insert_value_list_t>().set(assignments...))
  {
    return statement_t<no_insert_value_list_t>().set(assignments...);
  }

  template <typename... Columns>
  auto insert_columns(Columns... cols)
      -> decltype(statement_t<no_insert_value_list_t>().columns(cols...))
  {
    return statement_t<no_insert_value_list_t>().columns(cols...);
  }
}  // namespace sqlpp
