/*
 * Copyright (c) 2013-2014, Roland Bock
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

#ifndef SQLPP_TYPE_TRAITS_H
#define SQLPP_TYPE_TRAITS_H

#include <type_traits>
#include <tuple>
#include <sqlpp11/serializer.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/detail/get_first.h>

namespace sqlpp
{
	namespace tag
	{
		struct can_be_null{};
		struct contains_aggregate_function{};
	};

	namespace detail
	{
		template<typename T, typename Enable = void>
		struct can_be_null_impl { using type = std::false_type; };
		template<typename T>
		struct can_be_null_impl<T, typename std::enable_if<is_element_of<tag::can_be_null, typename T::_recursive_traits::_tags>::value>::type> { using type = std::true_type; };
	}
	template<typename T>
	using can_be_null_t = typename detail::can_be_null_impl<T>::type;

	namespace detail
	{
		template<typename T, typename Enable = void>
		struct contains_aggregate_function_impl { using type = std::false_type; };
		template<typename T>
		struct contains_aggregate_function_impl<T, typename std::enable_if<is_element_of<tag::contains_aggregate_function, typename T::_recursive_traits::_tags>::value>::type> { using type = std::true_type; };
	}
	template<typename T>
	using contains_aggregate_function_t = typename detail::contains_aggregate_function_impl<T>::type;

	namespace detail
	{
		template<typename T, typename Enable = void>
		struct column_spec_can_be_null_impl { using type = std::false_type; };
		template<typename T>
		struct column_spec_can_be_null_impl<T, typename std::enable_if<detail::is_element_of<tag::can_be_null, typename T::_traits::_tags>::value>::type> { using type = std::true_type; };
	}
	template<typename T>
	using column_spec_can_be_null_t = typename detail::column_spec_can_be_null_impl<T>::type;

	namespace tag
	{
		struct is_expression{};
	};
	namespace detail
	{
		template<typename T, typename Enable = void>
		struct is_expression_impl { using type = std::false_type; };
		template<typename T>
		struct is_expression_impl<T, typename std::enable_if<
					detail::is_element_of<tag::is_expression, typename T::_traits::_tags>::value
					and not detail::is_element_of<tag::contains_aggregate_function, typename T::_recursive_traits::_tags>::value
					>::type> { using type = std::true_type; };
	}
	template<typename T>
	using is_expression_t = typename detail::is_expression_impl<T>::type;

#define SQLPP_VALUE_TRAIT_GENERATOR(name) \
	namespace tag\
	{\
		struct name{};\
	};\
	namespace detail\
	{\
		template<typename T, typename Enable = void>\
		struct name##_impl { using type = std::false_type; };\
		template<typename T>\
		struct name##_impl<T, typename std::enable_if<detail::is_element_of<tag::name, typename T::_traits::_tags>::value>::type> { using type = std::true_type; };\
	}\
	template<typename T>\
	using name##_t = typename detail::name##_impl<T>::type;

	SQLPP_VALUE_TRAIT_GENERATOR(is_value_type);
	SQLPP_VALUE_TRAIT_GENERATOR(is_sql_null);
	SQLPP_VALUE_TRAIT_GENERATOR(is_boolean);
	SQLPP_VALUE_TRAIT_GENERATOR(is_integral);
	SQLPP_VALUE_TRAIT_GENERATOR(is_floating_point);
	template<typename T>
		using is_numeric_t = logic::any_t<
		detail::is_element_of<tag::is_integral, typename T::_traits::_tags>::value,
		detail::is_element_of<tag::is_floating_point, typename T::_traits::_tags>::value>;
	SQLPP_VALUE_TRAIT_GENERATOR(is_text);
	SQLPP_VALUE_TRAIT_GENERATOR(is_wrapped_value);
	SQLPP_VALUE_TRAIT_GENERATOR(is_selectable);
	SQLPP_VALUE_TRAIT_GENERATOR(is_multi_expression);
	SQLPP_VALUE_TRAIT_GENERATOR(is_alias);
	SQLPP_VALUE_TRAIT_GENERATOR(is_select_flag);
	SQLPP_VALUE_TRAIT_GENERATOR(is_result_field);

	SQLPP_VALUE_TRAIT_GENERATOR(must_not_insert);
	SQLPP_VALUE_TRAIT_GENERATOR(must_not_update);
	SQLPP_VALUE_TRAIT_GENERATOR(require_insert);
	SQLPP_VALUE_TRAIT_GENERATOR(trivial_value_is_null);
	SQLPP_VALUE_TRAIT_GENERATOR(null_is_trivial_value);

	SQLPP_VALUE_TRAIT_GENERATOR(is_with);
	SQLPP_VALUE_TRAIT_GENERATOR(is_cte);
	SQLPP_VALUE_TRAIT_GENERATOR(is_statement);
	SQLPP_VALUE_TRAIT_GENERATOR(is_prepared_statement);
	SQLPP_VALUE_TRAIT_GENERATOR(is_noop);
	SQLPP_VALUE_TRAIT_GENERATOR(is_missing);
	SQLPP_VALUE_TRAIT_GENERATOR(is_return_value);
	SQLPP_VALUE_TRAIT_GENERATOR(is_table);
	SQLPP_VALUE_TRAIT_GENERATOR(is_join);
	SQLPP_VALUE_TRAIT_GENERATOR(is_pseudo_table);
	SQLPP_VALUE_TRAIT_GENERATOR(is_column);
	SQLPP_VALUE_TRAIT_GENERATOR(is_select);
	SQLPP_VALUE_TRAIT_GENERATOR(is_select_flag_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_select_column_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_from);
	SQLPP_VALUE_TRAIT_GENERATOR(is_single_table);
	SQLPP_VALUE_TRAIT_GENERATOR(is_into);
	SQLPP_VALUE_TRAIT_GENERATOR(is_extra_tables);
	SQLPP_VALUE_TRAIT_GENERATOR(is_on);
	SQLPP_VALUE_TRAIT_GENERATOR(is_where);
	SQLPP_VALUE_TRAIT_GENERATOR(is_group_by);
	SQLPP_VALUE_TRAIT_GENERATOR(is_having);
	SQLPP_VALUE_TRAIT_GENERATOR(is_order_by);
	SQLPP_VALUE_TRAIT_GENERATOR(is_limit);
	SQLPP_VALUE_TRAIT_GENERATOR(is_offset);
	SQLPP_VALUE_TRAIT_GENERATOR(is_union);
	SQLPP_VALUE_TRAIT_GENERATOR(is_using_);
	SQLPP_VALUE_TRAIT_GENERATOR(is_column_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_multi_column);
	SQLPP_VALUE_TRAIT_GENERATOR(is_value_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_assignment);
	SQLPP_VALUE_TRAIT_GENERATOR(is_update_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_insert_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_insert_value);
	SQLPP_VALUE_TRAIT_GENERATOR(is_insert_value_list);
	SQLPP_VALUE_TRAIT_GENERATOR(is_sort_order);
	SQLPP_VALUE_TRAIT_GENERATOR(is_parameter);

	SQLPP_VALUE_TRAIT_GENERATOR(requires_braces);

	SQLPP_VALUE_TRAIT_GENERATOR(enforce_null_result_treatment);

	template<typename Tag, bool Condition>
		using tag_if = typename std::conditional<Condition, Tag, void>::type;

	template<typename Database>
		using is_database = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

	namespace detail
	{
		template<typename... T>
			using make_parameter_tuple_t = decltype(std::tuple_cat(std::declval<T>()...));
	}
	template<typename T>
		using value_type_of = typename T::_traits::_value_type;

	template<typename T>
		using cpp_value_type_of = typename value_type_of<T>::_cpp_value_type;

	template<typename T>
		using required_ctes_of = typename T::_recursive_traits::_required_ctes;

	template<typename T>
		using provided_ctes_of = typename T::_recursive_traits::_provided_ctes;

	template<typename T>
		using required_tables_of = typename T::_recursive_traits::_required_tables;

	template<typename T>
		using provided_tables_of = typename T::_recursive_traits::_provided_tables;

	template<typename T>
		using provided_outer_tables_of = typename T::_recursive_traits::_provided_outer_tables;

	template<typename T>
		using extra_tables_of = typename T::_recursive_traits::_extra_tables;

	template<typename T>
		using parameters_of = typename T::_recursive_traits::_parameters;

	template<typename T>
		using recursive_tags_of = typename T::_recursive_traits::_tags;

	template<typename T>
		using alias_of = typename T::_alias_t;

	template<typename T>
		using name_of = typename T::_alias_t::_name_t;

	template<typename ValueType, typename... Tags>
		struct make_traits
		{
			using _value_type = ValueType;
			using _tags = detail::make_type_set_t<typename ValueType::_tag, Tags...>;
		};

	template<typename... Arguments>
		struct make_recursive_traits
		{
			using _required_ctes = detail::make_joined_set_t<required_ctes_of<Arguments>...>;
			using _provided_ctes = detail::make_joined_set_t<required_ctes_of<Arguments>...>;
			using _required_tables = detail::make_joined_set_t<required_tables_of<Arguments>...>;
			using _provided_tables = detail::make_joined_set_t<provided_tables_of<Arguments>...>;
			using _provided_outer_tables = detail::make_joined_set_t<provided_outer_tables_of<Arguments>...>;
			using _extra_tables = detail::make_joined_set_t<extra_tables_of<Arguments>...>;
			using _parameters = detail::make_parameter_tuple_t<parameters_of<Arguments>...>;
			using _tags = detail::make_joined_set_t<recursive_tags_of<Arguments>...>;
		};

	template<typename... Tags>
		struct recursive_tags
		{
			using _required_ctes = detail::type_set<>;
			using _provided_ctes = detail::type_set<>;
			using _required_tables = detail::type_set<>;
			using _provided_tables = detail::type_set<>;
			using _provided_outer_tables = detail::type_set<>;
			using _extra_tables = detail::type_set<>;
			using _parameters = std::tuple<>;
			using _tags = detail::type_set<Tags...>;
		};

	struct aggregate_function
	{
		struct _traits { using _value_type = void; using _tags = detail::type_set<>; };
		using _recursive_traits = recursive_tags<tag::contains_aggregate_function>;
	};

	template<typename NameProvider, typename Member>
		using member_t = typename NameProvider::_alias_t::template _member_t<Member>;

	template<typename Policies>
		using derived_statement_t = typename Policies::_statement_t;

	struct consistent_t
	{
		using type = std::true_type;

		static void _() {};
	};

	template<typename T>
	using is_inconsistent_t = typename std::conditional<std::is_same<consistent_t, T>::value, std::false_type, std::true_type>::type;

	template<typename Context, typename... T>
	using serialize_check_of = detail::get_first_if<is_inconsistent_t, consistent_t, typename serializer_t<Context, T>::_serialize_check...>;

	struct assert_run_statement_or_prepared_t
	{
		using type = std::false_type;

		template<typename T = void>
			static void _()
			{
				static_assert(wrong_t<T>::value, "connection cannot run something that is neither statement nor prepared statement");
			};
	};

	struct assert_prepare_statement_t
	{
		using type = std::false_type;

		template<typename T = void>
			static void _()
			{
				static_assert(wrong_t<T>::value, "connection cannot prepare something that is not a statement");
			};
	};

	template<typename T, typename Enable = void>
		struct run_check
		{
			using type = assert_run_statement_or_prepared_t;
		};

	template<typename T>
		struct run_check<T, typename std::enable_if<is_statement_t<T>::value or is_prepared_statement_t<T>::value>::type>
		{
			using type = typename T::_run_check;
		};

	template<typename T>
		using run_check_t = typename run_check<T>::type;

	template<typename T, typename Enable = void>
		struct prepare_check
		{
			using type = assert_prepare_statement_t;
		};

	template<typename T>
		struct prepare_check<T, typename std::enable_if<is_statement_t<T>::value>::type>
		{
			using type = typename T::_prepare_check;
		};

	template<typename T>
		using prepare_check_t = typename prepare_check<T>::type;


	template<typename Context, typename T, typename Enable = void>
		struct serialize_check
		{
			using type = serialize_check_of<Context, T>;
		};

	template<typename Context, typename T>
		struct serialize_check<Context, T, typename std::enable_if<is_prepared_statement_t<T>::value>::type>
		{
			using type = consistent_t; // this is already serialized
		};

	template<typename Context, typename T>
		using serialize_check_t = typename serialize_check<Context, T>::type;

	template<typename Statement, typename Enable = void>
		struct has_result_row_impl
		{
			using type = std::false_type;
		};

	template<typename Statement>
		struct has_result_row_impl<Statement, 
		typename std::enable_if<
			not wrong_t<typename Statement::template _result_methods_t<Statement>::template _result_row_t<void>>::value, 
		void>::type>
		{
			using type = std::true_type;
		};

	template<typename Statement>
		using has_result_row_t = typename has_result_row_impl<Statement>::type;

	template<typename Statement, typename Enable = void>
		struct get_result_row_impl
		{
			using type = void;
		};

	template<typename Statement>
		struct get_result_row_impl<Statement, 
		typename std::enable_if<
			not wrong_t<typename Statement::template _result_methods_t<Statement>::template _result_row_t<void>>::value, 
		void>::type>
		{
			using type = typename Statement::template _result_methods_t<Statement>::template _result_row_t<void>;
		};

	template<typename Statement>
		using get_result_row_t = typename get_result_row_impl<Statement>::type;

	template<typename Statement, template<typename> class Predicate, typename Enable=void>
		struct has_policy_impl
		{
			using type = std::false_type;
		};

	template<typename Statement, template<typename> class Predicate>
		struct has_policy_impl<Statement, Predicate, typename std::enable_if<is_statement_t<Statement>::value>::type>
		{
			using type = std::true_type;
		};

	template<typename Statement, template<typename> class Predicate>
		using has_policy_t = typename has_policy_impl<Statement, Predicate>::type;

}

#endif
