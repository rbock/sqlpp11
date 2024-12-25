#pragma once

/*
 * Copyright (c) 2013, Roland Bock
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

#include <tuple>
#include <type_traits>
#include <array>
#include <vector>

#include <sqlpp11/core/name/name_tag.h>
#include <sqlpp11/core/compat/optional.h>
#include <sqlpp11/core/compat/string_view.h>
#include <sqlpp11/core/compat/type_traits.h>
#include <sqlpp11/core/compat/span.h>
#include <sqlpp11/core/consistent.h>
#include <sqlpp11/core/portable_static_assert.h>
#include <sqlpp11/core/detail/type_vector.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/detail/get_first.h>
#include <sqlpp11/core/type_traits/aggregates.h>
#include <sqlpp11/core/type_traits/group_by_column.h>
#include <sqlpp11/core/type_traits/nodes_of.h>
#include <sqlpp11/core/type_traits/optional.h>
#include <sqlpp11/core/type_traits/value_type.h>
#include <sqlpp11/core/type_traits/ctes_of.h>
#include <sqlpp11/core/type_traits/tables_of.h>

namespace sqlpp
{
  template <typename T>
  const T& get_value(const T& t)
  {
    return t;
  }

  template <typename T>
  const T& get_value(const ::sqlpp::optional<T>& t)
  {
    return t.value();
  }

  template <typename T>
  auto has_value(const T&) -> bool
  {
    return true;
  }

  template <typename T>
  auto has_value(const ::sqlpp::optional<T>& t) -> bool
  {
    return t.has_value();
  }

  template <typename T>
  struct has_default : public std::false_type
  {
  };

  template <typename T>
  struct can_be_null : public is_optional<value_type_of_t<T>> {};

  template <>
  struct can_be_null<sqlpp::nullopt_t> : public std::true_type {};

  template <typename T>
  struct dynamic_t;

  template <typename T>
  struct is_dynamic : public std::false_type
  {
  };

  template <typename T>
  struct is_dynamic<dynamic_t<T>> : public std::true_type
  {
  };

  template <typename L, typename R>
  struct values_are_comparable
      : public std::integral_constant<bool,
                                      (is_blob<L>::value and is_blob<R>::value) or
                                          (is_boolean<L>::value and is_boolean<R>::value) or
                                          (is_numeric<L>::value and is_numeric<R>::value) or
                                          (is_text<L>::value and is_text<R>::value) or
                                          (is_day_or_time_point<L>::value and is_day_or_time_point<R>::value) or
                                          (is_time_of_day<L>::value and is_time_of_day<R>::value)>
  {
  };

  template<typename T>
    struct result_value {};

  template<typename T>
    struct result_value<::sqlpp::optional<T>>
    {
      using type = ::sqlpp::optional<typename result_value<T>::type>;
    };

  template <typename T>
    using result_value_t = typename result_value<T>::type;

  template<>
    struct result_value<blob> { using type = ::sqlpp::span<uint8_t>; };

  template<>
    struct result_value<boolean> { using type = bool; };

  template<>
    struct result_value<integral> { using type = int64_t; };

  template<>
    struct result_value<unsigned_integral> { using type = uint64_t; };

  template<>
    struct result_value<floating_point> { using type = double; };

  template<>
    struct result_value<text> { using type = ::sqlpp::string_view; };

  template<>
    struct result_value<day_point> { using type = std::chrono::time_point<std::chrono::system_clock, sqlpp::chrono::days>; };
  template<>
    struct result_value<time_of_day> { using type = std::chrono::microseconds; };

  template<>
    struct result_value<time_point> { using type = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>; };

  template<typename T>
    struct parameter_value {};

  template<typename T>
    struct parameter_value<::sqlpp::optional<T>>
    {
      using type = ::sqlpp::optional<typename parameter_value<T>::type>;
    };

  template <typename T>
    using parameter_value_t = typename parameter_value<T>::type;

  template<>
    struct parameter_value<blob> { using type = std::vector<uint8_t>; };

  template<>
    struct parameter_value<boolean> { using type = bool; };

  template<>
    struct parameter_value<integral> { using type = int64_t; };

  template<>
    struct parameter_value<unsigned_integral> { using type = uint64_t; };

  template<>
    struct parameter_value<floating_point> { using type = double; };

  template<>
    struct parameter_value<text> { using type = std::string; };

  template<>
    struct parameter_value<day_point> { using type = std::chrono::time_point<std::chrono::system_clock, sqlpp::chrono::days>; };
  template<>
    struct parameter_value<time_of_day> { using type = std::chrono::microseconds; };

  template<>
    struct parameter_value<time_point> { using type = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>; };

  template <typename T>
  struct is_assignment : public std::false_type
  {
  };

  template <typename T>
  struct lhs
  {
    using type = void;
  };

  template <typename T>
  struct lhs<dynamic_t<T>>
  {
    using type = dynamic_t<typename lhs<T>::type>;
  };

  template <typename T>
  using lhs_t = typename lhs<T>::type;

  template <typename T>
  struct rhs
  {
    using type = void;
  };

  template <typename T>
  struct rhs<dynamic_t<T>>
  {
    using type = dynamic_t<typename rhs<T>::type>;
  };

  template <typename T>
  using rhs_t = typename rhs<T>::type;

  template<typename T>
  struct parameters_of
  {
    using type = typename parameters_of<nodes_of_t<T>>::type;
  };

  template<typename... T>
  struct parameters_of<detail::type_vector<T...>>
  {
    using type = detail::type_vector_cat_t<typename parameters_of<T>::type...>;
  };

  template <typename T>
  using parameters_of_t = typename parameters_of<T>::type;

  // Something that can be used as a table
  template <typename T>
  struct is_table : public std::false_type{};

  // Really a table, not a `table AS ...`, `JOIN` or `CTE` or `SELECT ... AS`
  template <typename T>
  struct is_raw_table : public std::false_type{};

  template <typename T>
  struct is_column : public std::false_type{};

  template <typename NameTagProvider, typename Member>
  using member_t = typename name_tag_of_t<NameTagProvider>::template _member_t<Member>;

  template <typename Policies>
  using derived_statement_t = typename Policies::_statement_t;

  template <typename T>
  using is_inconsistent_t =
      typename std::conditional<std::is_same<consistent_t, T>::value, std::false_type, std::true_type>::type;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_sqlpp_type_t, "expression is not an sqlpp type, consistency cannot be verified");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_run_statement_or_prepared_t,
                               "connection cannot run something that is neither statement nor prepared statement");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_prepare_statement_t,
                               "connection cannot prepare something that is not a statement");

  template<typename T>
    struct is_statement : public std::false_type {};

  template<typename T>
    struct is_prepared_statement : public std::false_type {};

  template <typename Context, typename T, typename Enable = void>
  struct run_check
  {
    using type = assert_run_statement_or_prepared_t;
  };

  template <typename Context, typename T>
  struct run_check<Context,
                   T,
                   typename std::enable_if<is_statement<T>::value or is_prepared_statement<T>::value>::type>
  {
    using type =
        detail::get_first_if<is_inconsistent_t, consistent_t, typename T::_run_check>;
  };

  template <typename Context, typename T>
  using run_check_t = typename run_check<Context, T>::type;

  template <typename Context, typename T, typename Enable = void>
  struct prepare_check
  {
    using type = assert_prepare_statement_t;
  };

  template <typename Context, typename T>
  struct prepare_check<Context, T, typename std::enable_if<is_statement<T>::value>::type>
  {
    using type = detail::
        get_first_if<is_inconsistent_t, consistent_t, typename T::_prepare_check>;
  };

  template <typename Context, typename T>
  using prepare_check_t = typename prepare_check<Context, T>::type;

  template <typename Statement, typename Enable = void>
  struct has_result_row: public std::false_type
  {
  };

#warning: Can we make this more explicit for statement_t?
  template <typename Statement>
  struct has_result_row<
      Statement,
      typename std::enable_if<
          not wrong_t<typename Statement::_result_methods_t::template _result_row_t<void>>::value,
          void>::type>: public std::true_type
  {
  };

  template <typename Statement, typename Enable = void>
  struct get_result_row_impl
  {
    using type = void;
  };

  template <typename Statement>
  struct get_result_row_impl<
      Statement,
      typename std::enable_if<
          not wrong_t<typename Statement::_result_methods_t::template _result_row_t<void>>::value,
          void>::type>
  {
    using type = typename Statement::_result_methods_t::template _result_row_t<void>;
  };

  template <typename Statement>
  using get_result_row_t = typename get_result_row_impl<Statement>::type;

  template<typename T>
  struct requires_parentheses : public std::false_type {};

  template<typename T>
  struct table_ref {
    using type = T;
  };

  template<typename T>
  using table_ref_t = typename table_ref<T>::type;

  template <typename T>
    struct is_select_flag : public std::false_type {};

  template <typename T>
    struct is_sort_order : public std::false_type {};

  template<typename T>
    struct is_result_clause : public std::false_type {};

  template <typename T>
  struct is_cte : public std::false_type
  {
  };

  template <typename T>
  struct is_expression_as : public std::false_type
  {
  };

  template <typename T>
  struct is_recursive_cte : public std::false_type
  {
  };

  template <typename T>
  struct is_pre_join : public std::false_type
  {
  };

  template<typename T>
    struct required_insert_columns_of
    {
      using type = detail::type_set<>;
    };
  template<typename T>
    using required_insert_columns_of_t = typename required_insert_columns_of<T>::type;

  template<typename T>
    struct is_where_required : public std::false_type {};

  template<typename T>
    struct is_clause : public std::false_type {};

#warning: Need to determine those clauses
  // Used to determine if a required clause is still missing, e.g. if no columns were selected in a select
  template<typename T>
    struct is_missing : public std::false_type {};

  // Not implemented to ensure implementation for every clause.
  template<typename Statement, typename Clause>
    struct consistency_check;

  template<typename Statement, typename Clause>
    using consistency_check_t = typename consistency_check<Statement, Clause>::type;

  // Not implemented to ensure implementation for statement_t
  template<typename Statement>
    struct statement_consistency_check;

  template<typename Statement>
    using statement_consistency_check_t = typename statement_consistency_check<Statement>::type;

}  // namespace sqlpp
