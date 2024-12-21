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
#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/basic/column_fwd.h>
#include <sqlpp11/core/basic/table.h>
#include <sqlpp11/core/clause/insert_value.h>
#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/no_data.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/portable_static_assert.h>
#include <sqlpp11/core/static_assert.h>
#include <sqlpp11/core/clause/simple_column.h>
#include <sqlpp11/core/query/statement.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/compat/type_traits.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename Policies, typename... Columns>
    struct have_all_required_columns
    {
      static constexpr bool value = detail::make_type_set_t<Columns...>::contains_all(required_insert_columns_of_t<Policies>{});
    };

    template <typename Policies, typename... Assignments>
    struct have_all_required_assignments
    {
      static constexpr bool value = have_all_required_columns<Policies, lhs_t<Assignments>...>::value;
    };
  }  // namespace detail

  struct insert_default_values_data_t
  {
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_all_columns_have_default_value_t,
      "at least one column does not have a default value (explicit default, NULL, or auto-increment)");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_all_required_columns_t, "at least one required column is missing in columns()");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_all_required_assignments_t, "at least one required column is missing in set()");

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
    };
  };

  template <typename Statement>
  struct consistency_check<Statement, insert_default_values_t>
  {
    using type = typename std::conditional<required_insert_columns_of_t<Statement>::empty(),
                                           consistent_t,
                                           assert_all_columns_have_default_value_t>::type;
  };

  template <typename... Assignments>
  struct insert_set_data_t
  {
    insert_set_data_t(std::tuple<Assignments...> assignments)
        : _assignments(std::move(assignments))
    {
    }

    insert_set_data_t(const insert_set_data_t&) = default;
    insert_set_data_t(insert_set_data_t&&) = default;
    insert_set_data_t& operator=(const insert_set_data_t&) = default;
    insert_set_data_t& operator=(insert_set_data_t&&) = default;
    ~insert_set_data_t() = default;

    std::tuple<Assignments...> _assignments;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_insert_assignments_t,
      "at least one insert assignment requires a table which is otherwise not known in the statement");

  template <typename... Assignments>
  struct insert_set_t
  {
    template <template <typename...> class Target>
    using copy_assignments_t = Target<Assignments...>;  // FIXME: Nice idea to copy variadic template arguments?
    template <template <typename...> class Target, template <typename> class Wrap>
    using copy_wrapped_assignments_t = Target<Wrap<Assignments>...>;

    // Data
    using _data_t = insert_set_data_t<Assignments...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

    };
  };

  template <typename Statement, typename... Assignments>
  struct consistency_check<Statement, insert_set_t<Assignments...>>
  {
    using type =
        static_combined_check_t<static_check_t<Statement::template _no_unknown_tables<insert_set_t<Assignments...>>,
                                               assert_no_unknown_tables_in_insert_assignments_t>,
                                static_check_t<detail::have_all_required_assignments<Statement, Assignments...>::value,
                                               assert_all_required_assignments_t>>;
  };

  template <typename... Assignments>
  struct nodes_of<insert_set_t<Assignments...>>
  {
    using type = detail::type_vector<Assignments...>;
  };

  template <typename... Columns>
  struct column_list_data_t
  {
    // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
    column_list_data_t(Columns... cols) : _columns(simple_column(std::move(cols))...)
    {
    }

    column_list_data_t(const column_list_data_t&) = default;
    column_list_data_t(column_list_data_t&&) = default;
    column_list_data_t& operator=(const column_list_data_t&) = default;
    column_list_data_t& operator=(column_list_data_t&&) = default;
    ~column_list_data_t() = default;

    using _value_tuple_t = std::tuple<make_insert_value_t<Columns>...>;
    std::tuple<make_simple_column_t<Columns>...> _columns;
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

      template <
          typename... Assignments,
          typename = sqlpp::enable_if_t<logic::all<is_assignment<remove_dynamic_t<Assignments>>::value...>::value>>
      auto add_values(Assignments... assignments) -> void
      {
        using _arg_value_tuple = std::tuple<make_insert_value_t<lhs_t<Assignments>>...>;
        constexpr bool _args_correct = std::is_same<_arg_value_tuple, _value_tuple_t>::value;
        SQLPP_STATIC_ASSERT(_args_correct, "add_values() arguments have to match columns() arguments");

        constexpr bool _no_expressions = logic::all<nodes_of_t<remove_dynamic_t<rhs_t<Assignments>>>::empty()...>::value;
        SQLPP_STATIC_ASSERT(_no_expressions, "add_values() arguments must not be expressions");

        constexpr bool _no_parameters = logic::all<parameters_of_t<rhs_t<Assignments>>::empty()...>::value;
        SQLPP_STATIC_ASSERT(_no_parameters, "add_values() arguments must not contain parameters");

        constexpr bool _no_names = logic::none<has_name_tag<remove_dynamic_t<rhs_t<Assignments>>>::value...>::value;
        SQLPP_STATIC_ASSERT(_no_names, "add_values() arguments must not have names");

        add_values_impl(logic::all<_args_correct, _no_expressions, _no_parameters, _no_names>{},
                        std::move(assignments)...);
      }

    private:
      auto add_values_impl(std::false_type, ...) -> void
      {
      }

      template <
          typename... Assignments>
      auto add_values_impl(std::true_type, Assignments... assignments) -> void
      {
        _data._insert_values.emplace_back(make_insert_value_t<lhs_t<Assignments>>(get_rhs(assignments))...);
      }
    };
  };

  template <typename Statement, typename... Columns>
  struct consistency_check<Statement, column_list_t<Columns...>>
  {
    using type = static_combined_check_t<
        static_check_t<Statement::template _no_unknown_tables<column_list_t<Columns...>>,
                       assert_no_unknown_tables_in_column_list_t>,
        static_check_t<detail::have_all_required_columns<Statement, Columns...>::value, assert_all_required_columns_t>>;
  };

  template <typename... Columns>
  struct nodes_of<column_list_t<Columns...>>
  {
    using type = detail::type_vector<Columns...>;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_values_t, "insert values required, e.g. set(...) or default_values()");

  // NO INSERT COLUMNS/VALUES YET
  struct no_insert_value_list_t
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

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_insert_value_list_t, T>;

      auto default_values() const -> _new_statement_t<consistent_t, insert_default_values_t>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), insert_default_values_data_t{}};
      }

      template <typename... Columns, typename = sqlpp::enable_if_t<logic::all<is_column<remove_dynamic_t<Columns>>::value...>::value>>
      auto columns(Columns... cols) const
          -> _new_statement_t<consistent_t, column_list_t<Columns...>>
      {
        SQLPP_STATIC_ASSERT(sizeof...(Columns), "at least one column required in columns()");
        SQLPP_STATIC_ASSERT(detail::are_unique<remove_dynamic_t<Columns>...>::value,
                            "at least one duplicate column detected in columns()");
        SQLPP_STATIC_ASSERT(detail::are_same<typename remove_dynamic_t<Columns>::_table...>::value,
                            "columns() contains columns from several tables");

        return {static_cast<const derived_statement_t<Policies>&>(*this), column_list_data_t<Columns...>{cols...}};

      }

      template <
          typename... Assignments,
          typename = sqlpp::enable_if_t<logic::all<is_assignment<remove_dynamic_t<Assignments>>::value...>::value>>
      auto set(Assignments... assignments) const -> _new_statement_t<consistent_t, insert_set_t<Assignments...>>
      {
        SQLPP_STATIC_ASSERT(sizeof...(Assignments) != 0, "at least one assignment expression required in set()");

        static constexpr bool has_duplicate_columns =
            detail::has_duplicates<lhs_t<remove_dynamic_t<Assignments>>...>::value;
        SQLPP_STATIC_ASSERT(not has_duplicate_columns, "at least one duplicate column detected in set()");

        static constexpr bool uses_exactly_one_table =
            detail::are_same<typename lhs_t<remove_dynamic_t<Assignments>>::_table...>::value;
        SQLPP_STATIC_ASSERT(uses_exactly_one_table, "set() arguments must be assignment for exactly one table");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                insert_set_data_t<Assignments...>{std::make_tuple(std::move(assignments)...)}};
      }

    };
  };

  template <typename Statement>
  struct consistency_check<Statement, no_insert_value_list_t>
  {
    using type = assert_insert_values_t;
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
    result += tuple_to_sql_string(context, t._columns, tuple_operand_no_dynamic{", "});
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
        result += ", ";
      }
      result += '(';
      result += tuple_to_sql_string(context, row, tuple_operand_no_dynamic{", "});
      result += ')';
    }

    return result;
  }

  // Used to serialize left hand side of assignment tuple that should ignore dynamic elements.
  struct tuple_lhs_assignment_operand_no_dynamic
  {
    template <typename Context, typename Lhs, typename Op, typename Rhs>
    auto operator()(Context& context, const assign_expression<Lhs, Op, Rhs>&, size_t ) const -> std::string
    {
      const auto prefix = need_prefix ? std::string{separator} : std::string{};
      need_prefix = true;
      return prefix + name_to_sql_string(context, name_tag_of_t<Lhs>{});
    }

    template <typename Context, typename T>
    auto operator()(Context& context, const sqlpp::dynamic_t<T>& t, size_t index) const -> std::string
    {
      if (t._condition)
      {
        return operator()(context, t._expr, index);
      }
      return "";
    }

    sqlpp::string_view separator;
    mutable bool need_prefix = false;
  };

  // Used to serialize right hand side of assignment tuple that should ignore dynamic elements.
  struct tuple_rhs_assignment_operand_no_dynamic
  {
    template <typename Context, typename Lhs, typename Op, typename Rhs>
    auto operator()(Context& context, const assign_expression<Lhs, Op, Rhs>& t, size_t ) const -> std::string
    {
      const auto prefix = need_prefix ? std::string{separator} : std::string{};
      need_prefix = true;
      return prefix + operand_to_sql_string(context, t._r);
    }

    template <typename Context, typename T>
    auto operator()(Context& context, const sqlpp::dynamic_t<T>& t, size_t index) const -> std::string
    {
      if (t._condition)
      {
        return operator()(context, t._expr, index);
      }
      return "";
    }

    sqlpp::string_view separator;
    mutable bool need_prefix = false;
  };

  template <typename Context, typename... Assignments>
  auto to_sql_string(Context& context, const insert_set_data_t<Assignments...>& t) -> std::string
  {
    auto result = std::string{" ("};
    result += tuple_to_sql_string(context, t._assignments, tuple_lhs_assignment_operand_no_dynamic{", "});
    result += ") VALUES(";
    result += tuple_to_sql_string(context, t._assignments, tuple_rhs_assignment_operand_no_dynamic{", "});
    result += ")";
    return result;
  }

  template <typename... Assignments>
  auto insert_default_values()
      -> decltype(statement_t<no_insert_value_list_t>().default_values())
  {
    return statement_t<no_insert_value_list_t>().default_values();
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
