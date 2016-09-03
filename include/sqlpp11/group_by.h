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

#ifndef SQLPP_GROUP_BY_H
#define SQLPP_GROUP_BY_H

#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/type_traits.h>
#include <tuple>

namespace sqlpp
{
  // GROUP BY DATA
  template <typename Database, typename... Expressions>
  struct group_by_data_t
  {
    group_by_data_t(Expressions... expressions) : _expressions(expressions...)
    {
    }

    group_by_data_t(const group_by_data_t&) = default;
    group_by_data_t(group_by_data_t&&) = default;
    group_by_data_t& operator=(const group_by_data_t&) = default;
    group_by_data_t& operator=(group_by_data_t&&) = default;
    ~group_by_data_t() = default;

    std::tuple<Expressions...> _expressions;
    interpretable_list_t<Database> _dynamic_expressions;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_group_by_t,
      "at least one group-by expression requires a table which is otherwise not known in the statement");

  // GROUP BY
  template <typename Database, typename... Expressions>
  struct group_by_t
  {
    using _traits = make_traits<no_value_t, tag::is_group_by>;
    using _nodes = detail::type_vector<Expressions...>;

    using _is_dynamic = is_database<Database>;
    using _provided_aggregates = typename std::
        conditional<_is_dynamic::value, detail::type_set<>, detail::make_type_set_t<Expressions...>>::type;

    // Data
    using _data_t = group_by_data_t<Database, Expressions...>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      template <typename Expression>
      void add(Expression expression)
      {
        static_assert(_is_dynamic::value, "add() must not be called for static group_by");
        static_assert(is_expression_t<Expression>::value, "invalid expression argument in group_by::add()");
        static_assert(Policies::template _no_unknown_tables<Expression>::value,
                      "expression uses tables unknown to this statement in group_by::add()");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Expression>;
        _serialize_check{};

        using ok = logic::all_t<_is_dynamic::value, is_expression_t<Expression>::value, _serialize_check::type::value>;

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
      using _data_t = group_by_data_t<Database, Expressions...>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : group_by{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> group_by;
      _impl_t<Policies>& operator()()
      {
        return group_by;
      }
      const _impl_t<Policies>& operator()() const
      {
        return group_by;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.group_by)
      {
        return t.group_by;
      }

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<group_by_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_group_by_t>::type;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_group_by_args_are_expressions_t,
                               "arguments for group_by() must be valid expressions");
  template <typename... Exprs>
  struct check_group_by
  {
    using type = static_combined_check_t<
        static_check_t<logic::all_t<is_expression_t<Exprs>::value...>::value, assert_group_by_args_are_expressions_t>>;
  };
  template <typename... Exprs>
  using check_group_by_t = typename check_group_by<Exprs...>::type;

  // NO GROUP BY YET
  struct no_group_by_t
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
      _base_t(Args&&... args) : no_group_by{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_group_by;
      _impl_t<Policies>& operator()()
      {
        return no_group_by;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_group_by;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_group_by)
      {
        return t.no_group_by;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_group_by_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Expressions>
      auto group_by(Expressions... expressions) const
          -> _new_statement_t<check_group_by_t<Expressions...>, group_by_t<void, Expressions...>>
      {
        static_assert(sizeof...(Expressions), "at least one expression (e.g. a column) required in group_by()");

        return _group_by_impl<void>(check_group_by_t<Expressions...>{}, expressions...);
      }

      template <typename... Expressions>
      auto dynamic_group_by(Expressions... expressions) const
          -> _new_statement_t<check_group_by_t<Expressions...>, group_by_t<_database_t, Expressions...>>
      {
        static_assert(not std::is_same<_database_t, void>::value,
                      "dynamic_group_by must not be called in a static statement");

        return _group_by_impl<_database_t>(check_group_by_t<Expressions...>{}, expressions...);
      }

    private:
      template <typename Database, typename Check, typename... Expressions>
      auto _group_by_impl(Check, Expressions... expressions) const -> inconsistent<Check>;

      template <typename Database, typename... Expressions>
      auto _group_by_impl(consistent_t, Expressions... expressions) const
          -> _new_statement_t<consistent_t, group_by_t<Database, Expressions...>>
      {
        static_assert(not detail::has_duplicates<Expressions...>::value,
                      "at least one duplicate argument detected in group_by()");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                group_by_data_t<Database, Expressions...>{expressions...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename... Expressions>
  struct serializer_t<Context, group_by_data_t<Database, Expressions...>>
  {
    using _serialize_check = serialize_check_of<Context, Expressions...>;
    using T = group_by_data_t<Database, Expressions...>;

    static Context& _(const T& t, Context& context)
    {
      if (sizeof...(Expressions) == 0 and t._dynamic_expressions.empty())
        return context;
      context << " GROUP BY ";
      interpret_tuple(t._expressions, ',', context);
      if (sizeof...(Expressions) and not t._dynamic_expressions.empty())
        context << ',';
      interpret_list(t._dynamic_expressions, ',', context);
      return context;
    }
  };

  template <typename... T>
  auto group_by(T&&... t) -> decltype(statement_t<void, no_group_by_t>().group_by(std::forward<T>(t)...))
  {
    return statement_t<void, no_group_by_t>().group_by(std::forward<T>(t)...);
  }

  template <typename Database, typename... T>
  auto dynamic_group_by(const Database&, T&&... t)
      -> decltype(statement_t<Database, no_group_by_t>().dynamic_group_by(std::forward<T>(t)...))
  {
    return statement_t<Database, no_group_by_t>().dynamic_group_by(std::forward<T>(t)...);
  }
}

#endif
