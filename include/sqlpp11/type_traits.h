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

#ifndef SQLPP_TYPE_TRAITS_H
#define SQLPP_TYPE_TRAITS_H

#include <type_traits>
#include <sqlpp11/vendor/wrap_operand.h>

namespace sqlpp
{
#define SQLPP_IS_VALUE_TRAIT_GENERATOR(name) \
	namespace detail\
	{\
		template<typename T, typename Enable = void>\
			struct is_##name##_impl: std::false_type {};\
		template<typename T>\
			struct is_##name##_impl<T, typename std::enable_if<std::is_same<typename T::_value_type::_is_##name, std::true_type>::value>::type>: std::true_type {};\
	}\
	template<typename T>\
		struct is_##name##_t: detail::is_##name##_impl<T> {};

#define SQLPP_IS_COLUMN_TRAIT_GENERATOR(name) \
	namespace detail\
	{\
		template<typename T, typename Enable = void>\
			struct name##_impl { using type = std::false_type; };\
		template<typename T>\
			struct name##_impl<T, typename std::enable_if<std::is_same<typename T::_column_type::_##name, std::true_type>::value>::type> { using type = std::true_type; };\
	}\
	template<typename T>\
		using name##_t = typename detail::name##_impl<T>::type;

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
	SQLPP_IS_VALUE_TRAIT_GENERATOR(numeric);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(integral);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(floating_point);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(text);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(value);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(expression);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(named_expression);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(multi_expression);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(alias);
	SQLPP_IS_VALUE_TRAIT_GENERATOR(select_flag);

	SQLPP_IS_COLUMN_TRAIT_GENERATOR(must_not_insert);
	SQLPP_IS_COLUMN_TRAIT_GENERATOR(must_not_update);
	SQLPP_IS_COLUMN_TRAIT_GENERATOR(require_insert);
	SQLPP_IS_COLUMN_TRAIT_GENERATOR(can_be_null);

	SQLPP_TYPE_TRAIT_GENERATOR(is_table);
	SQLPP_TYPE_TRAIT_GENERATOR(is_join);
	SQLPP_TYPE_TRAIT_GENERATOR(is_pseudo_table);
	SQLPP_TYPE_TRAIT_GENERATOR(is_column);
	SQLPP_TYPE_TRAIT_GENERATOR(is_select);
	SQLPP_TYPE_TRAIT_GENERATOR(is_select_flag_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_select_column_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_from);
	SQLPP_TYPE_TRAIT_GENERATOR(is_on);
	SQLPP_TYPE_TRAIT_GENERATOR(is_dynamic);
	SQLPP_TYPE_TRAIT_GENERATOR(is_where);
	SQLPP_TYPE_TRAIT_GENERATOR(is_group_by);
	SQLPP_TYPE_TRAIT_GENERATOR(is_having);
	SQLPP_TYPE_TRAIT_GENERATOR(is_order_by);
	SQLPP_TYPE_TRAIT_GENERATOR(is_limit);
	SQLPP_TYPE_TRAIT_GENERATOR(is_offset);
	SQLPP_TYPE_TRAIT_GENERATOR(is_using);
	SQLPP_TYPE_TRAIT_GENERATOR(is_column_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_multi_column);
	SQLPP_TYPE_TRAIT_GENERATOR(is_value_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_assignment);
	SQLPP_TYPE_TRAIT_GENERATOR(is_update_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_insert_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_insert_value);
	SQLPP_TYPE_TRAIT_GENERATOR(is_insert_value_list);
	SQLPP_TYPE_TRAIT_GENERATOR(is_sort_order);
	SQLPP_TYPE_TRAIT_GENERATOR(requires_braces);
	SQLPP_TYPE_TRAIT_GENERATOR(is_parameter);

	SQLPP_CONNECTOR_TRAIT_GENERATOR(has_empty_list_insert);

	template<typename T, template<typename> class IsTag>
		using copy_type_trait = typename std::conditional<IsTag<T>::value, std::true_type, std::false_type>::type;

	template<typename T, template<typename> class IsCorrectType>
		struct operand_t
		{
			using type = typename vendor::wrap_operand<T>::type;
			static_assert(not is_alias_t<type>::value, "expression operand must not be an alias");
			static_assert(is_expression_t<type>::value, "expression required");
			static_assert(IsCorrectType<type>::value, "invalid operand type");
		};
	
}

#endif
