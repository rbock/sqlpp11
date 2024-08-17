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

#include <sqlpp11/core/tuple_to_sql_string.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/statement_fwd.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/unconditional.h>
#include <sqlpp11/core/basic/value.h>

namespace sqlpp
{
  // WHERE DATA
  template <typename Expression>
  struct where_data_t
  {
    Expression _expression;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_where_t,
      "at least one expression in where() requires a table which is otherwise not known in the statement");

  // WHERE(EXPR)
  template <typename Expression>
  struct where_t
  {
    using _traits = make_traits<no_value_t, tag::is_where>;

    using _data_t = where_data_t<Expression>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(const _base_t&) = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<where_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_where_t>::type;
    };
  };

  template <typename Expression>
  struct nodes_of<where_t<Expression>>
  {
    using type = detail::type_vector<Expression>;
  };

  template <>
  struct where_data_t<unconditional_t>
  {
  };

  // WHERE() UNCONDITIONALLY
  template <>
  struct where_t<unconditional_t>
  {
    using _traits = make_traits<no_value_t, tag::is_where>;

    using _data_t = where_data_t<unconditional_t>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(const _base_t&) = default;
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_or_unconditionally_called_t,
                               "calling where() or unconditionally() required");

  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_arg_is_boolean_expression_t,
                               "where() argument has to be an sqlpp boolean expression.");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_where_arg_contains_no_aggregate_t,
                               "at least one aggregate function used in where()");

  // workaround for msvc bugs https://connect.microsoft.com/VisualStudio/Feedback/Details/2086629 &
  // https://connect.microsoft.com/VisualStudio/feedback/details/2173198
  //  template <typename... Expressions>
  //  using check_where_t = static_combined_check_t<
  //      static_check_t<logic::all<is_boolean<Expressions>::value...>::value,
  //      assert_where_arg_is_boolean_expression_t>,
  //      static_check_t<logic::all<(not contains_aggregate_function<Expressions>::value)...>::value,
  //                     assert_where_arg_contains_no_aggregate_t>>;
  template <typename Expression>
  struct check_where
  {
    using type = static_combined_check_t<
        static_check_t<is_boolean<Expression>::value, assert_where_arg_is_boolean_expression_t>,
        static_check_t<not contains_aggregate_function<Expression>::value,
                       assert_where_arg_contains_no_aggregate_t>>;
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
      using _new_statement_t = new_statement_t<Check, Policies, no_where_t, T>;

      using _consistency_check =
          typename std::conditional<WhereRequired and (Policies::_all_provided_tables::size::value > 0),
                                    assert_where_or_unconditionally_called_t,
                                    consistent_t>::type;

      auto unconditionally() const -> _new_statement_t<consistent_t, where_t<unconditional_t>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), where_data_t<unconditional_t>{}};
      }

      template <typename Expression>
      auto where(Expression expression) const
          -> _new_statement_t<check_where_static_t<Expression>, where_t<Expression>>
      {
        using Check = check_where_static_t<Expression>;
        return _where_impl(Check{}, expression);
      }

    private:
      template <typename Check, typename Expression>
      auto _where_impl(Check, Expression expression) const -> inconsistent<Check>;

      template <typename Expression>
      auto _where_impl(consistent_t /*unused*/, Expression expression) const
          -> _new_statement_t<consistent_t, where_t<Expression>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                where_data_t<Expression>{expression}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Expression>
  auto to_sql_string(Context& context, const where_data_t<Expression>& t) -> std::string
  {
    return  " WHERE " + to_sql_string(context, t._expression);
  }

  template <typename Context>
  auto to_sql_string(Context& , const where_data_t<unconditional_t>&) -> std::string
  {
    return {};
  }

  template <typename T>
  auto where(T&& t) -> decltype(statement_t<no_where_t<false>>().where(std::forward<T>(t)))
  {
    return statement_t<no_where_t<false>>().where(std::forward<T>(t));
  }

  inline auto unconditionally() -> decltype(statement_t<no_where_t<false>>().unconditionally())
  {
    return statement_t<no_where_t<false>>().unconditionally();
  }
}  // namespace sqlpp
