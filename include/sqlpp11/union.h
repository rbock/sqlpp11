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

#ifndef SQLPP_UNION_H
#define SQLPP_UNION_H

#include <sqlpp11/expression.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/logic.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/result_row.h>
#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/union_data.h>
#include <sqlpp11/union_flags.h>

namespace sqlpp
{
  struct no_union_t;

  using blank_union_t = statement_t<void, no_union_t>;
  // There is no order by or limit or offset in union, use it as a pseudo table to do that.

  template <typename Check, typename Union>
  struct union_statement_impl
  {
    using type = Check;
  };

  template <typename Union>
  struct union_statement_impl<consistent_t, Union>
  {
    using type = statement_t<void, Union, no_union_t>;
  };

  template <typename Check, typename Union>
  using union_statement_t = typename union_statement_impl<Check, Union>::type;

  // UNION(EXPR)
  template <typename Database, typename Flag, typename Lhs, typename Rhs>
  struct union_t
  {
    using _traits = make_traits<no_value_t, tag::is_union, tag::is_return_value>;
    using _nodes = detail::type_vector<Lhs, Rhs>;

    using _alias_t = struct
    {
    };

    // Data
    using _data_t = union_data_t<Database, Flag, Lhs, Rhs>;

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
      using _data_t = union_data_t<Database, Flag, Lhs, Rhs>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : union_{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> union_;
      _impl_t<Policies>& operator()()
      {
        return union_;
      }
      const _impl_t<Policies>& operator()() const
      {
        return union_;
      }

      using _selected_columns_t = typename std::decay<decltype(union_._data._lhs.get_selected_columns())>::type;
      _selected_columns_t& get_selected_columns()
      {
        return union_._data._lhs.get_selected_columns();
      }
      const _selected_columns_t& get_selected_columns() const
      {
        return union_._data._lhs.get_selected_columns();
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.union_)
      {
        return t.union_;
      }

      using _consistency_check = detail::get_first_if<is_inconsistent_t,
                                                      consistent_t,
                                                      typename Lhs::_consistency_check,
                                                      typename Rhs::_consistency_check>;
    };

    template <typename Statement>
    using _result_methods_t = typename Lhs::template _result_methods_t<Statement>;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_union_args_are_statements_t, "arguments for union() must be statements");
  template <typename... T>
  struct check_union
  {
    using type = static_combined_check_t<
        static_check_t<logic::all_t<is_statement_t<T>::value...>::value, assert_union_args_are_statements_t>>;
  };
  template <typename... T>
  using check_union_t = typename check_union<T...>::type;

  // NO UNION YET
  struct no_union_t
  {
    using _traits = make_traits<no_value_t, tag::is_union>;
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
      _base_t(Args&&... args) : no_union{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_union;
      _impl_t<Policies>& operator()()
      {
        return no_union;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_union;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_union)
      {
        return t.no_union;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = union_statement_t<Check, T>;

      using _consistency_check = consistent_t;

      template <typename Rhs>
      auto union_distinct(Rhs rhs) const
          -> _new_statement_t<check_union_t<derived_statement_t<Policies>, Rhs>,
                              union_t<void, union_distinct_t, derived_statement_t<Policies>, Rhs>>
      {
        static_assert(is_statement_t<Rhs>::value, "argument of union call has to be a statement");
        static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
        static_assert(has_result_row_t<Rhs>::value, "argument of a union has to be a complete select statement");
        static_assert(has_result_row_t<derived_statement_t<Policies>>::value,
                      "left hand side argument of a union has to be a complete select statement or union");

        using _result_row_t = get_result_row_t<Rhs>;
        static_assert(std::is_same<get_result_row_t<derived_statement_t<Policies>>, _result_row_t>::value,
                      "both arguments in a union have to have the same result columns (type and name)");
        static_assert(is_static_result_row_t<_result_row_t>::value, "unions must not have dynamically added columns");

        return _union_impl<void, union_distinct_t>(check_union_t<derived_statement_t<Policies>, Rhs>{}, rhs);
      }

      template <typename Rhs>
      auto union_all(Rhs rhs) const -> _new_statement_t<check_union_t<derived_statement_t<Policies>, Rhs>,
                                                        union_t<void, union_all_t, derived_statement_t<Policies>, Rhs>>
      {
        static_assert(is_statement_t<Rhs>::value, "argument of union call has to be a statement");
        static_assert(has_policy_t<Rhs, is_select_t>::value, "argument of union call has to be a select");
        static_assert(has_result_row_t<Rhs>::value, "argument of a union has to be a (complete) select statement");
        static_assert(has_result_row_t<derived_statement_t<Policies>>::value,
                      "left hand side argument of a union has to be a (complete) select statement");

        using _result_row_t = get_result_row_t<Rhs>;
        static_assert(std::is_same<get_result_row_t<derived_statement_t<Policies>>, _result_row_t>::value,
                      "both arguments in a union have to have the same result columns (type and name)");
        static_assert(is_static_result_row_t<_result_row_t>::value, "unions must not have dynamically added columns");

        return _union_impl<void, union_all_t>(check_union_t<derived_statement_t<Policies>, Rhs>{}, rhs);
      }

    private:
      template <typename Database, typename Flag, typename Check, typename Rhs>
      auto _union_impl(Check, Rhs rhs) const -> inconsistent<Check>;

      template <typename Database, typename Flag, typename Rhs>
      auto _union_impl(consistent_t, Rhs rhs) const
          -> _new_statement_t<consistent_t, union_t<Database, Flag, derived_statement_t<Policies>, Rhs>>
      {
        return {blank_union_t{}, union_data_t<Database, Flag, derived_statement_t<Policies>, Rhs>{
                                     static_cast<const derived_statement_t<Policies>&>(*this), rhs}};
      }
    };
  };

  template <typename T>
  auto union_all(T&& t) -> decltype(statement_t<void, no_union_t>().union_all(std::forward<T>(t)))
  {
    return statement_t<void, no_union_t>().union_all(std::forward<T>(t));
  }

  template <typename T>
  auto union_distinct(T&& t) -> decltype(statement_t<void, no_union_t>().union_distinct(std::forward<T>(t)))
  {
    return statement_t<void, no_union_t>().union_distinct(std::forward<T>(t));
  }
}

#endif
