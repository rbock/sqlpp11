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
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
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

#define SQLPP_IS_VALUE_TRAIT_GENERATOR(name) \
	namespace tag\
	{\
		struct name{};\
	};\
	namespace detail\
	{\
		template<typename T, typename Enable = void>\
		struct is_##name##_impl { using type = std::false_type; };\
		template<typename T>\
		struct is_##name##_impl<T, typename std::enable_if<detail::is_element_of<tag::name, typename T::_traits::_tags>::value>::type> { using type = std::true_type; };\
	}\
	template<typename T>\
	using is_##name##_t = typename detail::is_##name##_impl<T>::type;

#define SQLPP_TYPE_TRAIT_GENERATOR(name) \
	namespace detail\
	{\
		template<typename T, typename Enable = void>\
		struct name##_impl: std::false_type {};\
		template<typename T>\
		struct name##_impl<T, typename std::enable_if<std::is_same<typename T::_##name, std::true_type>::value>::type>: std::true_type {};\
	}\
	template<typename T>\
	struct name##_t: detail::name##_impl<T> {};

#define SQLPP_CONNECTOR_TRAIT_GENERATOR(name) \
	namespace detail\
	{\
		template<typename T, typename Enable = void>\
		struct connector_##name##_impl: std::false_type {};\
		template<typename T>\
		struct connector_##name##_impl<T, typename std::enable_if<std::is_same<typename T::_tags::_##name, std::true_type>::value>::type>: std::true_type {};\
	}\
	template<typename T>\
	struct connector_##name##_t: detail::connector_##name##_impl<T> {};

	SQLPP_IS_VALUE_TRAIT_GENERATOR(boolean);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(integral);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(floating_point);
	template<typename T>
		using is_numeric_t = detail::any_t<
		detail::is_element_of<tag::integral, typename T::_traits::_tags>::value,
		detail::is_element_of<tag::floating_point, typename T::_traits::_tags>::value>;
	SQLPP_IS_VALUE_TRAIT_GENERATOR(text);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(wrapped_value);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(expression);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(named_expression);
	namespace tag
	{
		template<typename C>
			using named_expression_if = typename std::conditional<C::value, tag::named_expression, void>::type;
	}
	namespace tag
	{
		template<typename C>
			using expression_if = typename std::conditional<C::value, tag::expression, void>::type;
	}
	SQLPP_IS_VALUE_TRAIT_GENERATOR(multi_expression);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(alias);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(select_flag);

	SQLPP_VALUE_TRAIT_GENERATOR(must_not_insert);
	SQLPP_VALUE_TRAIT_GENERATOR(must_not_update);
	SQLPP_VALUE_TRAIT_GENERATOR(require_insert);
	SQLPP_VALUE_TRAIT_GENERATOR(can_be_null);
	SQLPP_VALUE_TRAIT_GENERATOR(trivial_value_is_null);

	SQLPP_IS_VALUE_TRAIT_GENERATOR(noop);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(missing);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(return_value);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(table);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(join);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(pseudo_table);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(column);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(select);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(select_flag_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(select_column_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(from);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(single_table);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(into);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(extra_tables);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(on);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(where);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(group_by);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(having);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(order_by);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(limit);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(offset);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(using_);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(column_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(multi_column);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(value_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(assignment);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(update_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(insert_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(insert_value);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(insert_value_list);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(sort_order);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(parameter);

	SQLPP_TYPE_TRAIT_GENERATOR(requires_braces);

	SQLPP_CONNECTOR_TRAIT_GENERATOR(null_result_is_trivial_value);
	SQLPP_CONNECTOR_TRAIT_GENERATOR(assert_result_validity);

	template<typename Database>
		using is_database = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

	template<typename T, template<typename> class IsTag>
		using copy_type_trait = typename std::conditional<IsTag<T>::value, std::true_type, std::false_type>::type;

	namespace detail
	{
		template<typename T>
			struct value_type_of_impl
			{
				using type = typename T::_traits::_value_type;
			};

		template<typename T>
			struct required_table_of_impl
			{
				using type = typename T::_recursive_traits::_required_tables;
			};

		template<typename T>
			struct provided_table_of_impl
			{
				using type = typename T::_recursive_traits::_provided_tables;
			};

		template<typename T>
			struct extra_table_of_impl
			{
				using type = typename T::_recursive_traits::_extra_tables;
			};

		template<typename T>
			struct parameters_of_impl
			{
				using type = typename T::_recursive_traits::_parameters;
			};

		template<typename T>
			struct name_of_impl
			{
				using type = typename T::_name_t;
			};

		template<typename... T>
			struct make_parameter_tuple_impl
			{
				using type = decltype(std::tuple_cat(std::declval<T>()...));
			};

		template<typename... T>
			using make_parameter_tuple_t = typename make_parameter_tuple_impl<T...>::type;
	}
	template<typename T>
		using value_type_of = typename detail::value_type_of_impl<T>::type;

	template<typename T>
		using required_tables_of = typename detail::required_table_of_impl<T>::type;

	template<typename T>
		using provided_tables_of = typename detail::provided_table_of_impl<T>::type;

	template<typename T>
		using extra_tables_of = typename detail::extra_table_of_impl<T>::type;

	template<typename T>
		using parameters_of = typename detail::parameters_of_impl<T>::type;

	template<typename T>
		using name_of = typename detail::name_of_impl<T>::type;

	template<typename ValueType, typename... Tags>
		struct make_traits
		{
			using _value_type = ValueType;
			using _tags = detail::make_type_set_t<typename ValueType::_tag, Tags...>;
		};
	template<typename... Arguments>
		struct make_recursive_traits
		{
			using _required_tables = detail::make_joined_set_t<required_tables_of<Arguments>...>;
			using _provided_tables = detail::make_joined_set_t<provided_tables_of<Arguments>...>;
			using _extra_tables = detail::make_joined_set_t<extra_tables_of<Arguments>...>;
			using _parameters = detail::make_parameter_tuple_t<parameters_of<Arguments>...>;
		};

}

#endif
