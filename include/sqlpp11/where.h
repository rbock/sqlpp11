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

#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
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

    private:
      template <typename Database, typename Check, typename Expression>
      auto _where_impl(Check, Expression expression) const -> inconsistent<Check>;

      template <typename Database, typename Expression>
      auto _where_impl(consistent_t /*unused*/, Expression expression) const
          -> _new_statement_t<consistent_t, where_t<Database, Expression>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                where_data_t<Database, Expression>{expression}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename Expression>
  Context& serialize(const where_data_t<Database, Expression>& t, Context& context)
  {
    context << " WHERE ";
    serialize(t._expression, context);
    return context;
  }

  template <typename Context, typename Database>
  Context& serialize(const where_data_t<Database, unconditional_t>& t, Context& context)
  {
    return context;
  }

  template <typename Context>
  Context& serialize(const where_data_t<void, unconditional_t>&, Context& context)
  {
    return context;
  }

  template <typename T>
  auto where(T&& t) -> decltype(statement_t<void, no_where_t<false>>().where(std::forward<T>(t)))
  {
    return statement_t<void, no_where_t<false>>().where(std::forward<T>(t));
  }

  inline auto unconditionally() -> decltype(statement_t<void, no_where_t<false>>().unconditionally())
  {
    return statement_t<void, no_where_t<false>>().unconditionally();
  }
}  // namespace sqlpp
