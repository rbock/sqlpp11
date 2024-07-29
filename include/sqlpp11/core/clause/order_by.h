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
#include <sqlpp11/core/interpret_tuple.h>
#include <sqlpp11/core/logic.h>
#include <sqlpp11/core/query/policy_update.h>
#include <sqlpp11/core/type_traits.h>
#include <tuple>

namespace sqlpp
{
  // ORDER BY DATA
  template <typename... Expressions>
  struct order_by_data_t
  {
    order_by_data_t(Expressions... expressions) : _expressions(expressions...)
    {
    }

    order_by_data_t(const order_by_data_t&) = default;
    order_by_data_t(order_by_data_t&&) = default;
    order_by_data_t& operator=(const order_by_data_t&) = default;
    order_by_data_t& operator=(order_by_data_t&&) = default;
    ~order_by_data_t() = default;

    std::tuple<Expressions...> _expressions;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_order_by_t,
      "at least one order-by expression requires a table which is otherwise not known in the statement");

  // ORDER BY
  template <typename... Expressions>
  struct order_by_t
  {
    using _traits = make_traits<no_value_t, tag::is_order_by>;
    using _nodes = detail::type_vector<Expressions...>;

    using _data_t = order_by_data_t<Expressions...>;

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      _base_t(_data_t data) : _data{std::move(data)}
      {
      }

      _data_t _data;

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<order_by_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_order_by_t>::type;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_order_by_args_are_sort_order_expressions_t,
                               "arguments for order_by() must be sort order expressions");
  template <typename... Exprs>
  struct check_order_by
  {
    using type = static_combined_check_t<static_check_t<logic::all_t<is_sort_order<Exprs>::value...>::value,
                                                        assert_order_by_args_are_sort_order_expressions_t>>;
  };
  template <typename... Exprs>
  using check_order_by_t = typename check_order_by<Exprs...>::type;

  // NO ORDER BY YET
  struct no_order_by_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

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
      using _new_statement_t = new_statement_t<Check, Policies, no_order_by_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Expressions>
      auto order_by(Expressions... expressions) const
          -> _new_statement_t<check_order_by_t<Expressions...>, order_by_t<Expressions...>>
      {
        static_assert(sizeof...(Expressions), "at least one expression (e.g. a column) required in order_by()");

        return _order_by_impl(check_order_by_t<Expressions...>{}, std::move(expressions)...);
      }

    private:
      template <typename Check, typename... Expressions>
      auto _order_by_impl(Check, Expressions... expressions) const -> inconsistent<Check>;

      template <typename... Expressions>
      auto _order_by_impl(consistent_t /*unused*/, Expressions... expressions) const
          -> _new_statement_t<consistent_t, order_by_t<Expressions...>>
      {
        static_assert(not detail::has_duplicates<Expressions...>::value,
                      "at least one duplicate argument detected in order_by()");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                order_by_data_t<Expressions...>{std::move(expressions)...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename... Expressions>
  Context& serialize(Context& context, const order_by_data_t<Expressions...>& t)
  {
    context << " ORDER BY ";
    interpret_tuple(t._expressions, ',', context);
    return context;
  }

  template <typename... T>
  auto order_by(T... t) -> decltype(statement_t<no_order_by_t>().order_by(std::forward<T>(t)...))
  {
    return statement_t<no_order_by_t>().order_by(std::move(t)...);
  }

}  // namespace sqlpp
