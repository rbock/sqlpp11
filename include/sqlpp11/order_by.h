/*
 * Copyright (c) 2013-2015, Roland Bock
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

#ifndef SQLPP_ORDER_BY_H
#define SQLPP_ORDER_BY_H

#include <tuple>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  // ORDER BY DATA
  template <typename Database, typename... Expressions>
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
    interpretable_list_t<Database> _dynamic_expressions;
  };

  struct assert_no_unknown_tables_in_order_by_t
  {
    using type = std::false_type;

    template <typename T = void>
    static void _()
    {
      static_assert(wrong_t<T>::value,
                    "at least one order-by expression requires a table which is otherwise not known in the statement");
    }
  };

  // ORDER BY
  template <typename Database, typename... Expressions>
  struct order_by_t
  {
    using _traits = make_traits<no_value_t, tag::is_order_by>;
    using _nodes = detail::type_vector<Expressions...>;

    using _is_dynamic = is_database<Database>;

    // Data
    using _data_t = order_by_data_t<Database, Expressions...>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      template <typename Expression>
      void add_ntc(Expression expression)
      {
        add<Expression, std::false_type>(expression);
      }

      template <typename Expression, typename TableCheckRequired = std::true_type>
      void add(Expression expression)
      {
        static_assert(_is_dynamic::value, "add() must not be called for static order_by");
        static_assert(is_sort_order_t<Expression>::value, "invalid expression argument in order_by::add()");
        static_assert(TableCheckRequired::value or Policies::template _no_unknown_tables<Expression>::value,
                      "expression uses tables unknown to this statement in order_by::add()");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Expression>;
        _serialize_check::_();

        using ok = logic::all_t<_is_dynamic::value, is_sort_order_t<Expression>::value, _serialize_check::type::value>;

        _add_impl(expression, ok());  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename Expression>
      void _add_impl(Expression expression, const std::true_type&)
      {
        return _data._dynamic_expressions.emplace_back(expression);
      }

      template <typename Expression>
      void _add_impl(Expression expression, const std::false_type&);

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = order_by_data_t<Database, Expressions...>;

      _impl_t<Policies> order_by;
      _impl_t<Policies>& operator()()
      {
        return order_by;
      }
      const _impl_t<Policies>& operator()() const
      {
        return order_by;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.order_by)
      {
        return t.order_by;
      }

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<order_by_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_order_by_t>::type;
    };
  };

  // NO ORDER BY YET
  struct no_order_by_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = no_data_t;

      _impl_t<Policies> no_order_by;
      _impl_t<Policies>& operator()()
      {
        return no_order_by;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_order_by;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_order_by)
      {
        return t.no_order_by;
      }

      using _database_t = typename Policies::_database_t;

      template <typename... T>
      using _check = logic::all_t<is_sort_order_t<T>::value...>;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check::value, Policies, no_order_by_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Expressions>
      auto order_by(Expressions... expressions) const
          -> _new_statement_t<_check<Expressions...>, order_by_t<void, Expressions...>>
      {
        static_assert(sizeof...(Expressions), "at least one expression (e.g. a column) required in order_by()");
        static_assert(_check<Expressions...>::value, "at least one argument is not a sort order in order_by()");

        return _order_by_impl<void>(_check<Expressions...>{}, expressions...);
      }

      template <typename... Expressions>
      auto dynamic_order_by(Expressions... expressions) const
          -> _new_statement_t<_check<Expressions...>, order_by_t<_database_t, Expressions...>>
      {
        static_assert(not std::is_same<_database_t, void>::value,
                      "dynamic_order_by must not be called in a static statement");
        static_assert(_check<Expressions...>::value, "at least one argument is not a sort order in order_by()");

        return _order_by_impl<_database_t>(_check<Expressions...>{}, expressions...);
      }

    private:
      template <typename Database, typename... Expressions>
      auto _order_by_impl(const std::false_type&, Expressions... expressions) const -> bad_statement;

      template <typename Database, typename... Expressions>
      auto _order_by_impl(const std::true_type&, Expressions... expressions) const
          -> _new_statement_t<std::true_type, order_by_t<Database, Expressions...>>
      {
        static_assert(not detail::has_duplicates<Expressions...>::value,
                      "at least one duplicate argument detected in order_by()");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                order_by_data_t<Database, Expressions...>{expressions...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename... Expressions>
  struct serializer_t<Context, order_by_data_t<Database, Expressions...>>
  {
    using _serialize_check = serialize_check_of<Context, Expressions...>;
    using T = order_by_data_t<Database, Expressions...>;

    static Context& _(const T& t, Context& context)
    {
      if (sizeof...(Expressions) == 0 and t._dynamic_expressions.empty())
        return context;
      context << " ORDER BY ";
      interpret_tuple(t._expressions, ',', context);
      if (sizeof...(Expressions) and not t._dynamic_expressions.empty())
        context << ',';
      interpret_list(t._dynamic_expressions, ',', context);
      return context;
    }
  };
}

#endif
