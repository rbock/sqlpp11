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
#include <sqlpp11/policy_update.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/value.h>

namespace sqlpp
{
  // HAVING DATA
  template <typename Database, typename Expression>
  struct having_data_t
  {
    having_data_t(Expression expression) : _expression(expression)
    {
    }

    having_data_t(const having_data_t&) = default;
    having_data_t(having_data_t&&) = default;
    having_data_t& operator=(const having_data_t&) = default;
    having_data_t& operator=(having_data_t&&) = default;
    ~having_data_t() = default;

    Expression _expression;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_having_no_unknown_tables_t,
      "at least one having-expression requires a table which is otherwise not known in the statement");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_having_all_aggregates_t,
                               "having expression not built out of aggregate expressions");

  // HAVING
  template <typename Database, typename Expression>
  struct having_t
  {
    using _traits = make_traits<no_value_t, tag::is_having>;
    using _nodes = detail::type_vector<Expression>;

    // Data
    using _data_t = having_data_t<Database, Expression>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
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
      using _data_t = having_data_t<Database, Expression>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : having{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> having;
      _impl_t<Policies>& operator()()
      {
        return having;
      }
      const _impl_t<Policies>& operator()() const
      {
        return having;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.having)
      {
        return t.having;
      }

      using _table_check = typename std::conditional<Policies::template _no_unknown_tables<having_t>::value,
                                                     consistent_t,
                                                     assert_having_no_unknown_tables_t>::type;

      using _aggregate_check = typename std::conditional<Policies::template _all_aggregates<Expression>::value,
                                                         consistent_t,
                                                         assert_having_all_aggregates_t>::type;

      using _consistency_check = detail::get_first_if<is_inconsistent_t, consistent_t, _table_check, _aggregate_check>;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_having_not_cpp_bool_t,
                               "having() argument has to be an sqlpp boolean expression. Please use "
                               "sqlpp::value(bool_expresson) if you really want to use a bool value here");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_having_boolean_expression_t,
                               "having() argument has to be an sqlpp boolean expression.");

  template <typename Expression>
  struct check_having
  {
    using type =
        static_combined_check_t<static_check_t<is_not_cpp_bool_t<Expression>::value, assert_having_not_cpp_bool_t>,
                                static_check_t<is_expression_t<Expression>::value, assert_having_boolean_expression_t>,
                                static_check_t<is_boolean_t<Expression>::value, assert_having_boolean_expression_t>>;
  };

  template <typename Expression>
  using check_having_t = typename check_having<Expression>::type;

  template <typename Expression>
  using check_having_static_t = check_having_t<Expression>;

  template <typename... Exprs>
  constexpr auto are_all_parameters_expressions() -> bool
  {
    return logic::all_t<is_expression_t<Exprs>::value...>::value;
  }

  // NO HAVING YET
  struct no_having_t
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
      _base_t(Args&&... args) : no_having{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_having;
      _impl_t<Policies>& operator()()
      {
        return no_having;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_having;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_having)
      {
        return t.no_having;
      }

      using _database_t = typename Policies::_database_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      //	  template <typename... T>
      //	  using _check = logic::all_t<is_expression_t<T>::value...>;
      template <typename... T>
      struct _check : std::integral_constant<bool, are_all_parameters_expressions<T...>()>
      {
      };

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_having_t, T>;

      using _consistency_check = consistent_t;

      template <typename Expression>
      auto having(Expression expression) const
          -> _new_statement_t<check_having_static_t<Expression>, having_t<void, Expression>>
      {
        using Check = check_having_static_t<Expression>;

        return _having_impl<void>(Check{}, expression);
      }

    private:
      template <typename Database, typename Check, typename Expression>
      auto _having_impl(Check, Expression expression) const -> inconsistent<Check>;

      template <typename Database, typename Expression>
      auto _having_impl(consistent_t /*unused*/, Expression expression) const
          -> _new_statement_t<consistent_t, having_t<Database, Expression>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                having_data_t<Database, Expression>{expression}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename Expression>
  Context& serialize(const having_data_t<Database, Expression>& t, Context& context)
  {
    context << " HAVING ";
    serialize(t._expression, context);
    return context;
  }

  template <typename T>
  auto having(T&& t) -> decltype(statement_t<void, no_having_t>().having(std::forward<T>(t)))
  {
    return statement_t<void, no_having_t>().having(std::forward<T>(t));
  }

}  // namespace sqlpp
