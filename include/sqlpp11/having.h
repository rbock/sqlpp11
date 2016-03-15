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

#ifndef SQLPP_HAVING_H
#define SQLPP_HAVING_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/logic.h>

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
    interpretable_list_t<Database> _dynamic_expressions;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(
      assert_no_unknown_tables_in_having_t,
      "at least one having-expression requires a table which is otherwise not known in the statement");

  // HAVING
  template <typename Database, typename Expression>
  struct having_t
  {
    using _traits = make_traits<no_value_t, tag::is_having>;
    using _nodes = detail::type_vector<Expression>;

    using _is_dynamic = is_database<Database>;

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

      template <typename Expr>
      void add_ntc(Expr expression)
      {
        add<Expr, std::false_type>(expression);
      }

      template <typename Expr, typename TableCheckRequired = std::true_type>
      void add(Expr expression)
      {
        static_assert(_is_dynamic::value, "having::add() can only be called for dynamic_having");
        static_assert(is_expression_t<Expr>::value, "invalid expression argument in having::add()");
        static_assert(not TableCheckRequired::value or Policies::template _no_unknown_tables<Expr>::value,
                      "expression uses tables unknown to this statement in having::add()");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Expr>;
        _serialize_check::_();

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
      using _data_t = having_data_t<Database, Expression>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args)
          : having{std::forward<Args>(args)...}
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

      using _consistency_check = typename std::conditional<Policies::template _no_unknown_tables<having_t>::value,
                                                           consistent_t,
                                                           assert_no_unknown_tables_in_having_t>::type;
    };
  };

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
      _base_t(Args&&... args)
          : no_having{std::forward<Args>(args)...}
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
      struct _check : logic::all_t<is_expression_t<T>::value...>
      {
      };

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check::value, Policies, no_having_t, T>;

      using _consistency_check = consistent_t;

      template <typename Expression>
      auto having(Expression expression) const -> _new_statement_t<_check<Expression>, having_t<void, Expression>>
      {
        static_assert(_check<Expression>::value, "at least one argument is not an expression in having()");

        return _having_impl<void>(_check<Expression>{}, expression);
      }

      template <typename Expression>
      auto dynamic_having(Expression expression) const
          -> _new_statement_t<_check<Expression>, having_t<_database_t, Expression>>
      {
        static_assert(_check<Expression>::value, "at least one argument is not an expression in having()");
        static_assert(not std::is_same<_database_t, void>::value,
                      "dynamic_having must not be called in a static statement");
        return _having_impl<_database_t>(_check<Expression>{}, expression);
      }

    private:
      template <typename Database, typename Expression>
      auto _having_impl(const std::false_type&, Expression expression) const -> bad_statement;

      template <typename Database, typename Expression>
      auto _having_impl(const std::true_type&, Expression expression) const
          -> _new_statement_t<std::true_type, having_t<Database, Expression>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this),
                having_data_t<Database, Expression>{expression}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename Expression>
  struct serializer_t<Context, having_data_t<Database, Expression>>
  {
    using _serialize_check = serialize_check_of<Context, Expression>;
    using T = having_data_t<Database, Expression>;

    static Context& _(const T& t, Context& context)
    {
      context << " HAVING ";
      serialize(t._expression, context);
      if (not t._dynamic_expressions.empty())
        context << " AND ";
      interpret_list(t._dynamic_expressions, " AND ", context);
      return context;
    }
  };
}

#endif
