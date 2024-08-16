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

#warning: remove this
  template <typename T>
  struct is_not_cpp_bool_t
  {
    static constexpr bool value = not std::is_same<T, bool>::value;
  };

  //
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
#define SQLPP_VALUE_TRAIT_GENERATOR(name)                                                                   \
  namespace tag                                                                                             \
  {                                                                                                         \
    struct name;                                                                                            \
  }                                                                                                         \
  namespace detail                                                                                          \
  {                                                                                                         \
    template <typename T, typename Enable = void>                                                           \
    struct name##_impl                                                                                      \
    {                                                                                                       \
      using type = std::false_type;                                                                         \
    };                                                                                                      \
    template <typename T>                                                                                   \
    struct name##_impl<                                                                                     \
        T,                                                                                                  \
        typename std::enable_if<detail::is_element_of<tag::name, typename T::_traits::_tags>::value>::type> \
    {                                                                                                       \
      using type = std::true_type;                                                                          \
    };                                                                                                      \
  }                                                                                                         \
  template <typename T>                                                                                     \
  using name##_t = typename detail::name##_impl<T>::type;

  SQLPP_VALUE_TRAIT_GENERATOR(is_sql_null)
  SQLPP_VALUE_TRAIT_GENERATOR(is_value_type)
  SQLPP_VALUE_TRAIT_GENERATOR(is_selectable)
  SQLPP_VALUE_TRAIT_GENERATOR(is_expression)
  SQLPP_VALUE_TRAIT_GENERATOR(is_multi_expression)
  SQLPP_VALUE_TRAIT_GENERATOR(is_alias)
  SQLPP_VALUE_TRAIT_GENERATOR(is_select_flag)
  SQLPP_VALUE_TRAIT_GENERATOR(is_union_flag)
  SQLPP_VALUE_TRAIT_GENERATOR(is_result_field)

  SQLPP_VALUE_TRAIT_GENERATOR(is_statement)
  SQLPP_VALUE_TRAIT_GENERATOR(is_prepared_statement)
  SQLPP_VALUE_TRAIT_GENERATOR(is_union)
  SQLPP_VALUE_TRAIT_GENERATOR(is_with)
  SQLPP_VALUE_TRAIT_GENERATOR(is_cte)
  SQLPP_VALUE_TRAIT_GENERATOR(is_noop)
  SQLPP_VALUE_TRAIT_GENERATOR(is_missing)
  SQLPP_VALUE_TRAIT_GENERATOR(is_return_value)
  SQLPP_VALUE_TRAIT_GENERATOR(is_raw_table)
  SQLPP_VALUE_TRAIT_GENERATOR(is_pre_join)
  SQLPP_VALUE_TRAIT_GENERATOR(is_join)
  SQLPP_VALUE_TRAIT_GENERATOR(is_dynamic_pre_join)
  SQLPP_VALUE_TRAIT_GENERATOR(is_dynamic_join)
  SQLPP_VALUE_TRAIT_GENERATOR(is_pseudo_table)
  SQLPP_VALUE_TRAIT_GENERATOR(is_column)
  SQLPP_VALUE_TRAIT_GENERATOR(is_select)
  SQLPP_VALUE_TRAIT_GENERATOR(is_select_flag_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_select_column_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_from)
  SQLPP_VALUE_TRAIT_GENERATOR(is_single_table)
  SQLPP_VALUE_TRAIT_GENERATOR(is_into)
  SQLPP_VALUE_TRAIT_GENERATOR(is_on)
  SQLPP_VALUE_TRAIT_GENERATOR(is_where)
  SQLPP_VALUE_TRAIT_GENERATOR(is_group_by)
  SQLPP_VALUE_TRAIT_GENERATOR(is_having)
  SQLPP_VALUE_TRAIT_GENERATOR(is_order_by)
  SQLPP_VALUE_TRAIT_GENERATOR(is_limit)
  SQLPP_VALUE_TRAIT_GENERATOR(is_for_update)
  SQLPP_VALUE_TRAIT_GENERATOR(is_offset)
  SQLPP_VALUE_TRAIT_GENERATOR(is_using_)
  SQLPP_VALUE_TRAIT_GENERATOR(is_column_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_value_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_update_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_insert_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_insert_value)
  SQLPP_VALUE_TRAIT_GENERATOR(is_insert_value_list)
  SQLPP_VALUE_TRAIT_GENERATOR(is_parameter)

  SQLPP_VALUE_TRAIT_GENERATOR(enforce_null_result_treatment)

  template <typename Tag, bool Condition>
  using tag_if = typename std::conditional<Condition, Tag, void>::type;

  template <typename Database>
  using is_database =
      typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

#define SQLPP_RECURSIVE_TRAIT_SET_GENERATOR(trait)                                      \
  namespace detail                                                                      \
  {                                                                                     \
    template <typename T, typename Leaf = void>                                         \
    struct trait##_of_impl                                                              \
    {                                                                                   \
      using type = typename trait##_of_impl<nodes_of_t<T>>::type;                       \
    };                                                                                  \
    template <typename T>                                                               \
    struct trait##_of_impl<T, ::sqlpp::void_t<typename T::_##trait>>                     \
    {                                                                                   \
      using type = typename T::_##trait;                                                \
    };                                                                                  \
    template <typename... Nodes>                                                        \
    struct trait##_of_impl<type_vector<Nodes...>, void>                                 \
    {                                                                                   \
      using type = detail::make_joined_set_t<typename trait##_of_impl<Nodes>::type...>; \
    };                                                                                  \
  }                                                                                     \
  template <typename T>                                                                 \
  using trait##_of = typename detail::trait##_of_impl<T>::type;

  SQLPP_RECURSIVE_TRAIT_SET_GENERATOR(required_ctes)
  SQLPP_RECURSIVE_TRAIT_SET_GENERATOR(provided_ctes)
  SQLPP_RECURSIVE_TRAIT_SET_GENERATOR(provided_tables)
  SQLPP_RECURSIVE_TRAIT_SET_GENERATOR(provided_outer_tables)
  SQLPP_RECURSIVE_TRAIT_SET_GENERATOR(provided_aggregates)

  template <typename T>
  struct lhs
  {
    using type = void;
  };

  template <typename T>
  using lhs_t = typename lhs<T>::type;

  template <typename T>
  struct rhs
  {
    using type = void;
  };

  template <typename T>
  using rhs_t = typename rhs<T>::type;

  // Anything that directly requires a table (e.g. a column) has to specialize required_tables_of.
  template<typename T>
  struct required_tables_of
  {
    using type = typename required_tables_of<nodes_of_t<T>>::type;
  };

  template<typename... T>
  struct required_tables_of<detail::type_vector<T...>>
  {
    using type = detail::make_joined_set_t<typename required_tables_of<T>::type...>;
  };

  template<typename T>
    using required_tables_of_t = typename required_tables_of<T>::type;

  static_assert(required_tables_of_t<int>::size::value == 0, "");

  template <typename ValueType, typename T>
  struct is_valid_operand
  {
    static constexpr bool value =
        is_expression_t<T>::value                            // expressions are OK
        and ValueType::template _is_valid_operand<T>::value  // the correct value type is required, of course
        ;
  };

#warning: This should go away
  namespace detail
  {
    template <typename KnownAggregates, typename T, typename Leaf = void>
    struct is_aggregate_expression_impl
    {
      using type = typename is_aggregate_expression_impl<KnownAggregates, nodes_of_t<T>>::type;
    };
    template <typename KnownAggregates, typename T>
    struct is_aggregate_expression_impl<
        KnownAggregates,
        T,
        typename std::enable_if<T::_is_aggregate_expression::value>::type>
    {
      using type = std::true_type;
    };
    template <typename KnownAggregates, typename T>
    struct is_aggregate_expression_impl<
        KnownAggregates,
        T,
        typename std::enable_if<T::_is_literal_expression::value>::type>
    {
      using type = std::true_type;
    };
    template <typename KnownAggregates, typename T>
    struct is_aggregate_expression_impl<KnownAggregates,
                                        T,
                                        typename std::enable_if<detail::is_element_of<T, KnownAggregates>::value>::type>
    {
      using type = std::true_type;
    };
    template <typename KnownAggregates, typename... Nodes>
    struct is_aggregate_expression_impl<KnownAggregates, type_vector<Nodes...>, void>
    {
      using type =
          logic::all_t<sizeof...(Nodes) != 0, is_aggregate_expression_impl<KnownAggregates, Nodes>::type::value...>;
    };
  }  // namespace detail
  template <typename KnownAggregates, typename T>
  using is_aggregate_expression_t = typename detail::is_aggregate_expression_impl<KnownAggregates, T>::type;

  namespace detail
  {
    template <typename KnownAggregates, typename T, typename Leaf = void>
    struct is_non_aggregate_expression_impl
    {
      using type = typename is_non_aggregate_expression_impl<KnownAggregates, nodes_of_t<T>>::type;
    };
    template <typename KnownAggregates, typename T>
    struct is_non_aggregate_expression_impl<
        KnownAggregates,
        T,
        typename std::enable_if<T::_is_aggregate_expression::value>::type>
    {
      using type = std::false_type;
    };
    template <typename KnownAggregates, typename T>
    struct is_non_aggregate_expression_impl<
        KnownAggregates,
        T,
        typename std::enable_if<T::_is_literal_expression::value>::type>
    {
      using type = std::true_type;
    };
    template <typename KnownAggregates, typename T>
    struct is_non_aggregate_expression_impl<KnownAggregates,
                                        T,
                                        typename std::enable_if<detail::is_element_of<T, KnownAggregates>::value>::type>
    {
      using type = std::false_type;
    };
    template <typename KnownAggregates, typename... Nodes>
    struct is_non_aggregate_expression_impl<KnownAggregates, type_vector<Nodes...>, void>
    {
      using type =
          logic::any_t<sizeof...(Nodes) == 0,
                       logic::all_t<sizeof...(Nodes) != 0,
                                    is_non_aggregate_expression_impl<KnownAggregates, Nodes>::type::value...>::value>;
    };
  }  // namespace detail
  template <typename KnownAggregates, typename T>
  using is_non_aggregate_expression_t = typename detail::is_non_aggregate_expression_impl<KnownAggregates, T>::type;

  namespace detail
  {
    template <typename T, typename Leaf = void>
    struct parameters_of_impl
    {
      using type = typename parameters_of_impl<nodes_of_t<T>>::type;
    };
    template <typename T>
    struct parameters_of_impl<T, typename std::enable_if<std::is_class<typename T::_parameters>::value>::type>
    {
      using type = typename T::_parameters;
    };
    template <typename... Nodes>
    struct parameters_of_impl<type_vector<Nodes...>, void>
    {
      using type = detail::type_vector_cat_t<typename parameters_of_impl<Nodes>::type...>;
    };
  }  // namespace detail
  template <typename T>
  using parameters_of = typename detail::parameters_of_impl<T>::type;

  template <typename T>
  struct is_table : public std::false_type{};

  template <typename ValueType, typename... Tags>
  struct make_traits
  {
    using _value_type = ValueType;
    using _tags = detail::make_type_set_t<Tags...>;
  };

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

  template <typename T, typename Enable = void>
  struct consistency_check
  {
    using type = assert_sqlpp_type_t;
  };

  template <typename T>
  struct consistency_check<T, ::sqlpp::void_t<typename T::_consistency_check>>
  {
    using type = typename T::_consistency_check;
  };

  template <typename T>
  using consistency_check_t = typename consistency_check<T>::type;

  template <typename Context, typename T, typename Enable = void>
  struct run_check
  {
    using type = assert_run_statement_or_prepared_t;
  };

  template <typename Context, typename T>
  struct run_check<Context,
                   T,
                   typename std::enable_if<is_statement_t<T>::value or is_prepared_statement_t<T>::value>::type>
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
  struct prepare_check<Context, T, typename std::enable_if<is_statement_t<T>::value>::type>
  {
    using type = detail::
        get_first_if<is_inconsistent_t, consistent_t, typename T::_prepare_check>;
  };

  template <typename Context, typename T>
  using prepare_check_t = typename prepare_check<Context, T>::type;

  template <typename Statement, typename Enable = void>
  struct has_result_row_impl
  {
    using type = std::false_type;
  };

  template <typename Statement>
  struct has_result_row_impl<
      Statement,
      typename std::enable_if<
          not wrong_t<typename Statement::template _result_methods_t<Statement>::template _result_row_t<void>>::value,
          void>::type>
  {
    using type = std::true_type;
  };

  template <typename Statement>
  using has_result_row_t = typename has_result_row_impl<Statement>::type;

  template <typename Statement, typename Enable = void>
  struct get_result_row_impl
  {
    using type = void;
  };

  template <typename Statement>
  struct get_result_row_impl<
      Statement,
      typename std::enable_if<
          not wrong_t<typename Statement::template _result_methods_t<Statement>::template _result_row_t<void>>::value,
          void>::type>
  {
    using type = typename Statement::template _result_methods_t<Statement>::template _result_row_t<void>;
  };

  template <typename Statement>
  using get_result_row_t = typename get_result_row_impl<Statement>::type;

  template <typename Statement, template <typename> class Predicate, typename Enable = void>
  struct has_policy_impl
  {
    using type = std::false_type;
  };

  template <typename Statement, template <typename> class Predicate>
  struct has_policy_impl<Statement, Predicate, typename std::enable_if<is_statement_t<Statement>::value>::type>
  {
    using type = std::true_type;
  };

  template <typename Statement, template <typename> class Predicate>
  using has_policy_t = typename has_policy_impl<Statement, Predicate>::type;

  template<typename T>
  struct requires_parentheses : public std::false_type {};

  struct no_context_t
  {
  };
  template <typename Db, typename = void>
  struct serializer_context_of_impl
  {
    using type = no_context_t;
  };

  template <typename Db>
  struct serializer_context_of_impl<Db, ::sqlpp::void_t<typename Db::_serializer_context_t>>
  {
    using type = typename Db::_serializer_context_t;
  };

  template <typename T>
    struct is_sort_order : public std::false_type {};

  template <typename Db>
  using serializer_context_of = typename serializer_context_of_impl<Db>::type;
}  // namespace sqlpp
