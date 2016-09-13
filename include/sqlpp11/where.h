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

#ifndef SQLPP_WHERE_H
#define SQLPP_WHERE_H

#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/unconditional.h>
#include <sqlpp11/value.h>

namespace sqlpp
{
  // WHERE DATA
  template <typename Database, typename Expression>
  struct where_data_t
  {
    where_data_t(Expression expression) : _expression(expression)
    {
    }

    where_data_t(const where_data_t&) = default;
    where_data_t(where_data_t&&) = default;
    where_data_t& operator=(const where_data_t&) = default;
    where_data_t& operator=(where_data_t&&) = default;
    ~where_data_t() = default;

    Expression _expression;
    interpretable_list_t<Database> _dynamic_expressions;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_where_t,
      "at least one expression in where() requires a table which is otherwise not known in the statement");

  // WHERE(EXPR)
  template <typename Database, typename Expression>
  struct where_t
  {
    using _traits = make_traits<no_value_t, tag::is_where>;
    using _nodes = detail::type_vector<Expression>;

    using _is_dynamic = is_database<Database>;

    // Data
    using _data_t = where_data_t<Database, Expression>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      template <typename Expr>
      void add(Expr expression)
      {
        static_assert(_is_dynamic::value, "where::add() can only be called for dynamic_where");
        static_assert(is_expression_t<Expr>::value, "invalid expression argument in where::add()");
        static_assert(is_boolean_t<Expr>::value, "invalid expression argument in where::add()");
        static_assert(Policies::template _no_unknown_tables<Expr>::value,
                      "expression uses tables unknown to this statement in where::add()");
        static_assert(not contains_aggregate_function_t<Expr>::value,
                      "where expression must not contain aggregate functions");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Expr>;
        _serialize_check{};

        using ok = logic::all_t<_is_dynamic::value, is_expression_t<Expr>::value, _serialize_check::type::value>;

        _add_impl(expression, ok());  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename Expr>
      void _add_impl(Expr expression, const std::true_type&)
      {
        return _data._dynamic_expressions.emplace_back(expression);
      }

      template <typename Expr>
      void _add_impl(Expr expression, const std::false_type&);

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = where_data_t<Database, Expression>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : where{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> where;
      _impl_t<Policies>& operator()()
      {
        return where;
      }
      const _impl_t<Policies>& operator()() const
      {
        return where;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.where)
      {
        return t.where;
      }

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<where_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_where_t>::type;
    };
  };

  template <>
  struct where_data_t<void, unconditional_t>
  {
  };

  // WHERE() UNCONDITIONALLY
  template <>
  struct where_t<void, unconditional_t>
  {
    using _traits = make_traits<no_value_t, tag::is_where>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = where_data_t<void, unconditional_t>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
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
      using _data_t = where_data_t<void, unconditional_t>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : where{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> where;
      _impl_t<Policies>& operator()()
      {
        return where;
      }
      const _impl_t<Policies>& operator()() const
      {
        return where;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.where)
      {
        return t.where;
      }

      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_or_unconditionally_called_t,
                               "calling where() or unconditionally() required");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_arg_is_not_cpp_bool_t,
                               "where() argument has to be an sqlpp boolean expression. Please use "
                               ".unconditionally() instead of .where(true), or sqlpp::value(bool)");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_arg_is_boolean_expression_t,
                               "where() argument has to be an sqlpp boolean expression.");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_arg_contains_no_aggregate_functions_t,
                               "at least one aggregate function used in where()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_dynamic_used_with_dynamic_statement_t,
                               "dynamic_where() must not be called in a static statement");

  // workaround for msvc bugs https://connect.microsoft.com/VisualStudio/Feedback/Details/2086629 &
  // https://connect.microsoft.com/VisualStudio/feedback/details/2173198
  //  template <typename... Expressions>
  //  using check_where_t = static_combined_check_t<
  //      static_check_t<logic::all_t<is_not_cpp_bool_t<Expressions>::value...>::value,
  //      assert_where_arg_is_not_cpp_bool_t>,
  //      static_check_t<logic::all_t<is_expression_t<Expressions>::value...>::value,
  //      assert_where_boolean_expressions_t>,
  //      static_check_t<logic::all_t<is_boolean_t<Expressions>::value...>::value,
  //      assert_where_arg_is_boolean_expression_t>,
  //      static_check_t<logic::all_t<(not contains_aggregate_function_t<Expressions>::value)...>::value,
  //                     assert_where_arg_contains_no_aggregate_functions_t>>;
  template <typename Expression>
  struct check_where
  {
    using type = static_combined_check_t<
        static_check_t<is_not_cpp_bool_t<Expression>::value, assert_where_arg_is_not_cpp_bool_t>,
        static_check_t<is_expression_t<Expression>::value, assert_where_arg_is_boolean_expression_t>,
        static_check_t<is_boolean_t<Expression>::value, assert_where_arg_is_boolean_expression_t>,
        static_check_t<not contains_aggregate_function_t<Expression>::value,
                       assert_where_arg_contains_no_aggregate_functions_t>>;
  };

  template <typename Expression>
  using check_where_t = typename check_where<Expression>::type;

  template <typename Expression>
  using check_where_static_t = check_where_t<Expression>;

  template <typename Database, typename Expression>
  using check_where_dynamic_t = static_combined_check_t<
      static_check_t<not std::is_same<Database, void>::value, assert_where_dynamic_used_with_dynamic_statement_t>,
      check_where_t<Expression>>;

  // NO WHERE YET
  template <bool WhereRequired>
  struct no_where_t
  {
    using _traits = make_traits<no_value_t, tag::is_where>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
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

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : no_where{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_where;
      _impl_t<Policies>& operator()()
      {
        return no_where;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_where;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_where)
      {
        return t.no_where;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_where_t, T>;

      using _consistency_check =
          typename std::conditional<WhereRequired and (Policies::_all_provided_tables::size::value > 0),
                                    assert_where_or_unconditionally_called_t,
                                    consistent_t>::type;

      auto unconditionally() const -> _new_statement_t<consistent_t, where_t<void, unconditional_t>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), where_data_t<void, unconditional_t>{}};
      }

      template <typename Expression>
      auto where(Expression expression) const
          -> _new_statement_t<check_where_static_t<Expression>, where_t<void, Expression>>
      {
        using Check = check_where_static_t<Expression>;
        return _where_impl<void>(Check{}, expression);
      }

      template <typename Expression>
      auto dynamic_where(Expression expression) const
          -> _new_statement_t<check_where_dynamic_t<_database_t, Expression>, where_t<_database_t, Expression>>
      {
        using Check = check_where_dynamic_t<_database_t, Expression>;
        return _where_impl<_database_t>(Check{}, expression);
      }

      auto dynamic_where() const -> _new_statement_t<check_where_dynamic_t<_database_t, boolean_operand>,
                                                     where_t<_database_t, boolean_operand>>
      {
        return dynamic_where(::sqlpp::value(true));
      }

    private:
      template <typename Database, typename Check, typename Expression>
      auto _where_impl(Check, Expression expression) const -> inconsistent<Check>;

      template <typename Database, typename Expression>
      auto _where_impl(consistent_t, Expression expression) const
          -> _new_statement_t<consistent_t, where_t<Database, Expression>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                where_data_t<Database, Expression>{expression}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename Expression>
  struct serializer_t<Context, where_data_t<Database, Expression>>
  {
    using _serialize_check = serialize_check_of<Context, Expression>;
    using T = where_data_t<Database, Expression>;

    static Context& _(const T& t, Context& context)
    {
      context << " WHERE ";
      serialize(t._expression, context);
      if (not t._dynamic_expressions.empty())
        context << " AND ";
      interpret_list(t._dynamic_expressions, " AND ", context);
      return context;
    }
  };

  template <typename Context>
  struct serializer_t<Context, where_data_t<void, unconditional_t>>
  {
    using _serialize_check = consistent_t;
    using T = where_data_t<void, unconditional_t>;

    static Context& _(const T&, Context& context)
    {
      return context;
    }
  };

  template <typename T>
  auto where(T&& t) -> decltype(statement_t<void, no_where_t<false>>().where(std::forward<T>(t)))
  {
    return statement_t<void, no_where_t<false>>().where(std::forward<T>(t));
  }

  template <typename Database, typename T>
  auto dynamic_where(const Database&, T&& t)
      -> decltype(statement_t<Database, no_where_t<false>>().dynamic_where(std::forward<T>(t)))
  {
    return statement_t<Database, no_where_t<false>>().dynamic_where(std::forward<T>(t));
  }

  inline auto unconditionally() -> decltype(statement_t<void, no_where_t<false>>().unconditionally())
  {
    return statement_t<void, no_where_t<false>>().unconditionally();
  }
}

#endif
