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

#ifndef SQLPP_INSERT_VALUE_LIST_H
#define SQLPP_INSERT_VALUE_LIST_H

#include <sqlpp11/assignment.h>
#include <sqlpp11/column_fwd.h>
#include <sqlpp11/expression_fwd.h>
#include <sqlpp11/insert_value.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/portable_static_assert.h>
#include <sqlpp11/simple_column.h>
#include <sqlpp11/statement.h>
#include <sqlpp11/type_traits.h>

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
  }

  struct insert_default_values_data_t
  {
  };

  // COLUMN AND VALUE LIST
  struct insert_default_values_t
  {
    using _traits = make_traits<no_value_t>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = insert_default_values_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = insert_default_values_data_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : default_values{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> default_values;
      _impl_t<Policies>& operator()()
      {
        return default_values;
      }
      const _impl_t<Policies>& operator()() const
      {
        return default_values;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.default_values)
      {
        return t.default_values;
      }

      using _consistency_check = consistent_t;
    };
  };

  template <typename Database, typename... Assignments>
  struct insert_list_data_t
  {
    insert_list_data_t(Assignments... assignments)
        : _assignments(assignments...), _columns(assignments._lhs...), _values(assignments._rhs...)
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
    interpretable_list_t<Database> _dynamic_columns;
    interpretable_list_t<Database> _dynamic_values;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_assignments_t, "at least one argument is not an assignment in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_no_duplicates_t, "at least one duplicate column detected in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_allowed_t,
                               "at least one assignment is prohibited by its column definition in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_set_single_table_t,
                               "set() arguments contain assignments from more than one table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_static_set_count_args_t,
                               "at least one assignment expression required in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_static_set_all_required_t,
                               "at least one required column is missing in set()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_dynamic_set_statement_dynamic_t,
                               "dynamic_set must not be called in a static statement");

  // workaround for msvc bugs https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269 &
  // https://connect.microsoft.com/VisualStudio/Feedback/Details/2173198
  //  template <typename... Assignments>
  //  using check_insert_set_t = static_combined_check_t<
  //      static_check_t<logic::all_t<is_assignment_t<Assignments>::value...>::value, assert_insert_set_assignments_t>,
  //      static_check_t<not detail::has_duplicates<lhs_t<Assignments>...>::value, assert_insert_set_no_duplicates_t>,
  //      static_check_t<logic::none_t<must_not_insert_t<lhs_t<Assignments>>::value...>::value,
  //                     assert_insert_set_allowed_t>,
  //      static_check_t<sizeof...(Assignments) == 0 or
  //                         detail::make_joined_set_t<required_tables_of<lhs_t<Assignments>>...>::size::value == 1,
  //                     assert_insert_set_single_table_t>>;

  template <typename Expr>
  struct must_not_insert
  {
    static const bool value = must_not_insert_t<lhs_t<Expr>>::value;
  };

  template <typename... Assignments>
  using check_insert_set_t = static_combined_check_t<
      static_check_t<logic::all_t<detail::is_assignment_impl<Assignments>::type::value...>::value,
                     assert_insert_set_assignments_t>,
      static_check_t<not detail::has_duplicates<typename lhs<Assignments>::type...>::value,
                     assert_insert_set_no_duplicates_t>,
      static_check_t<logic::none_t<must_not_insert<Assignments>::value...>::value, assert_insert_set_allowed_t>,
      static_check_t<
          sizeof...(Assignments) == 0 or
              detail::make_joined_set_t<required_tables_of<typename lhs<Assignments>::type>...>::size::value == 1,
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

  // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
  //  template <typename Database, typename... Assignments>
  //  using check_insert_dynamic_set_t = static_combined_check_t<
  //      static_check_t<not std::is_same<Database, void>::value, assert_insert_dynamic_set_statement_dynamic_t>,
  //      check_insert_set_t<Assignments...>>;
  template <typename Database, typename... Assignments>
  struct check_insert_dynamic_set
  {
    using type = static_combined_check_t<
        static_check_t<not std::is_same<Database, void>::value, assert_insert_dynamic_set_statement_dynamic_t>,
        check_insert_set_t<Assignments...>>;
  };

  template <typename Database, typename... Assignments>
  using check_insert_dynamic_set_t = typename check_insert_dynamic_set<Database, Assignments...>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_insert_assignments_t,
      "at least one insert assignment requires a table which is otherwise not known in the statement");

  template <typename Database, typename... Assignments>
  struct insert_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_insert_list>;
    using _nodes = detail::type_vector<lhs_t<Assignments>..., rhs_t<Assignments>...>;

    using _is_dynamic = is_database<Database>;

    template <template <typename...> class Target>
    using copy_assignments_t = Target<Assignments...>;  // FIXME: Nice idea to copy variadic template arguments?
    template <template <typename...> class Target, template <typename> class Wrap>
    using copy_wrapped_assignments_t = Target<Wrap<Assignments>...>;

    // Data
    using _data_t = insert_list_data_t<Database, Assignments...>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      template <typename Assignment>
      void add(Assignment assignment)
      {
        static_assert(_is_dynamic::value, "add must not be called for static from()");
        static_assert(is_assignment_t<Assignment>::value, "add() arguments require to be assigments");
        using _assigned_columns = detail::make_type_set_t<lhs_t<Assignments>...>;
        static_assert(not detail::is_element_of<lhs_t<Assignment>, _assigned_columns>::value,
                      "Must not assign value to column twice");
        static_assert(not must_not_insert_t<lhs_t<Assignment>>::value, "add() argument must not be used in insert");
        static_assert(Policies::template _no_unknown_tables<Assignment>::value,
                      "add() contains a column from a foreign table");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Assignment>;
        _serialize_check{};

        using ok = logic::all_t<_is_dynamic::value, is_assignment_t<Assignment>::value, _serialize_check::type::value>;

        _add_impl(assignment, ok());  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename Assignment>
      void _add_impl(Assignment assignment, const std::true_type&)
      {
        _data._dynamic_columns.emplace_back(simple_column_t<lhs_t<Assignment>>{assignment._lhs});
        _data._dynamic_values.emplace_back(assignment._rhs);
      }

      template <typename Assignment>
      void _add_impl(Assignment assignment, const std::false_type&);

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = insert_list_data_t<Database, Assignments...>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : insert_list{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> insert_list;
      _impl_t<Policies>& operator()()
      {
        return insert_list;
      }
      const _impl_t<Policies>& operator()() const
      {
        return insert_list;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.insert_list)
      {
        return t.insert_list;
      }

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<insert_list_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_insert_assignments_t>::type;
    };
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
    using _nodes = detail::type_vector<Columns...>;

    using _value_tuple_t = typename column_list_data_t<Columns...>::_value_tuple_t;

    // Data
    using _data_t = column_list_data_t<Columns...>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      template <typename... Assignments>
      void add(Assignments... assignments)
      {
        static_assert(logic::all_t<is_assignment_t<Assignments>::value...>::value,
                      "add_values() arguments have to be assignments");
        using _arg_value_tuple = std::tuple<insert_value_t<lhs_t<Assignments>>...>;
        using _args_correct = std::is_same<_arg_value_tuple, _value_tuple_t>;
        static_assert(_args_correct::value, "add_values() arguments do not match columns() arguments");

        using ok = logic::all_t<logic::all_t<is_assignment_t<Assignments>::value...>::value, _args_correct::value>;

        _add_impl(ok(), assignments...);  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename... Assignments>
      void _add_impl(const std::true_type&, Assignments... assignments)
      {
        return _data._insert_values.emplace_back(insert_value_t<lhs_t<Assignments>>{assignments._rhs}...);
      }

      template <typename... Assignments>
      void _add_impl(const std::false_type&, Assignments... assignments);

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = column_list_data_t<Columns...>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : values{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> values;
      _impl_t<Policies>& operator()()
      {
        return values;
      }
      const _impl_t<Policies>& operator()() const
      {
        return values;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.values)
      {
        return t.values;
      }

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<column_list_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_column_list_t>::type;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_values_t, "insert values required, e.g. set(...) or default_values()");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_insert_columns_are_columns, "arguments for columns() must be table columns");
  template <typename... Columns>
  struct check_insert_columns
  {
    using type = static_combined_check_t<
        static_check_t<logic::all_t<is_column_t<Columns>::value...>::value, assert_insert_columns_are_columns>>;
  };
  template <typename... Columns>
  using check_insert_columns_t = typename check_insert_columns<Columns...>::type;

  // NO INSERT COLUMNS/VALUES YET
  struct no_insert_value_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = no_data_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : no_insert_values{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_insert_values;
      _impl_t<Policies>& operator()()
      {
        return no_insert_values;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_insert_values;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_insert_values)
      {
        return t.no_insert_values;
      }

      using _database_t = typename Policies::_database_t;

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
          -> _new_statement_t<check_insert_static_set_t<Assignments...>, insert_list_t<void, Assignments...>>
      {
        using Check = check_insert_static_set_t<Assignments...>;
        return _set_impl<void>(Check{}, assignments...);
      }

      template <typename... Assignments>
      auto dynamic_set(Assignments... assignments) const
          -> _new_statement_t<check_insert_dynamic_set_t<_database_t, Assignments...>,
                              insert_list_t<_database_t, Assignments...>>
      {
        using Check = check_insert_dynamic_set_t<_database_t, Assignments...>;
        return _set_impl<_database_t>(Check{}, assignments...);
      }

    private:
      template <typename Check, typename... Columns>
      auto _columns_impl(Check, Columns... cols) const -> inconsistent<Check>;

      template <typename... Columns>
      auto _columns_impl(consistent_t, Columns... cols) const
          -> _new_statement_t<consistent_t, column_list_t<Columns...>>
      {
        static_assert(not detail::has_duplicates<Columns...>::value,
                      "at least one duplicate argument detected in columns()");
        static_assert(logic::none_t<must_not_insert_t<Columns>::value...>::value,
                      "at least one column argument has a must_not_insert tag in its definition");
        using _column_required_tables = detail::make_joined_set_t<required_tables_of<Columns>...>;
        static_assert(_column_required_tables::size::value == 1, "columns() contains columns from several tables");

        static_assert(detail::have_all_required_columns<Columns...>::value,
                      "At least one required column is missing in columns()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), column_list_data_t<Columns...>{cols...}};
      }

      template <typename Database, typename Check, typename... Assignments>
      auto _set_impl(Check, Assignments... assignments) const -> inconsistent<Check>;

      template <typename Database, typename... Assignments>
      auto _set_impl(consistent_t, Assignments... assignments) const
          -> _new_statement_t<consistent_t, insert_list_t<Database, Assignments...>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                insert_list_data_t<Database, Assignments...>{assignments...}};
      }
    };
  };

  // Interpreters
  template <typename Context>
  struct serializer_t<Context, insert_default_values_data_t>
  {
    using _serialize_check = consistent_t;
    using T = insert_default_values_data_t;

    static Context& _(const T&, Context& context)
    {
      context << " DEFAULT VALUES";
      return context;
    }
  };

  template <typename Context, typename... Columns>
  struct serializer_t<Context, column_list_data_t<Columns...>>
  {
    using _serialize_check = serialize_check_of<Context, Columns...>;
    using T = column_list_data_t<Columns...>;

    static Context& _(const T& t, Context& context)
    {
      context << " (";
      interpret_tuple(t._columns, ",", context);
      context << ")";
      context << " VALUES ";
      bool first = true;
      for (const auto& row : t._insert_values)
      {
        if (not first)
          context << ',';
        else
          first = false;
        context << '(';
        interpret_tuple(row, ",", context);
        context << ')';
      }

      return context;
    }
  };

  template <typename Context, typename Database, typename... Assignments>
  struct serializer_t<Context, insert_list_data_t<Database, Assignments...>>
  {
    using _serialize_check = serialize_check_of<Context, Assignments...>;
    using T = insert_list_data_t<Database, Assignments...>;

    static Context& _(const T& t, Context& context)
    {
      if (sizeof...(Assignments) + t._dynamic_columns.size() == 0)
      {
        serialize(insert_default_values_data_t(), context);
      }
      else
      {
        context << " (";
        interpret_tuple(t._columns, ",", context);
        if (sizeof...(Assignments) and not t._dynamic_columns.empty())
          context << ',';
        interpret_list(t._dynamic_columns, ',', context);
        context << ") VALUES(";
        interpret_tuple(t._values, ",", context);
        if (sizeof...(Assignments) and not t._dynamic_values.empty())
          context << ',';
        interpret_list(t._dynamic_values, ',', context);
        context << ")";
      }
      return context;
    }
  };

  template <typename... Assignments>
  auto insert_set(Assignments... assignments)
      -> decltype(statement_t<void, no_insert_value_list_t>().set(assignments...))
  {
    return statement_t<void, no_insert_value_list_t>().set(assignments...);
  }

  template <typename Database, typename... Assignments>
  auto dynamic_insert_set(Assignments... assignments)
      -> decltype(statement_t<Database, no_insert_value_list_t>().dynamic_set(assignments...))
  {
    return statement_t<Database, no_insert_value_list_t>().dynamic_set(assignments...);
  }
}

#endif
