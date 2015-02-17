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

#ifndef SQLPP_RESULT_FIELD_METHODS_H
#define SQLPP_RESULT_FIELD_METHODS_H

#include <utility>
#include <sqlpp11/basic_expression_operators.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Field>
			struct get_field_spec_impl
		{
			static_assert(wrong_t<get_field_spec_impl>::value, "Invalid argument for get_field_spec");
		};

		template<template<typename, typename, typename> class Field, typename ValueType, typename Db, typename FieldSpec>
			struct get_field_spec_impl<Field<ValueType, Db, FieldSpec>>
		{
			using type = FieldSpec;
		};

		template<typename Field>
			using get_field_spec_t = typename get_field_spec_impl<Field>::type;
	}

	template<typename Field, typename Enable = void>
		struct result_field_methods_base_t
		{
			using _field_spec_t = detail::get_field_spec_t<Field>;
			static constexpr bool _null_is_trivial = true;
			operator cpp_value_type_of<_field_spec_t>() const { return static_cast<const Field&>(*this).value(); }
		};

	template<template<typename, typename, typename> class Field, typename ValueType, typename Db, typename FieldSpec>
		struct result_field_methods_base_t<
				Field<ValueType, Db, FieldSpec>, 
				typename std::enable_if<enforce_null_result_treatment_t<Db>::value 
					and column_spec_can_be_null_t<FieldSpec>::value
					and not null_is_trivial_value_t<FieldSpec>::value>::type>
		{
			using _field_spec_t = FieldSpec;
			static constexpr bool _null_is_trivial = false;
		};

	template<typename Field>
		struct result_field_methods_t: public result_field_methods_base_t<Field>,
		                               public alias_operators<Field>
		{
			using _base_t = result_field_methods_base_t<Field>;
			using _field_spec_t = typename _base_t::_field_spec_t;

			using _traits = make_traits<value_type_of<_field_spec_t>,
						tag::is_result_field,
						tag::is_expression, 
						tag_if<tag::null_is_trivial_value, _base_t::_null_is_trivial>>;

			using _nodes = std::tuple<>;
			using _can_be_null = column_spec_can_be_null_t<_field_spec_t>;
		};

}
#endif
